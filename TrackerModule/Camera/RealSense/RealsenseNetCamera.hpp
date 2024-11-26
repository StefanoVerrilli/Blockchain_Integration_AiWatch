#include "RealsenseCamera.hpp"
#include <librealsense2-net/rs_net.hpp>
#include <boost/asio.hpp>

#ifndef REALSENSENETCAMERA_H
#define REALSENSENETCAMERA_H

#define LISTEN_PORT 9943

class RealSenseNetCamera : public RealSenseCamera {
    private:
        std::string cameraIP;
    public:
        RealSenseNetCamera(std::string IP);
        static std::string getIPAutomatically();
        std::string getIP(void);
};

#endif