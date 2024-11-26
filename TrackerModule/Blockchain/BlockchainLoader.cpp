
#include "BlockchainLoader.hpp"


BlockchainLoader::BlockchainLoader(){
    pybind11::initialize_interpreter();
    //pybind11::module sys = pybind11::module::import("sys");
    //sys.attr("path").attr("insert")(1, CUSTOM_SYS_PATH);
}

BlockchainLoader::~BlockchainLoader(){
    pybind11::finalize_interpreter();
}

std::string BlockchainLoader::getHash(Json::Value Json){
    auto HashingModule = pybind11::module::import("blockchain_code.Hashing");
    auto func = HashingModule.attr("hashToLoad");
    return func(Json.toStyledString()).cast<std::string>();
} 


void BlockchainLoader::LoadToBlockchain(std::string Hash){
    auto BlockchainModule = pybind11::module::import("blockchain_code.Tracker_Module").attr("sendToBlockchain");
    BlockchainModule(Hash);
}
