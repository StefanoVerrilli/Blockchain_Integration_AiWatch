// FP + Denny Caruso

#ifndef FILEDEL_CPP
#define FILEDEL_CPP

#include "FileDeleter.hpp"

FileDeleter* FileDeleter::instance = nullptr;

void FileDeleter::setForDeletion(std::string filePath) {
    deletionQueue.enqueue(filePath);
}


void FileDeleter::deleteFile() {
    std::string filePath = deletionQueue.dequeue();
    std::stringstream cleanTerminalCommand;
    cleanTerminalCommand << "rm -r " << filePath << " > /dev/null";
    SystemExec::executeCommand(std::string(cleanTerminalCommand.str()));
}

void FileDeleter::deleteFiles(int n) {
    for(int i=0;i<n; i++) {
        deleteFile();
    }
}

//TODO: rimuovere qui:

FileDeleter* FileDeleter::get() {
    if(instance == nullptr)
        instance = new FileDeleter();
    return instance;
}

#endif