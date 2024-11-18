#include "hand.h"
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace HandLib;
int camTest(){
 
  Hand m_hand;
  if (m_hand.LoadHandDetect() != 0 ||
      m_hand.LoadHandLandmark() != 0) {
    std::cout << "Failed load model." << std::endl;
    return -1;
  }

	cv::VideoCapture cap(0);

	if (!cap.isOpened()) {
		std::cout << "Camera did not turn on successfully" << std::endl;
	}
	
	while (1)
	{
		cv::Mat frame;

		bool res = cap.read(frame);
		if (!res){ break;}

		if (frame.empty()){
			break;
		}
    cv::Mat image = frame;
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

    std::vector<ObjectInfo> objects;
    m_hand.HandDetect(in, type, objects);
    m_hand.HandLandmark(in, type, objects);

    for (const auto &object : objects) {
      
      cv::Scalar color1(0, 255, 0);
      
      for (int i = 0; i < object.landmarks.size(); ++i) {
        cv::Point pt = cv::Point((int)object.landmarks[i].x, (int)object.landmarks[i].y);
        cv::circle(image, pt, 5, cv::Scalar(0, 0, 255));

        cv::Point pt1 = cv::Point((int)object.landmarks[i].x, (int)object.landmarks[i].y);
        cv::Point pt2 = cv::Point((int)object.landmarks[i+1].x, (int)object.landmarks[i+1].y);

        if (i < 4) {   
            cv::line(image, pt1, pt2, color1, 2, 8);
        }
        if (i < 8 && i > 4) {
            cv::line(image, pt1, pt2, color1, 2, 8);
        }
        if (i < 12 && i > 8) {
            cv::line(image, pt1, pt2, color1, 2, 8);
        }
        if (i < 16 && i > 12) {
            cv::line(image, pt1, pt2, color1, 2, 8);
        }
        if (i < 20 && i > 16) {
            cv::line(image, pt1, pt2, color1, 2, 8);
        }

        pt1 = cv::Point((int)object.landmarks[0].x, (int)object.landmarks[0].y);
        cv::line(image, pt1, cv::Point((int)object.landmarks[5].x, (int)object.landmarks[5].y), color1, 2, 8);
        cv::line(image, pt1, cv::Point((int)object.landmarks[9].x, (int)object.landmarks[9].y), color1, 2, 8);
        cv::line(image, pt1, cv::Point((int)object.landmarks[13].x, (int)object.landmarks[13].y), color1, 2, 8);
        cv::line(image, pt1, cv::Point((int)object.landmarks[17].x, (int)object.landmarks[17].y), color1, 2, 8);

        // printf("\n 3D coordinate--> x: %f, y: %f, z: %f", object.landmarks3d[0].x, object.landmarks3d[0].y, object.landmarks3d[0].z);
      }
    }

		cv::imshow("live video", image);
		//wait 1ms, exit loop if key is pressed
		if (cv::waitKey(1) >= 0){
			break;
		}
	}
	cap.release();
  return 0;
}

int main(int argc, char *argv[]) {
 
  camTest();

  return 0;
}
