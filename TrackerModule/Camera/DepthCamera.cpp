#include "DepthCamera.hpp"

void DepthCamera::setFrameID(unsigned int value) {
    this->frameID = value;
}

unsigned int DepthCamera::getFrameID(void) {
    return this->frameID;
}

void DepthCamera::increaseFrameID(void) {
    this->frameID += 1;
}

void DepthCamera::setCameraModel(std::string model) {
    this->cameraModel = model;
}

std::string DepthCamera::getCameraModel(void) {
    return this->cameraModel;
}

DepthCamera::DepthCamera(void) {
    DepthCamera::setFrameID(0);
}