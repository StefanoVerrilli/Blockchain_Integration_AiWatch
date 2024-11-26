
#include "OpenCVImgGen.hpp"
#include "../Room/CoordinateMapping/CoordinateMappingManager.hpp"
#include "../Output/Skeleton.hpp"
#include "../Camera/RealSense/RealsenseCamera.hpp"

#define RSIMAGESFOLDER "rs-images"

namespace RealSenseOpenCV {
    void framesetToMat (const rs2::frame & singleFrame, cv::Mat &outputMat) {
        rs2::video_frame videoFrame = singleFrame.as<rs2::video_frame>();
        const int frameWidth = videoFrame.get_width(), frameHeight = videoFrame.get_height();
    
        switch (singleFrame.get_profile().format()) {
            case RS2_FORMAT_BGR8:
                outputMat = cv::Mat(cv::Size(frameWidth, frameHeight), CV_8UC3, (void *) singleFrame.get_data(), cv::Mat::AUTO_STEP);
                break;
            case RS2_FORMAT_RGB8: {
                cv::Mat rgbFrame = cv::Mat(cv::Size(frameWidth, frameHeight), CV_8UC3, (void *) singleFrame.get_data(), cv::Mat::AUTO_STEP);
                cv::cvtColor(rgbFrame, rgbFrame, cv::COLOR_RGB2BGR);
                outputMat = rgbFrame;
                break;
            }
            case RS2_FORMAT_Z16:
                outputMat = cv::Mat(cv::Size(frameWidth, frameHeight), CV_16UC1, (void *) singleFrame.get_data(), cv::Mat::AUTO_STEP);
                break;
            case RS2_FORMAT_Y8:
                outputMat = cv::Mat(cv::Size(frameWidth, frameHeight), CV_8UC1, (void *) singleFrame.get_data(), cv::Mat::AUTO_STEP);
                break;
            default:
                throw std::runtime_error("Frame format is not supported yet!");
                break;
        }
    }

    void saveFrameImages (unsigned int frameID, rs2::frameset& frameset, float scale, std::string imagesFolder = RSIMAGESFOLDER) {
        rs2::colorizer colorMap;
        rs2::depth_frame depthFrame = frameset.get_depth_frame();
        rs2::spatial_filter spatialFilter;
        spatialFilter.set_option(RS2_OPTION_HOLES_FILL, 1);
        depthFrame = spatialFilter.process(depthFrame);
        rs2::frame colorFrame = frameset.get_color_frame(), colorizedDepthFrame = depthFrame.apply_filter(colorMap);;
        int cols = depthFrame.get_width(), rows = depthFrame.get_height();
        // Convert
        cv::Mat colorImage, distanceImage = cv::Mat::zeros(rows, cols, CV_32FC1), depthImage, colorizedDepthImage;
        framesetToMat(colorFrame, colorImage);
        framesetToMat(depthFrame, depthImage);
        framesetToMat(colorizedDepthFrame, colorizedDepthImage);
        std::cout << "Scale: " << scale << std::endl;
        // Scale
        depthImage *= 1000.0 * scale;
        // Compute distance
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                distanceImage.at <float> (j, i) = (float) depthFrame.get_distance(i, j);
                //std::cout << depthFrame.get_distance(i,j) << " ";
            }
            //std::cout << std::endl;
        }
        
        std::stringstream colorImagePath, distanceImagePath, colorizedDepthImagePath;
        colorImagePath <<           imagesFolder << "/rgb/" <<   frameID << "_Color.png";
        distanceImagePath <<        imagesFolder << "/d/" <<     frameID << "_Distance.exr";
        colorizedDepthImagePath <<  imagesFolder << "/depth/" << frameID << "_Depth.png";
        // Save
        OpenCVImgGen::saveImages( { colorImage, distanceImage, colorizedDepthImage },
                                 { colorImagePath.str(), distanceImagePath.str(), colorizedDepthImagePath.str() } );
        OpenCVImgGen::releaseImages( { colorImage, depthImage, distanceImage, colorizedDepthImage } );
    }
}

namespace RealSenseOpenCVToSkeleton {
    void createSkeletons (RealSenseCamera* camera, CoordinateMappingManager* mapping, unsigned int imageID, const float skeletonThreshold, std::string imagesFolder = RSIMAGESFOLDER, std::string outputFolder = "op-output", std::string thingID = "thing") {

        std::stringstream inputJsonFilePath, skeletonImagePath, colorImagePath, distanceImagePath, colorizedDepthImagePath, skeletonOnlyImagePath, outputJsonFilePath;
        inputJsonFilePath << outputFolder << "/op/" << imageID << "_Color_keypoints.json";
        Json::Value currentJSON;

        // Try to load OpenPose's output JSON file
        if (JSONOutput::loadJSON(inputJsonFilePath.str(), currentJSON)) {
            Json::Value people = JSONOutput::getValueAt("people", currentJSON);
            colorImagePath << imagesFolder << "/rgb/" << imageID << "_Color.png";
            distanceImagePath << imagesFolder << "/d/" << imageID << "_Distance.exr";
            colorizedDepthImagePath << imagesFolder << "/depth/" << imageID << "_Depth.png";

            // Load images captured previously from camera
            cv::Mat colorImage, colorizedDepthImage;
            OpenCVImgGen::loadImage(colorImagePath.str(), cv::IMREAD_COLOR, colorImage);
            cv::Mat distanceImage = cv::Mat(colorImage.rows, colorImage.cols, CV_32FC1);
            OpenCVImgGen::loadImage(distanceImagePath.str(), cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH, distanceImage);
            OpenCVImgGen::loadImage(colorizedDepthImagePath.str(), cv::IMREAD_COLOR, colorizedDepthImage);
            // Note: Comment following line for better performance 
            //OpenCVImgGen::showImages( { colorImage, colorizedDepthImage }, { "Frame No Skeleton", "Frame Colorized Depth" } );
            cv::Mat skeletonOnlyImage = cv::Mat::zeros(colorImage.rows, colorImage.cols, colorImage.type());

            // Produce a JSON output file for people present in this frame
            JSONOutput::createJSON(people, imageID, colorImage, distanceImage, camera, mapping, thingID, skeletonThreshold, outputFolder);

            // Show images and save them. Note: Comment following line for better performance
            //OpenCVImgGen::showImages( { skeletonOnlyImage, colorImage }, { "Frame Skeleton Background Cut", "Frame Skeleton" } );
            skeletonOnlyImagePath << imagesFolder << "/sk/" << imageID << "_sk.png";
            skeletonImagePath << imagesFolder << "/skeleton/" << imageID << "_Skeleton.png";
            OpenCVImgGen::saveImages( { colorImage, skeletonOnlyImage }, { skeletonImagePath.str(), skeletonOnlyImagePath.str() } );
            OpenCVImgGen::releaseImages( { colorImage, distanceImage, colorizedDepthImage, skeletonOnlyImage } );
        }
    }
}