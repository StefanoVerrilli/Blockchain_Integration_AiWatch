#include "../SystemInterface/SystemExec.hpp"

namespace OpenPoseCommandLine {
    void processImageFolder(std::string openPoseFolder, std::string openPoseExecuteCommand, std::string imagesFolder, std::string outputFolder = "op-output") {
        std::stringstream openPoseTerminalCommand;
        
        openPoseTerminalCommand << "cd " << openPoseFolder << " && " << openPoseExecuteCommand
                                << " --net_resolution -1x128 --display 0 --render_pose 0 --image_dir "
                                << imagesFolder << "/rgb/" << " --write_json " << outputFolder << "/op/"
                                << " --logging_level 255 > /dev/null";
        SystemExec::executeCommand(openPoseTerminalCommand.str());
    }
};