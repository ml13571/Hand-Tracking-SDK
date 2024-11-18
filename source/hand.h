#pragma once
#include "TypeDefines.h"

namespace HandLib {
class Hand
{
private:
    
public:
    Hand();
    ~Hand();
    int LoadHandDetect();
    int LoadHandLandmark();

    int HandDetect(const ImageHead &in, RotateType type, std::vector<ObjectInfo> &objects);
    int HandLandmark(const ImageHead &in, RotateType type, std::vector<ObjectInfo> &objects);
};
}
