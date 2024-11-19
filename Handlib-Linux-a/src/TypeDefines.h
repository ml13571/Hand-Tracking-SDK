#pragma once

#include <string>
#include <vector>
namespace HandLib {
using byte = unsigned char;
struct Point2f {
  float x;
  float y;
};

struct Point3f {
  float x;
  float y;
  float z;
  float visibility;  // landmark score
  float presence;     // landmark in frame score
};

using LandmarkList = std::vector<Point2f>;
using NormalizedLandmarkList = std::vector<Point2f>;

struct Rect {
  float left;
  float top;
  float right;
  float bottom;
};

struct ObjectInfo {
  Rect rect;
  std::vector<Point2f> landmarks;
  std::vector<Point3f> landmarks3d;
  float score;
  float angle;
  int left_right;
};

enum PixelFormat {
  RGBA = 0,
  RGB = 1,
  BGR = 2,
  GRAY = 3,
  BGRA = 4,
  YCrCb = 5,
  YUV = 6,
  HSV = 7,
  XYZ = 8,
  BGR555 = 9,
  BGR565 = 10,
  YUV_NV21 = 11,
  YUV_NV12 = 12,
  YUV_I420 = 13,
  HSV_FULL = 14,
};

enum RotateType {
  CLOCKWISE_ROTATE_0 = 0, ///< The image does not need to be rotated, and the face in the image is a frontal face
  CLOCKWISE_ROTATE_90 = 1, ///< The image needs to be rotated 90 degrees clockwise to make the face frontal.
  CLOCKWISE_ROTATE_180 = 2, ///< The image needs to be rotated 180 degrees clockwise to make the face frontal.
  CLOCKWISE_ROTATE_270 = 3 ///< The image needs to be rotated 270 degrees clockwise to make the face frontal.
};

typedef struct ImageHead_t {
  byte *data;               
  PixelFormat pixel_format; 
  int width;                
  int height;               
  int width_step;           
  double time_stamp;        
} ImageHead;

struct Embedding {
  std::vector<float> float_embedding;
  std::string quantized_embedding;
};

struct ClassifierInfo {
  int id;
  float score;
};

} // namespace HandLib
