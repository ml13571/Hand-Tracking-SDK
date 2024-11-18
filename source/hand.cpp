#include "hand.h"
#include "PalmDetector.h"
#include "PalmLandmarkDetector.h"

using namespace HandLib;

PalmDetector detector;
PalmLandmarkDetector landmarker;

Hand::Hand(){ }

Hand::~Hand(){ }

int Hand::LoadHandDetect(){
    if(!detector.LoadModel()){ return -1; }
    return 0;
}

int Hand::LoadHandLandmark(){
    if(!landmarker.LoadModel()) { return -1;}
    return 0;
}

int Hand::HandDetect(const ImageHead &in, RotateType type, std::vector<ObjectInfo> &objects){
    detector.setFormat(in.pixel_format);
    detector.Detect(in, type, objects);
    return 0;
}
int Hand::HandLandmark(const ImageHead &in, RotateType type, std::vector<ObjectInfo> &objects){
    landmarker.setFormat(in.pixel_format);
    landmarker.Detect(in, type, objects);
    return 0
}


