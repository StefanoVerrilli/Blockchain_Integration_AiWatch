#ifndef FILEDEL_HPP
#define FILEDEL_HPP

#include <sstream>
#include "../Utilities/SyncQueue.hpp"
#include "SystemExec.hpp"


class FileDeleter {
    private:
        SyncQueue<std::string> deletionQueue;
        FileDeleter() {};
        static FileDeleter *instance;
    public:
        void deleteFile();
        void deleteFiles(int);
        void setForDeletion(std::string filePath);
        static FileDeleter* get();
};

#endif