#include <iostream>
#include <jsoncpp/json/value.h>
#include <pybind11/embed.h>

#ifndef BlockchainLoader_hpp
#define BlockchainLoader_hpp

using namespace pybind11::literals;


class BlockchainLoader{
private:
    //pybind11::scoped_interpreter BlockchainModule;
public:

    BlockchainLoader();

    std::string getHash(Json::Value json);

    void LoadToBlockchain(std::string Hash);

    ~BlockchainLoader();

};

#endif