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

    bool HandDetect(const ImageHead &in, RotateType type, std::vector<ObjectInfo> &objects);
    bool HandLandmark(const ImageHead &in, RotateType type, std::vector<ObjectInfo> &objects);
};
}
