# mediapipe mnn inference
 - Author: chenjingyu 20230619

## 1.项目说明
 - mediapipe模型推理测试代码；

## 2.更新日志
时间 | 更新内容
--|--
20230619 | 1.创建项目；2.添加相关依赖库；
20230620 | 1.添加相关转换后mnn模型；
20230625 | 1.添加手掌检测相关推理代码；
20230626 | 1.添加手掌关键点模型相关推理代码；
20230727 | 1.添加推理测试代码；
20230729 | 1.添加人脸检测相关模型及基本推理代码；2.添加人脸检测相关解析代码及测试代码；3.添加Blazeface的大模型推理代码；
20230730 | 1.添加Mediapipe的face mesh推理代码；2.添加MNN的人脸检测模型推理代码；
20230802 | 1.添加最新版本的YuNet推理代码；
20230814 | 1.添加image embedding相关推理代码；
20230819 | 1.添加Knift模型推理代码；
20230820 | 1.添加BlazePose人体检测模型及推理代码；2.添加BlazePose人体关键点检测模型及推理代码；3.添加图像分类模型及推理代码；
20230822 | 1.添加mediapipe的selfie_segmentation模型推理demo；

## 3.一些效果
示例 | 效果图
--|--
mediapipe-classifier | ![mediapipe-classifier](./data/results/mediapipe_classifier_result.jpg)
mediapipe-face | ![mediapipe-face](./data/results/mediapipe_face.jpg)
mediapipe-hand | ![mediapipe-hand](./data/results/mediapipe_hand.jpg)
mediapipe-pose | ![mediapipe-pose](./data/results/mediapipe_pose.jpg)
mediapipe-knift | ![mediapipe-knift](./data/results/mediapipe_knift.jpg)
mediapipe-embbeding | ![mediapipe-embedding](./data/results/mediapipe-embbeding.jpg)
mediapipe-selfie_segmentation | ![mediapipe-selfie_segmentation](./data/results/mediapipe_segment_result.jpg)
YN-facedetection | ![YN-facedetection](./data/results/YN_face.jpg)
Lite-facedetection | ![Lite-facedetection](./data/results/lite_face.jpg)

- 实测knift效果很差，大概率是我这边接的有问题，但是没发现是什么问题，脑壳疼！后面再研究一下mediapipe源码看看。

## 4.参考项目
- [1] [mediapipe](https://github.com/google/mediapipe)

## 5.可视化网络输入
```C++
#include <opencv2/opencv.hpp>

using namespace MNN;
CV::ImageProcess::Config image_process_config;
image_process_config.filterType = CV::BILINEAR;
image_process_config.sourceFormat = CV::BGR;
image_process_config.destFormat = CV::BGR;
image_process_config.wrap = CV::ZERO;
std::shared_ptr<CV::ImageProcess> pretreat =
  std::shared_ptr<CV::ImageProcess>(CV::ImageProcess::create(image_process_config));
pretreat->setMatrix(trans_);

cv::Mat dst_image = cv::Mat(input_h_, input_w_, CV_8UC3);
std::shared_ptr<MNN::Tensor> dst_tensor(MNN::Tensor::create<uint8_t>(std::vector<int>{1, dst_image.rows, dst_image.cols, dst_image.channels()}, dst_image.data));
pretreat->convert((uint8_t *)in.data, in.width, in.height, in.width_step, dst_tensor.get());
cv::imshow("result", dst_image);
cv::waitKey(0);
```

