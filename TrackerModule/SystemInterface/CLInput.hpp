// Gets data from configuration file and puts it in a map
// FP


#include <iostream>
#include <vector>
#include <stdexcept>
#include <map>
#include <fstream>

#ifndef CLINPUT_HPP
#define CLINPUT_HPP

class CLInput {
    private:
        static CLInput* inputParams;
        std::string confFilePath;
        std::map<std::string, std::string> parameters;
        CLInput(std::string confFilePath) {
            this->confFilePath = confFilePath;

            std::ifstream inputFile(confFilePath);
            std::string configSingleLine;

            std::string key; int pos;
            // Read usage's configuration parameters from file
            while (inputFile >> configSingleLine) {
               try {
                    key = configSingleLine.substr(0, pos = configSingleLine.find('='));
               } catch( std::out_of_range& exception ) {
                    continue;
               }
                parameters[key] = configSingleLine.substr(pos+1);
            }
        };
    public:
        static CLInput *get() {
            if(inputParams == nullptr) {
                throw std::runtime_error("[AI_WATCH_A1] Error: Tried getting CLInput class before building it (use build(...) first).");
            } 
            return inputParams;   
        };
        static CLInput *build(std::string _confFilePath) {
            if(inputParams == nullptr) {
                inputParams = new CLInput(_confFilePath);
            }

            return inputParams;
        };
        CLInput(const CLInput& clone) = delete; // no clone
        void operator=(const CLInput&) = delete; // no assign
        std::string getParam(std::string identifier) {
            return parameters[identifier];
        };
};

#endif