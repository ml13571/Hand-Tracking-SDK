# Hand Tracking Linux SDK
 This repo is for building hand tracking static library on Linux

## Install OpenCV
To build library, OpenCV should be installed.</br>
### 1. Update Package
```bash
sudo apt-get update
sudo apt-get upgrade
```
### 2. Build and Install OpenCV
```bash
sudo apt-get install python3-pip -y
sudo pip3 install numpy
sudo apt-get install cmake build-essential pkg-config ant -y
sudo apt-get install libtiff5-dev libopenexr-dev libwebp-dev libpng-dev -y
sudo apt-get install libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev libavresample-dev libv4l-dev -y
sudo apt-get install libgtk-3-dev -y
sudo apt-get install libtesseract-dev -y
sudo apt-get install libhdf5-dev -y
sudo apt-get install libopenblas-dev libopenblas-base liblapacke-dev -y
sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev -y
sudo apt-get install libtbb-dev -y
```
Due to a bug after installing the OpenBLAS lib (https://github.com/opencv/opencv/issues/12957), edit the opencv-4.5.0/cmake/OpenCVFindOpenBLAS.cmake file:

* Append the path `/usr/include/x86_64-linux-gnu` to `SET(Open_BLAS_INCLUDE_SEARCH_PATHS`
* Append the path `/usr/lib/x86_64-linux-gnu` to `SET(Open_BLAS_LIB_SEARCH_PATHS`

Download and untar the [opencv-4.5.0](https://github.com/opencv/opencv/archive/4.5.0.tar.gz) and [opencv_contrib-4.5.0](https://github.com/opencv/opencv_contrib/archive/4.5.0.zip) source files.

Considering that we have the files unzipped under the /tmp folder.
```bash
cd /tmp/opencv-4.5.0
mkdir build
cd build
```

Run

```bash
cmake -D CMAKE_BUILD_TYPE=RELEASE \	
-D CMAKE_INSTALL_PREFIX=/usr/local \	
-D WITH_TBB=OFF \	
-D WITH_IPP=OFF \	
-D WITH_1394=OFF \	
-D BUILD_WITH_DEBUG_INFO=OFF \	
-D BUILD_DOCS=OFF \	
-D INSTALL_C_EXAMPLES=ON \	
-D INSTALL_PYTHON_EXAMPLES=ON \	
-D BUILD_EXAMPLES=OFF \	
-D BUILD_TESTS=OFF \	
-D BUILD_PERF_TESTS=OFF \	
-D WITH_QT=OFF \	
-D WITH_GTK=ON \	
-D WITH_OPENGL=ON \	
-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.5.0/modules \	
-D WITH_V4L=ON  \	
-D WITH_FFMPEG=ON \	
-D WITH_XINE=ON \	
-D BUILD_NEW_PYTHON_SUPPORT=ON \	
-D OPENCV_GENERATE_PKGCONFIG=ON ../	
```
Finally, build and install OpenCV as follows. (It will take some 30~40 minutes to build.)
```bash
make
sudo make install
sudo ldconfig
```
## 1. How To Build
 - Create new folder "build" as below
 ```bash
 mkdir build
 ```
 - Build project
 ```bash
 cmake .. && make
 ```
 - Run app (webcam should be connected to run app)
 ```bash
 ./test
 ```

 ## 2. How To Build Static Library Which Doesn't Need MNN & OpenCV Dependencies
 Copy all files in folder "source" into the folder in `3rdLibs/MNN-2.3.0/source`.</br>
 Update CMakeLists.txt in MNN-2.3.0 accordingly and build MNN-2.3.0 source code. Once building is completed, `libHand.a` file would be generated.</br>
 The pre-built shared library(`libHand.a`) and demo project to run it has been added to this project.(`Handlib-Linux-a/lib/libHand.a`)</br>
 - Go to the following path: `Handlib-Linux-a`
 - Create new folder "build"
 ```bash
 mkdir build
 ```
 - build and run app
 ```bash
 cmake .. && make
 ```
