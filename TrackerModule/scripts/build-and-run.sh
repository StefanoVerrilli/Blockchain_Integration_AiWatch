#! /bin/bash

## COLORS ##
RED="\e[31m"
GREEN="\e[42m"
CEND="\e[0m"


echo "     ___       __     ____    __    ____  ___   .___________.  ______  __    __    ";
echo "    /   \     |  |    \   \  /  \  /   / /   \  |           | /      ||  |  |  |   ";
echo "   /  ^  \    |  |     \   \/    \/   / /  ^  \ \`---|  |----\`|  ,----'|  |__|  | ";
echo "  /  /_\  \   |  |      \            / /  /_\  \    |  |     |  |     |   __   |   ";
echo " /  _____  \  |  |       \    /\    / /  _____  \   |  |     |  \`----.|  |  |  |  ";
echo "/__/     \__\ |__|        \__/  \__/ /__/     \__\  |__|      \______||__|  |__|   ";
echo "                                                                                   ";



if ! -d build
then
    echo "Creating build directory"
    mkdir build
    if [[ $? != 0 ]]
    then
        echo -e "${RED}✘ Could not generate build directory, please check your permissions.${CEND}\n"
	exit 1
    fi
fi

cd build
cmake .. 

if [ $? != 0 ]
then
	echo -e "${RED}✘ CMake: failed. Please check if you installed all the required dependencies.${CEND}\n"
	exit 2
fi

echo -e "${GREEN}✔ CMake: success.${CEND}\n"

make -j8
if [ $? != 0 ]
then
	echo -e "${RED}✘ Make: compilation failed.${CEND}\n"
	exit 3
fi

echo -e "${GREEN}✔ [SUCCESS!] Running compiled Ai-Watch...${CEND}\n"

sleep 3 

clear

export OPENCV_IO_ENABLE_OPENEXR=1


./AI_Watch_A1
