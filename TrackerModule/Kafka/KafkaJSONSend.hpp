#include "KafkaManager.hpp"
#include "../Blockchain/BlockchainLoader.hpp"

namespace KafkaJSONSend {
    void sendData (unsigned int frameID, KafkaManager* kafka, BlockchainLoader* loader, std::string outputFolder = "op-output"){
        Json::Value currentJSON;
        std::stringstream outputJsonFilePath;
        outputJsonFilePath << outputFolder << "/movement/frame" << frameID << "_skeletonsPoints3D.json";
        if (JSONOutput::loadJSON(outputJsonFilePath.str(), currentJSON)) {
            std::string hash = loader->getHash(currentJSON);
            loader->LoadToBlockchain(hash);
            std::string key = std::to_string(frameID);
            kafka->sendData(key.c_str(), currentJSON);
        }
    }
}