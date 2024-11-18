/*
 * @Author: chenjingyu
 * @Date: 2023-06-19 17:37:42
 * @LastEditTime: 2023-08-19 22:33:32
 * @Description: palm detector module
 * @FilePath: \Mediapipe-MNN\source\PalmDetector.cc
 */
#include "PalmDetector.h"
#include "CommonData.h"
#include "Utils.h"
#include <iostream>


using namespace MNN;
namespace mirror {
PalmDetector::~PalmDetector() {
  net_->releaseModel();
  net_->releaseSession(sess_);
}

bool PalmDetector::LoadModel(const char *model_file) {
  std::cout << "Start load model." << std::endl;
  // 1.load model
  net_ = std::unique_ptr<Interpreter>(Interpreter::createFromFile(model_file));
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

void PalmDetector::setFormat(int format) {
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

bool PalmDetector::Detect(const ImageHead &in, RotateType type,
                          std::vector<ObjectInfo> &objects) {
  std::cout << "Start detect." << std::endl;
  objects.clear();
  if (in.data == nullptr) {
    std::cout << "Error Input empty." << std::endl;
    return false;
  }

  if (!inited_) {
    std::cout << "Model Uninitialized." << std::endl;
    return false;
  }  
  // 1.set input
  int width = in.width;
  int height = in.height;
  // clang-format off
  std::vector<Point2f> input_region = getInputRegion(in, type, input_w_, input_h_);
  float points_src[] = {
    input_region[0].x, input_region[0].y,
    input_region[1].x, input_region[1].y,
    input_region[2].x, input_region[2].y,
    input_region[3].x, input_region[3].y,
  };
  float points_dst[] = {
    0.0f, 0.0f,
    0.0f, (float)(input_h_ - 1),
    (float)(input_w_ - 1), 0.0f,
    (float)(input_w_ - 1), (float)(input_h_ - 1),
  };
  // clang-format on
  trans_.setPolyToPoly((CV::Point *)points_dst, (CV::Point *)points_src, 4);
  pretreat_->setMatrix(trans_);
  pretreat_->convert((uint8_t *)in.data, width, height, in.width_step, input_tensor_);

  // 2.do inference
  int ret = net_->runSession(sess_);
  if (ret != 0) {
    std::cout << "Failed do inference." << std::endl;
    return -1;
  }

  // 3.get the result
  MNN::Tensor *classifier = net_->getSessionOutput(sess_, "classificators");
  MNN::Tensor *regressor = net_->getSessionOutput(sess_, "regressors");
  if (nullptr == classifier || nullptr == regressor) {
    std::cout << "Error output." << std::endl;
    return false;
  }
  std::shared_ptr<MNN::Tensor> output_classifier(
      new MNN::Tensor(classifier, classifier->getDimensionType()));
  std::shared_ptr<MNN::Tensor> output_regressor(
      new MNN::Tensor(regressor, regressor->getDimensionType()));
  classifier->copyToHostTensor(output_classifier.get());
  regressor->copyToHostTensor(output_regressor.get());

  // 4.parse the result
  ParseOutputs(output_classifier.get(), output_regressor.get(), objects);
  NMSObjects(objects, iou_thresh_);

  std::cout << "End detect." << std::endl;
  return true;
}

void PalmDetector::ParseOutputs(MNN::Tensor *scores, MNN::Tensor *boxes,
                                std::vector<ObjectInfo> &objects) {
  objects.clear();
  float *scores_ptr = scores->host<float>();
  float *boxes_ptr = boxes->host<float>();

  int batch = scores->batch();
  int channel = scores->channel();
  int height = scores->height();
  int width = scores->width();

  ObjectInfo object;
  for (int i = 0; i < channel; ++i) {
    float score = sigmoid(scores_ptr[i]);
    if (score < score_thresh_) continue;
    float offset_x = BLAZE_PALM_ANCHORS[4 * i + 0] * input_w_;
    float offset_y = BLAZE_PALM_ANCHORS[4 * i + 1] * input_h_;
    float *ptr = boxes_ptr + 18 * i;
    // 1.parse the box information
    float cx = ptr[0] + offset_x;
    float cy = ptr[1] + offset_y;
    float w = ptr[2];
    float h = ptr[3];

    object.score = score;
    Point2f tl, br, tl_origin, br_origin;
    tl.x = cx - 0.5f * w;
    tl.y = cy - 0.5f * h;
    br.x = cx + 0.5f * w;
    br.y = cy + 0.5f * h;

    // 2.parse the index landmarks
    Point2f src, dst;
    src.x = ptr[4] + offset_x;
    src.y = ptr[5] + offset_y;
    dst.x = ptr[8] + offset_x;
    dst.y = ptr[9] + offset_y; 

    float dx = dst.x - src.x;
    float dy = dst.y - src.y;
    object.angle = -(kTargetPalmAngle - std::atan2(-dy, dx) * 180.0f / M_PI);

    object.landmarks.resize(7);
    Point2f pt;
    for (int k = 0; k < 7; ++k) {      
      pt.x = ptr[4 + 2 * k + 0] + offset_x;
      pt.y = ptr[4 + 2 * k + 1] + offset_y;
      object.landmarks[k].x = trans_[0] * pt.x + trans_[1] * pt.y + trans_[2];
      object.landmarks[k].y = trans_[3] * pt.x + trans_[4] * pt.y + trans_[5];
    }
    
    tl_origin.x = trans_[0] * tl.x + trans_[1] * tl.y + trans_[2];
    tl_origin.y = trans_[3] * tl.x + trans_[4] * tl.y + trans_[5];
    br_origin.x = trans_[0] * br.x + trans_[1] * br.y + trans_[2];
    br_origin.y = trans_[3] * br.x + trans_[4] * br.y + trans_[5];

    object.rect.left = MIN_(tl_origin.x, br_origin.x);
    object.rect.top = MIN_(tl_origin.y, br_origin.y);
    object.rect.right = MAX_(tl_origin.x, br_origin.x);
    object.rect.bottom = MAX_(tl_origin.y, br_origin.y);

    objects.emplace_back(object);
  }
}

} // namespace mirror
