//
//  Skeleton.cpp
//  AI Watch A1
//
//  Created by Denny Caruso on 17/07/22.
//

// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2022. All Rights Reserved.

#include "Skeleton.hpp"


void Skeleton::setRGB_Image (cv::Mat & myImage) {
    this->rgb_Image = myImage;
}

void Skeleton::setDistance_Image (cv::Mat & myImage) {
    this->distance_Image = myImage;
}

void Skeleton::setSkeleton_Image (cv::Mat & myImage) {
    this->skeleton_Image = myImage;
}

void Skeleton::setBodyKeyPoints (std::vector <BodyKeyPoint> & bodyKeyPoints) {
    this->bodyKeyPoints = bodyKeyPoints;
}

void Skeleton::setBodyKeyPointsMap (std::vector <bool> & bodyKeyPointsMap) {
    this->bodyKeyPointsMap = bodyKeyPointsMap;
}

void Skeleton::setSkeletonData (Json::Value skeletonData) {
    this->skeletonData = skeletonData;
}

void Skeleton::setSkeletonPoints3D_RS (std::vector <Point3D *> skeletonPoints3D_RS) {
    this->skeletonPoints3D_RS = skeletonPoints3D_RS;
}

void Skeleton::setSkeletonPoints3D (std::vector <Point3D *> * skeletonPoints3D) {
    this->skeletonPoints3D = skeletonPoints3D;
}

void Skeleton::setConsistency (float consistency) {
    this->consistency = consistency;
}



cv::Mat Skeleton::getRGB_Image(void) {
    return this->rgb_Image;
}

cv::Mat Skeleton::getDistance_Image (void) {
    return this->distance_Image;
}

cv::Mat Skeleton::getSkeleton_Image (void) {
    return this->skeleton_Image;
}

std::vector <BodyKeyPoint> Skeleton::getBodyKeyPoints (void) {
    return this->bodyKeyPoints;
}

std::vector <bool> Skeleton::getBodyKeyPointsMap (void) {
    return this->bodyKeyPointsMap;
}

Json::Value Skeleton::getSkeletonData (void) {
    return this->skeletonData;
}

// Don't change following method, otherwise error
void Skeleton::calcBodyKeypoints (void) {
    int j = 0;
    for (Json::Value::ArrayIndex i = 0; i < skeletonData.size(); i++) {
        bodyKeyPoints.push_back(BodyKeyPoint(
                                 JSONOutput::getValueAt((unsigned int) i, getSkeletonData()).asInt(),
                                 JSONOutput::getValueAt((unsigned int) i + 1, getSkeletonData()).asInt(),
                                 JSONOutput::getValueAt((unsigned int) i + 2, getSkeletonData()).asFloat())
                                );
        
        bodyKeyPointsMap.push_back(bodyKeyPoints.at(j).getX() > 0 && bodyKeyPoints.at(j).getY() > 0 && bodyKeyPoints.at(j).getConfidence() > 0.00);
        setConsistency(getConsistency() + bodyKeyPoints.at(j).getConfidence());
        i += 2;
        j += 1;
    }
    
    setConsistency(getConsistency() / OPENPOSE_BODY_KEYPOINTS);
}

void Skeleton::calcBodyEdges (void) {
    for (unsigned char i = 0; i < getBodyKeyPoints().size(); i++) {
        if (i >= OPENPOSE_BODY_KEYPOINTS || (!getBodyKeyPointsMap().at(i))) continue;
        drawCircle(cv::Point(getBodyKeyPoints().at(i).getX(), getBodyKeyPoints().at(i).getY()));
        
        switch (i) {
            case Nose ... RElbow:
            case LShoulder ... LElbow:
            case MidHip ... RKnee:
            case LHip ... LKnee:
            case LBigToe:
            case RBigToe:
                drawLine(i, i + 1);
                break;
            default:
                break;
        }
    }
    
    drawLine(Neck, LShoulder);
    drawLine(Neck, MidHip);
    drawLine(MidHip, LHip);
    drawLine(REye, Nose);
    drawLine(REye, REar);
    drawLine(LEye, Nose);
    drawLine(LEye, LEar);
    drawLine(LAnkle, LBigToe);
    drawLine(LAnkle, LHeel);
    drawLine(RAnkle, RBigToe);
    drawLine(RAnkle, RHeel);
}

