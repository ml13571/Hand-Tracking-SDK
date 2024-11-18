/*
 * @Author: chenjingyu
 * @Date: 2023-08-04 20:29:55
 * @LastEditTime: 2023-08-16 10:14:07
 * @Description: Image Embedding module
 * @FilePath: \Mediapipe-MNN\source\ImageEmbedding.h
 */
#pragma once

#include "TypeDefines.h"

#include <MNN/ImageProcess.hpp>
#include <MNN/Interpreter.hpp>
#include <MNN/Matrix.h>
#include <MNN/Tensor.hpp>
#include <memory>
#include <vector>

namespace mirror {
class ImageEmbedding {
public:
  ImageEmbedding() = default;
  ~ImageEmbedding();

  bool LoadModel(const char *model_file);
  void setFormat(int format);
  bool Detect(const ImageHead &in, RotateType type, Embedding &embedding);


private:
  bool inited_ = false;
  int input_w_ = 0;
  int input_h_ = 0;
  std::shared_ptr<MNN::CV::ImageProcess> pretreat_ = nullptr;
  std::unique_ptr<MNN::Interpreter> net_ = nullptr;
  MNN::Session *sess_ = nullptr;
  MNN::Tensor *input_tensor_ = nullptr;
  MNN::CV::Matrix trans_;

  const float meanVals_[3] = {127.5f, 127.5f, 127.5f};
  const float normVals_[3] = {1.0f / 127.5f, 1.0f / 127.5f, 1.0f / 127.5f};
};
} // namespace mirror

