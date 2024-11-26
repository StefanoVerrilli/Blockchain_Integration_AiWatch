#include "Camera/RealSense/RealsenseNetCamera.hpp"
#include "ImageGen/RealSenseOpenCV.hpp"
#include "OpenPose/OpenPoseCommandLine.hpp"
#include "SystemInterface/CLInput.cpp"
#include "Output/Skeleton.hpp"
#include "Blockchain/BlockchainLoader.hpp"
#include "Kafka/KafkaJSONSend.hpp"

#include <map>

std::map<int, RealSenseCamera*> connectedCameras;

#define OPENPOSE_SKELETON_THRESHOLDING 0.05

int main() {
    // Get configuration from file
    CLInput::build("conf.conf");

        Room room = CVPR_Lab();
    UnityCoordinateMappingManager* mapping = new UnityCoordinateMappingManager(room);

    SystemExec::executeCommand("mkdir " + CLInput::get()->getParam("imgSaveFolder"));
    SystemExec::executeCommand("mkdir " + CLInput::get()->getParam("openposeOutputFolder") );

    SystemExec::executeCommand("mkdir " + CLInput::get()->getParam("imgSaveFolder") +  "/d");
    SystemExec::executeCommand("mkdir " + CLInput::get()->getParam("imgSaveFolder") +  "/rgb");
    SystemExec::executeCommand("mkdir " + CLInput::get()->getParam("imgSaveFolder") +  "/sk");
    SystemExec::executeCommand("mkdir " + CLInput::get()->getParam("imgSaveFolder") +  "/skeleton");
    
   
    //RealSenseNetCamera* camera = new RealSenseNetCamera(RealSenseNetCamera::getIPAutomatically());
    RealSenseCamera* camera = new RealSenseCamera();
    camera->start();

    KafkaManager* kafka = new KafkaManager("topic1");
    BlockchainLoader* blockchain = new BlockchainLoader();
    rs2::frameset frameset;
    int frameID;

    for(;;) {
        frameID = camera->captureFrame(frameset);
        camera->processFrame(frameset);
        RealSenseOpenCV::saveFrameImages(frameID, frameset, camera->getScale(), CLInput::get()->getParam("imgSaveFolder"));
        OpenPoseCommandLine::processImageFolder(CLInput::get()->getParam("openposeDir"), CLInput::get()->getParam("openposeExec"), CLInput::get()->getParam("imgSaveFolder"), CLInput::get()->getParam("openposeOutputFolder"));
    
        RealSenseOpenCVToSkeleton::createSkeletons (camera, mapping, frameID, OPENPOSE_SKELETON_THRESHOLDING, CLInput::get()->getParam("imgSaveFolder"), CLInput::get()->getParam("openposeOutputFolder"), "Laboratorio_Corridoio");
        SystemExec::executeCommand("rm " + CLInput::get()->getParam("imgSaveFolder") +  "/d/*");
        SystemExec::executeCommand("rm " + CLInput::get()->getParam("imgSaveFolder") +  "/rgb/*");
        SystemExec::executeCommand("rm " + CLInput::get()->getParam("imgSaveFolder") +  "/sk/*");
        SystemExec::executeCommand("rm " + CLInput::get()->getParam("imgSaveFolder") +  "/skeleton/*");
        SystemExec::executeCommand("rm " + CLInput::get()->getParam("imgSaveFolder") +  "/depth/*");
        

        KafkaJSONSend::sendData(frameID, kafka, blockchain, CLInput::get()->getParam("openposeOutputFolder"));
        SystemExec::executeCommand("rm " + CLInput::get()->getParam("openposeOutputFolder") +  "/op/*");
        return 0;
    }

    return 0;
}
