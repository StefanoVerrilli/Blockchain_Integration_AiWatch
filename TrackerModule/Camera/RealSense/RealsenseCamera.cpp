
#include "RealsenseCamera.hpp"



/**
 * RGB and depth frames can be disaligned. This function saves the required alignment information.
 * @param[in] stream Stream to which the alignment filter shall operate to align the captured streams.
*/
void RealSenseCamera::alignStream(rs2::align stream) {
    rs2::align align_to(stream);
    this->RS2_alignmentFilter = new rs2::align(align_to);
}

rs2::align* RealSenseCamera::getAlignment(void) {
    return this->RS2_alignmentFilter;
}

RealSenseCamera* RealSenseCamera::setPipeline(rs2::pipeline* pipeline) {
    this->RS2_pipeline = pipeline;
    return this;
}

void RealSenseCamera::setDepthIntrinsics(struct rs2_intrinsics depthIntrisics) {
    this->RS2_depthIntrinsicParameters = depthIntrisics;
}

void RealSenseCamera::setColorIntrinsics(struct rs2_intrinsics colorIntrisics) {
    this->RS2_colorIntrisicParameters = colorIntrisics;
}

struct rs2_intrinsics  RealSenseCamera::getColorIntrinsics() {
    return this->RS2_colorIntrisicParameters;
}

void RealSenseCamera::setDepthToColor(struct rs2_extrinsics depthToColorExtrinsics) {
    this->RS2_depthToColorExtrinsics = depthToColorExtrinsics;
}

void RealSenseCamera::setColorToDepth(struct rs2_extrinsics colorToDepthExtrinsics) {
    this->RS2_colorToDepthExtrinsics = colorToDepthExtrinsics;
}

float RealSenseCamera::getScale(void) {
    return this->RS2_scale;
}

/**
 * Captures dummy frames, to allow RealSense autoexposure to settle.
 * @details This function checks if the RelSense camera has been booted for the first time and, if it has, captures `framesNumber` frames to allow autoexposure and more a chance to settle.
 * @param framesNumber Number of dummy frames to capture (recommended: 30).
 * @return True if dummy frames were captured, false otherwise.
 * @see \link{https://dev.intelrealsense.com/docs/rs-save-to-disk}
*/
bool RealSenseCamera::captureDummyFrames(int framesNumber) {
    if(this->RS_firstBoot) {
        for(int i=0; i < framesNumber; i++)
            this->RS2_pipeline->wait_for_frames();
        this->RS_firstBoot = false;
        return true;
    }
    return false;
}

/**
 * Enables the color and depth streams of the RealSense camera.
*/
void RealSenseCamera::configureCamera(void) {
    this->RS2_cameraConfig.enable_stream(rs2_stream::RS2_STREAM_DEPTH, this->resX, this->resY, rs2_format::RS2_FORMAT_Z16, 30);
    this->RS2_cameraConfig.enable_stream(rs2_stream::RS2_STREAM_COLOR, this->resX, this->resY, rs2_format::RS2_FORMAT_RGB8, 30);
}

/**
 * Starts the camera by executing the required functions.
*/
void RealSenseCamera::start(void) {
    this->RS2_pipeline = new rs2::pipeline(context);

    std::cout<<"Setting up RealSense camera"<<std::endl;
    rs2::rates_printer printer;
    
    std::cout<<"Configuring RealSense camera"<<std::endl;

    configureCamera();
    
    std::cout<<"RealSense camera configured successfully" << std::endl;


    std::cout<<"RealSense: Starting pipeline profile"<<std::endl;
    rs2::pipeline_profile pipelineProfile = this->RS2_pipeline->start(this->RS2_cameraConfig);
    
    std::cout<<"RealSense: Aligning senor streams data to color stream"<<std::endl;
    alignStream(rs2_stream::RS2_STREAM_COLOR);

    std::cout<<"RealSense: configuring depth sensor"<<std::endl;
    rs2::depth_sensor sensor = pipelineProfile.get_device().first<rs2::depth_sensor>();
    RS2_scale = sensor.get_depth_scale();


    // Capture 30 frames to give autoexposure, etc. a chance to settle (resulting images would have a blue/green hue otherwise)
    if(captureDummyFrames(30))
        std::cout<<"RealSense: captured dummy frames for autoexposure"<<std::endl;

    setDepthIntrinsics(pipelineProfile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics());
    struct rs2_intrinsics color_intrin = pipelineProfile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>().get_intrinsics();
    setColorIntrinsics(color_intrin);
    setColorToDepth(pipelineProfile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_extrinsics_to(pipelineProfile.get_stream(RS2_STREAM_COLOR)));
    setDepthToColor(pipelineProfile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>().get_extrinsics_to(pipelineProfile.get_stream(RS2_STREAM_DEPTH)));

    std::cout<<"Realsense camera started successfully"<<std::endl;
}