void Skeleton::drawLine (unsigned char start, unsigned char end) {
    if (getBodyKeyPointsMap().at(start) && getBodyKeyPointsMap().at(end)) {
        cv::line(getRGB_Image(), cv::Point(getBodyKeyPoints().at(start).getX(), getBodyKeyPoints().at(start).getY()),
                 cv::Point(getBodyKeyPoints().at(end).getX(), getBodyKeyPoints().at(end).getY()), cv::Scalar(0, 255, 0), 3, cv::LINE_8, 0);
        cv::line(getSkeleton_Image(), cv::Point(getBodyKeyPoints().at(start).getX(), getBodyKeyPoints().at(start).getY()),
                 cv::Point(getBodyKeyPoints().at(end).getX(), getBodyKeyPoints().at(end).getY()), cv::Scalar(0, 255, 0), 3, cv::LINE_8, 0); // remove in future
    }
}

void Skeleton::drawCircle (cv::Point center) {
    cv::circle(getRGB_Image(), center, 4, cv::Scalar(0, 0, 255), 8, cv::LINE_8, 0);
    cv::circle(getSkeleton_Image(), center, 4, cv::Scalar(0, 0, 255), 8, cv::LINE_8, 0); // remove
}

void Skeleton::deprojectSkeletonPoints3D (RealSenseCamera* camera) {
    struct rs2_intrinsics color_intrin = camera->getColorIntrinsics();
    for (unsigned char i = 0; i < getBodyKeyPoints().size(); i++) {
        float * pixel = new (std::nothrow) float [2], * point = new (std::nothrow) float [3], distance = 0;
        if (!pixel || !point) exit(-1);
        
        if (!getBodyKeyPointsMap().at(i)) {
            point[0] = point[1] = point[2] = pixel[0] = pixel[1] = 0;
        } else {
            pixel[0] = getBodyKeyPoints().at(i).getX();
            pixel[1] = getBodyKeyPoints().at(i).getY();
            distance = getDistance_Image().at<float>(pixel[1], pixel[0]);
            rs2_deproject_pixel_to_point(point, & color_intrin, pixel, distance);
        }

        Point3D * point3D = new Point3D(point[0], point[1], point[2], new BodyKeyPoint(0, 0, getBodyKeyPoints().at(i).getConfidence()));
        skeletonPoints3D_RS.push_back(point3D);
        delete [] pixel;
        delete [] point;
    }
}

void Skeleton::writeCoordinates (void) {
    for (unsigned char i = 0; i < getSkeletonPoints3D()->size(); i++) {
        std::stringstream labelTextX, labelTextY, labelTextZ, labelTextConfidence;
        labelTextX << getSkeletonPoints3D()->at(i)->getX();
        labelTextY << getSkeletonPoints3D()->at(i)->getY();
        labelTextZ << getSkeletonPoints3D()->at(i)->getZ();
        labelTextConfidence << ((BodyKeyPoint *) getSkeletonPoints3D()->at(i)->getDecorated())->getConfidence();
        
        if (i == 0 || i == 12 || i == 14) {
            cv::putText(getRGB_Image(), labelTextX.str(), cv::Point(getBodyKeyPoints().at(i).getX() + 10, getBodyKeyPoints().at(i).getY() + 10), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0), 1, cv::LINE_8);
            cv::putText(getRGB_Image(), labelTextY.str(), cv::Point(getBodyKeyPoints().at(i).getX() + 10, getBodyKeyPoints().at(i).getY() + 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0), 1, cv::LINE_8);
            cv::putText(getRGB_Image(), labelTextZ.str(), cv::Point(getBodyKeyPoints().at(i).getX() + 10, getBodyKeyPoints().at(i).getY() + 40), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0), 1, cv::LINE_8);
        }
    }
}



Skeleton::Skeleton (cv::Mat & rgbImage, cv::Mat & dImage, Json::Value skeletonData) {
    setRGB_Image(rgbImage);
    setDistance_Image(dImage);
    cv::Mat image;
    setSkeleton_Image(image);
    setSkeletonData(skeletonData);
    setConsistency(0.0);
}

Skeleton::~Skeleton (void) {
    for (auto & point: * getSkeletonPoints3D()) delete point;
    for (auto & point: getSkeletonPoints3D_RS()) delete point;
    getSkeletonPoints3D()->clear();
    if (getSkeletonPoints3D() != nullptr) delete getSkeletonPoints3D();
    getSkeletonPoints3D_RS().clear();
}

void Skeleton::generateSkeleton (RealSenseCamera* camera, CoordinateMappingManager* mapping) {
    calcBodyKeypoints();
    calcBodyEdges();
    deprojectSkeletonPoints3D(camera);
    // xOrigin and zOrigin parameters are inverted as the built Unity world requires.
    setSkeletonPoints3D(mapping->mapToMeters(getSkeletonPoints3D_RS(), getBodyKeyPointsMap(), zOriginUnity, xOriginUnity));
    writeCoordinates();
}

