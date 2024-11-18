#include "PalmLandmarkDetector.h"
#include "Utils.h"
#include <cfloat>
#include <iostream>
#include "hand_landmark_lite.h"

using namespace MNN;
namespace HandLib {
PalmLandmarkDetector::~PalmLandmarkDetector() {
  net_->releaseModel();
  net_->releaseSession(sess_);
}

bool PalmLandmarkDetector::LoadModel() {
  std::cout << "Start load model." << std::endl;
  // 1.load model
  net_ = std::unique_ptr<Interpreter>(Interpreter::createFromBuffer(hand_landmark_lite, 2072844));
  if (net_ == nullptr) {
    std::cout << "Failed load model." << std::endl;
    return false;
  }

  // 2.create session
  ScheduleConfig schedule_config;
  schedule_config.type = MNN_FORWARD_CPU;
  schedule_config.numThread = 4;
  sess_ = net_->createSession(schedule_config);
  input_tensor_ = net_->getSessionInput(sess_, nullptr);

  input_h_ = input_tensor_->height();
  input_w_ = input_tensor_->width();

  std::cout << "End load model." << std::endl;
  inited_ = true;
  return true;
}

void PalmLandmarkDetector::setFormat(int format) {
  // create image process
  CV::ImageProcess::Config image_process_config;
  image_process_config.filterType = CV::BILINEAR;
  image_process_config.sourceFormat = CV::ImageFormat(format);
  image_process_config.destFormat = CV::RGB;
  image_process_config.wrap = CV::ZERO;
  memcpy(image_process_config.mean, meanVals_, sizeof(meanVals_));
  memcpy(image_process_config.normal, normVals_, sizeof(normVals_));
  pretreat_ = std::shared_ptr<CV::ImageProcess>(
      CV::ImageProcess::create(image_process_config));
}

bool PalmLandmarkDetector::Detect(const ImageHead &in, RotateType type,
                                std::vector<ObjectInfo> &objects) {
  // std::cout << "Start detect." << std::endl;
  if (in.data == nullptr) {
    std::cout << "Error Input empty." << std::endl;
    return false;
  }

  if (!inited_) {
    std::cout << "Model Uninitialized." << std::endl;
    return false;
  }
  int width = in.width;
  int height = in.height;
  for (auto &object : objects) {
    // std::vector<Point2f> region = getInputRegion(in, type, object, 2.6f, 0.0f, -0.5f);
    std::vector<Point2f> region = getInputRegion(in, type, object.rect, input_h_, input_w_, object.angle, 2.6f, 0.0f, -0.5f);
    float points_src[] = {
      region[0].x, region[0].y, region[1].x, region[1].y,
      region[2].x, region[2].y, region[3].x, region[3].y,
    };
    float points_dst[] = {
      0.0f,  0.0f,
      0.0f,  (float)(input_h_ - 1),
      (float)(input_w_ - 1), 0.0f,
      (float)(input_w_ - 1), (float)(input_h_ - 1),
    };
    trans_.setPolyToPoly((CV::Point *)points_dst, (CV::Point *)points_src, 4);
    pretreat_->setMatrix(trans_);
    pretreat_->convert((uint8_t *)in.data, width, height, in.width_step, input_tensor_);


    // 1.5.do inference
    int ret = net_->runSession(sess_);
    if (ret != 0) {
      std::cout << "Failed do inference." << std::endl;
      return -1;
    }

    // 1.6 get the result
    MNN::Tensor *palm_score = net_->getSessionOutput(sess_, "Identity_1");
    MNN::Tensor *left_right = net_->getSessionOutput(sess_, "Identity_2");
    MNN::Tensor *landmark_norm = net_->getSessionOutput(sess_, "Identity");
    MNN::Tensor *landmark_world = net_->getSessionOutput(sess_, "Identity_3");

    std::shared_ptr<MNN::Tensor> output_left_right(
        new MNN::Tensor(left_right, left_right->getDimensionType()));
    std::shared_ptr<MNN::Tensor> output_palm_score(
        new MNN::Tensor(palm_score, palm_score->getDimensionType()));
    std::shared_ptr<MNN::Tensor> output_landmark_norm(
        new MNN::Tensor(landmark_norm, landmark_norm->getDimensionType()));
    std::shared_ptr<MNN::Tensor> output_landmark_world(
        new MNN::Tensor(landmark_world, landmark_world->getDimensionType()));

    left_right->copyToHostTensor(output_left_right.get());
    palm_score->copyToHostTensor(output_palm_score.get());
    landmark_norm->copyToHostTensor(output_landmark_norm.get());
    landmark_world->copyToHostTensor(output_landmark_world.get());

    float *palm_score_ptr = output_palm_score->host<float>();
    if (palm_score_ptr[0] < 0.8f) continue;

    float *left_right_ptr = output_left_right->host<float>();
    if (left_right_ptr[0] > 0.5f) {
      object.left_right = 1;
    } else {
      object.left_right = 0;
    }

    object.landmarks.resize(kNumPalmLandmarks);
    object.landmarks3d.resize(kNumPalmLandmarks);
    float *landmarks_ptr = output_landmark_norm->host<float>();
    float *landmarks3d_ptr = output_landmark_world->host<float>();
    for (int k = 0; k < kNumPalmLandmarks; ++k) {
      float x = landmarks_ptr[3 * k + 0];
      float y = landmarks_ptr[3 * k + 1];
      object.landmarks[k].x = trans_[0] * x + trans_[1] * y + trans_[2];
      object.landmarks[k].y = trans_[3] * x + trans_[4] * y + trans_[5];

      x = landmarks3d_ptr[3 * k + 0];
      y = landmarks3d_ptr[3 * k + 1];
      object.landmarks3d[k].x = trans_[0] * x + trans_[1] * y + trans_[2];
      object.landmarks3d[k].y = trans_[3] * x + trans_[4] * y + trans_[5];
      object.landmarks3d[k].z = landmarks3d_ptr[3 * k + 2];   // z is only a scale
    }
  }

  return true;
}

} // namespace HandLib
