//
//  SystemCommand.hpp
//  AI Watch A1
//
//  Created by Denny Caruso on 23/07/22.
//  Modified by Francesco Pollasto

// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2022. All Rights Reserved.

#ifndef SystemExec_hpp
#define SystemExec_hpp

#include <string>

/**
 * SystemCommand class is a class that represents a command given by the program to the system. 
 * Examples of commands are the following: start and run OpenPose, move files, and clean the build folder 
 * by deleting old files. SystemCommand implements a variant of the Command design pattern. 
 * Each concrete command is realized by a specific subclass that implements the parent abstract method.
 * @brief SystemCommand class is a class that represents a command given by the program to the system. 
 */
namespace SystemExec {
    /**
     * @brief This method is responsible for executing commands on the system based on the filled command string.
     * If the string is empty, then no command is executed.
    */
    inline void executeCommand (std::string command){
        if (command.size() > 0) std::system(command.c_str());
    };
};

#endif /* SystemCommand_hpp */
