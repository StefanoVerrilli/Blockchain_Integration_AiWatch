// DepthCamera.cpp
// Author: Francesco Pollasto

#ifndef DEPTHCAMERA_H
#define DEPTHCAMERA_H

#include <iostream>

class DepthCamera {

    private:
        unsigned int frameID;
        std::string cameraModel;
        unsigned int captureFramesSkip;

    protected:
        void setFrameID(unsigned int value);
        void setCameraModel(std::string model);

    public:
        unsigned int getFrameID(void);
        void increaseFrameID(void);
        std::string getCameraModel(void);
        virtual void start(void) {
            std::cout << "Simulated camera start" << std::endl;
        }
        virtual unsigned int captureFrame() {
            std::cout << "Simulated frame capture" << std::endl;
            return 0;
        }
        DepthCamera(void);
};

#endif