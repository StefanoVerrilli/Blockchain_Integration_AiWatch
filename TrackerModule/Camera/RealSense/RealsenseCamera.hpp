#ifndef REALSENSECAMERA_HPP
# define REALSENSECAMERA_HPP

#include "../DepthCamera.hpp"
#include "RSFrame.hpp"
#include "../../Utilities/SyncQueue.hpp"
#include <librealsense2/rs.hpp>
#include <thread>

class RealSenseCamera : public DepthCamera {
    private:
        rs2::align *RS2_alignmentFilter;
        struct rs2_intrinsics RS2_depthIntrinsicParameters;
        struct rs2_intrinsics RS2_colorIntrisicParameters;
        struct rs2_extrinsics RS2_depthToColorExtrinsics;
        struct rs2_extrinsics RS2_colorToDepthExtrinsics;
        rs2::config RS2_cameraConfig;
        rs2::context context;
        rs2::pipeline* RS2_pipeline;
        bool RS_firstBoot;
        float RS2_scale;
        int resX;
        int resY;
        SyncQueue<RSFrame> frameQueue;
    protected:
        bool captureDummyFrames(int framesNumber);
        void configureCamera(void);
        void setContext(rs2::context);
    public:
        void alignStream(rs2::align stream);
        void start(void) override;
        unsigned int captureFrame() override;
        unsigned int captureFrame(rs2::frameset &RS2_frameset);
        unsigned int captureFrame(RSFrame &frame);
        void getFrameData(RSFrame& RS_frame);
        void processFrame(RSFrame &RS_frame);
        void processFrame(rs2::frameset &frameset);
        RealSenseCamera* setPipeline(rs2::pipeline* pipeline);
        void setColorIntrinsics(struct rs2_intrinsics colorIntrisics);
        struct rs2_intrinsics getColorIntrinsics();
        void setDepthIntrinsics(struct rs2_intrinsics depthIntrisics);
        void setDepthToColor(struct rs2_extrinsics depthToColorExtrinsics);
        void setColorToDepth(struct rs2_extrinsics colorToDepthExtrinsics);
        float getScale(void);
        rs2::align* getAlignment(void);
        RealSenseCamera(int resX=848, int resY=480);
};

#endif