std::vector <Point3D *> Skeleton::getSkeletonPoints3D_RS (void) {
    return this->skeletonPoints3D_RS;
}

std::vector <Point3D *> * Skeleton::getSkeletonPoints3D (void) {
    return this->skeletonPoints3D;
}

float Skeleton::getConsistency (void) {
    return this->consistency;
}



Json::Value JSONOutput::makeOutputString (std::vector <Point3D *> skeletonPoints3D, std::vector <bool> bodyKeyPointsMap, unsigned int frameID, unsigned int personID) {
    Json::Value root, arraySkeletonPoints3D(Json::arrayValue);
    Json::StyledStreamWriter writer;
    // For each OpenPose's skeleton body joint point
    for (unsigned char i = 0; i < skeletonPoints3D.size(); i++) {
        if (i >= OPENPOSE_BODY_KEYPOINTS) continue;
        Json::Value singlePoint3D_JSON;
        // Build JSON node for i-th body joint point
        singlePoint3D_JSON["pointID"] = Json::Value((unsigned int) i);
        singlePoint3D_JSON["confidence"] = Json::Value(((BodyKeyPoint *) skeletonPoints3D.at(i)->getDecorated())->getConfidence());
        singlePoint3D_JSON["x"] = Json::Value((double) skeletonPoints3D.at(i)->getZ());
        singlePoint3D_JSON["y"] = Json::Value((double) skeletonPoints3D.at(i)->getY());
        singlePoint3D_JSON["z"] = Json::Value((double) skeletonPoints3D.at(i)->getX());
        singlePoint3D_JSON["x_rotation"] = Json::Value(0.0);
        singlePoint3D_JSON["y_rotation"] = Json::Value(0.0);
        singlePoint3D_JSON["z_rotation"] = Json::Value(0.0);
        singlePoint3D_JSON["w_rotation"] = Json::Value(1.0);
        arraySkeletonPoints3D.append(singlePoint3D_JSON);
    }
    root["personID"] = Json::Value((unsigned int) personID);
    root["skeleton"] = arraySkeletonPoints3D;
    //output = root.toStyledString();
    return root;
}
bool JSONOutput::loadJSON (std::string filePathJSON, Json::Value & currentJSON) {
    Json::Reader readerJSON;
    std::ifstream streamJSON(filePathJSON.c_str(), std::ifstream::binary);
    return readerJSON.parse(streamJSON, currentJSON, false);
}
void JSONOutput::saveJSON (std::string JSONdata, std::string filePath) {
    Json::StyledStreamWriter writer;
    std::ofstream outputFile(filePath);
    outputFile << JSONdata;
    outputFile.close();
}
Json::Value JSONOutput::getValueAt (std::string key, Json::Value currentJSON) {
    return currentJSON[key];
}
Json::Value JSONOutput::getValueAt (unsigned int i, Json::Value currentJSON) {
    return currentJSON[i];
}
Json::Value JSONOutput::getValueAt (std::string key, unsigned int i, Json::Value currentJSON) {
    return (currentJSON[i])[key];
}
void JSONOutput::createJSON (Json::Value & people, unsigned int nFrame, cv::Mat & colorImage, cv::Mat & distanceImage, RealSenseCamera* camera, CoordinateMappingManager* mapping, std::string thingID, const float skeletonThreshold, std::string outputFolder) {
    Json::Value root, peopleArray(Json::arrayValue);
    std::stringstream outputJsonFilePath;
    root["ID_Frame"] = nFrame;
    root["thingId"] = Json::Value(std::string("digitaltwin:"+thingID+":1"));
    // For each OpenPose's detected skeleton 
    for (Json::Value::ArrayIndex i = 0; i < people.size(); i++) {
        Json::Value singlePerson = getValueAt("pose_keypoints_2d", i, people);
        Skeleton singlePersonSkeleton = Skeleton(colorImage, distanceImage, singlePerson);
        singlePersonSkeleton.generateSkeleton(camera, mapping);
        // Discard Skeleton with low consistency value
        if (singlePersonSkeleton.getConsistency() > skeletonThreshold) {
            peopleArray.append(makeOutputString(* singlePersonSkeleton.getSkeletonPoints3D(), singlePersonSkeleton.getBodyKeyPointsMap(), nFrame, (unsigned int) i));
        }
    }
    root["People"] = peopleArray;
    outputJsonFilePath << outputFolder << "/movement/frame" << nFrame << "_" << JSON_FILE_PATH;
    saveJSON(root.toStyledString(), std::string(outputJsonFilePath.str()));
    outputJsonFilePath.str(std::string());
    outputJsonFilePath.clear();
}