RealSenseCamera::RealSenseCamera(int resX, int resY) : DepthCamera::DepthCamera() {
    this->resX = resX;
    std::cout << resX << std::endl;
    this->resY = resY;
    DepthCamera::setCameraModel("Intel RealSense");
    this->context = rs2::context();
    this->RS_firstBoot = true;
}

void RealSenseCamera::setContext(rs2::context context) {
    this->context = context;
}


/**
 * Captures a RealSense frame and adds it to a queue of captured frames. Useful for generic frame capture. To retrieve the data see getFrameData.
 * @note This function can be used to implement a generic program which can handle multiple depth cameras. Use this function if you want to implement a thread that captures data or if you want to use a generic function for any depth camera. The data can be retrieved using the function getFrameData.
 * @remark Currently this function is not used because each the image generation process has RealSense-specific functions and, for this reason, uses the overloaded functions for performance-oriented reasons.
 * @see getFrameData_func
 * @return Frame identification number of the captured frame.
*/
unsigned int RealSenseCamera::captureFrame(void) try {
    unsigned int frameID = this->getFrameID();
    rs2::frameset frame = this->RS2_pipeline->wait_for_frames();
    frameQueue.enqueue(RSFrame(frameID, frame));
    increaseFrameID();
    return frameID;
} catch (const rs2::error & e){
    std::cerr << "RealSense error occurred during video capture:" << std::endl << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n   " << e.what() << std::endl;
} catch (const std::exception & e) {
    std::cerr << "System error occurred during video capture:" << std::endl << e.what() << std::endl;
}

/**
 * RealSense-specific frame capture which returns raw rs2::frameset data captured from the camera.
 * @note Use this if you are not using threads and need only the RGB and depth frames captured from the RealSense camera.
 * @param[out] RS2_frameset Reference to a rs2::frameset object where the captured data will be saved.
 * @return Frame identification number of the captured frame.
 * @overload
*/
unsigned int RealSenseCamera::captureFrame(rs2::frameset &RS2_frameset) {
    unsigned int frameID = this->getFrameID();
    RS2_frameset = this->RS2_pipeline->wait_for_frames();
    increaseFrameID();
    return frameID;
}

/**
 * RealSense-specific frame capture which returns a RSFrame object containing caaptured data and frame identification.
 * @note Use this if you are not using threads or implementing a RealSense-specific function and need only the RSFrame object containing data.
 * @param[out] RS_frame Reference to a RSFrame object where the captured data and frame identification number.
 * @return Frame identification number of the captured frame
 * @overload
*/
unsigned int RealSenseCamera::captureFrame(RSFrame &RS_frame) {
    RS_frame = RSFrame(this->getFrameID(), this->RS2_pipeline->wait_for_frames());
    increaseFrameID();
    return RS_frame.getFrameIdentifier();
}

/**
 * Returns a frame with its data.
 * @note This function was created because originally this program processed data locally on the Raspberry Pi. Unfortunately, the Raspberry Pi can't handle the processing of much frames per second and so the original program was changed to the current one. Use this function if you want to implement a thread that retrieves data captured froma  different capture thread.
 * @param[out] RS_frame Reference to a frame object where the frame data will be saved.
 * @link getFrameData_func
*/
void RealSenseCamera::getFrameData(RSFrame &RS_frame) {
    RS_frame = frameQueue.dequeue();
}

/**
 * Processes a frame by aligning the RGB frame with the depth frame.
 * @param[out] RS_frame Reference to a RSFrame object where the data will be saved.
*/
void RealSenseCamera::processFrame(RSFrame &RS_frame) {
    RS_frame = this->RS2_alignmentFilter->process(RS_frame.getFrameData()); 
}

void RealSenseCamera::processFrame(rs2::frameset &frameset) {
    frameset = this->RS2_alignmentFilter->process(frameset); 
}