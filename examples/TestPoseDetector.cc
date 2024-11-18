/*
 * @Author: chenjingyu
 * @Date: 2023-08-19 23:15:40
 * @LastEditTime: 2023-08-19 23:19:51
 * @Description: Test Pose Detector
 * @FilePath: \Mediapipe-MNN\examples\TestPoseDetector.cc
 */
#include "TypeDefines.h"
#include "PoseDetector.h"
#include "PoseLandmarkDetector.h"
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace mirror;

int main(int argc, char *argv[]) {
  const char *image_file = "../data/images/image.jpg";
  cv::Mat image = cv::imread(image_file);
  if (image.empty()) {
    std::cout << "failed load image." << std::endl;
    return -1;
  }
  RotateType type = RotateType::CLOCKWISE_ROTATE_0;
  if (type == CLOCKWISE_ROTATE_90) {
    cv::transpose(image, image);
  } else if (type == CLOCKWISE_ROTATE_180) {
    cv::flip(image, image, -1);
  }
  if (type == CLOCKWISE_ROTATE_270) {
    cv::transpose(image, image);
    cv::flip(image, image, 1);
  }
  ImageHead in;
  in.data = image.data;
  in.height = image.rows;
  in.width = image.cols;
  in.width_step = image.step[0];
  in.pixel_format = PixelFormat::BGR;

  const char *pose_model_file = "../data/models/pose_detector_fp16.mnn";
  const char *pose_landmark_model_file = "../data/models/pose_landmarks_detector_fp16.mnn";
  PoseDetector detector;
  PoseLandmarkDetector landmarker;
  if (!detector.LoadModel(pose_model_file) ||
      !landmarker.LoadModel(pose_landmark_model_file)) {
    std::cout << "Failed load model." << std::endl;
    return -1;
  }
  detector.setFormat(in.pixel_format);
  landmarker.setFormat(in.pixel_format);

  std::vector<ObjectInfo> objects;
  detector.Detect(in, type, objects);
  landmarker.Detect(in, type, objects);
  for (const auto &object : objects) {
    cv::rectangle(image, cv::Point2f(object.rect.left, object.rect.top),
                  cv::Point2f(object.rect.right, object.rect.bottom),
                  cv::Scalar(255, 0, 255), 2);    
    for (int i = 0; i < object.landmarks.size(); ++i) {
       cv::Point pt = cv::Point(
         (int)object.landmarks[i].x,                               
         (int)object.landmarks[i].y
      );
      cv::circle(image, pt, 2, cv::Scalar(255, 255, 0));
      cv::putText(image, std::to_string(i), pt, 1, 1.0, cv::Scalar(255, 0, 255));
    }

    for (int i = 0; i < object.landmarks3d.size(); ++i) {
      cv::Point pt = cv::Point(
        (int)object.landmarks3d[i].x,                               
        (int)object.landmarks3d[i].y
      );
      cv::circle(image, pt, 2, cv::Scalar(255, 0, 255));
    }
  }

  cv::imshow("result", image);
  cv::waitKey(0);
  cv::imwrite("../data/results/mediapipe_pose.jpg", image);

  return 0;
}