#ifndef RSFRAME_HPP
# define RSFRAME_HPP

#include <librealsense2/rs.hpp>

class RSFrame {
    private:
        unsigned int FRAME_IDENTIFIER;
        rs2::frameset RS2_frameData;
    public:
        rs2::depth_frame getDepthFrame(void) {return RS2_frameData.get_depth_frame();}
        rs2::frame getColorFrame(void) {return RS2_frameData.get_color_frame();}
        rs2::frameset getFrameData(void) {return RS2_frameData;}
        unsigned int getFrameIdentifier(void) {return FRAME_IDENTIFIER;}
        void setFrameData(rs2::frameset frame) {(this->RS2_frameData) = frame;}
        void setFrameIdentifier(unsigned int id) {this->FRAME_IDENTIFIER = id;}
        RSFrame operator=(rs2::frameset frame) {this->RS2_frameData = frame; return *this;};
        RSFrame(unsigned int frameID, rs2::frameset frameData) {FRAME_IDENTIFIER = frameID; this->RS2_frameData = frameData;};
        RSFrame(unsigned int frameID) {FRAME_IDENTIFIER = frameID; RSFrame();};
        RSFrame() {};
};

#endif