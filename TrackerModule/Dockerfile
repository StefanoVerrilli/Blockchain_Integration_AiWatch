# AI Watch A1 Dockerfile

FROM ubuntu:22.04
LABEL author="Francesco Pollasto"
LABEL version="0.1"
LABEL description="AI Watch A1 image for Linux systems"

# Disable Prompt During Packages Installation
#ARG DEBIAN_FRONTEND=noninteractive

RUN apt update -y && apt upgrade -y

# tzdata workaround by https://rtfm.co.ua/en/docker-configure-tzdata-and-timezone-during-build/
ENV TZ=Europe/Rome 
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# APT Dependencies, append next
RUN apt-get -y install sudo git cmake libssl-dev libusb-1.0-0-dev libudev-dev pkg-config libgtk-3-dev python3-dev python3 python3-pip libopencv-dev libglfw3-dev libjsoncpp-dev librdkafka-dev libzstd-dev liblz4-dev libsasl2-dev wget libglu1-mesa libxi-dev libxmu-dev libglu1-mesa-dev

# PIP (Python) dependencies, append next
RUN python3 -m pip install numpy opencv-python "pybind11[global]"

# librealsense v2.47.0 has support for RTSP for RealSense cameras, other versions may not support it
ENV PATH=${PATH}:/usr/local/lib/
ENV LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib/:/lib

WORKDIR /home/Dependencies/RealSense
RUN wget https://github.com/IntelRealSense/librealsense/archive/refs/tags/v2.47.0.tar.gz
RUN tar xvzf v2.47.0.tar.gz
WORKDIR librealsense-2.47.0 
RUN sed -i '19 s/^/#/' tools/rs-server/CMakeLists.txt # there's an error when compiling, it can be resolved by commenting line 19 of this file
WORKDIR build
#RUN git clone https://github.com/IntelRealSense/librealsense.git librealsense
#WORKDIR librealsense/build
#RUN git checkout tags/v2.47.0
RUN cmake -DBUILD_EXAMPLES=true -DCMAKE_BUILD_TYPE=Release -DBUILD_WITH_OPENMP=true -DBUILD_NETWORK_DEVICE=ON ..
RUN sudo make uninstall && make clean && make -j8
RUN make install -j8
WORKDIR /home/Dependencies
#RUN rm -r RealSense

# OpenPose
WORKDIR /home/Dependencies/OpenPose
RUN git clone https://github.com/CMU-Perceptual-Computing-Lab/openpose openpose
WORKDIR openpose
#RUN git submodule update --init --recursive --remote
RUN sh scripts/ubuntu/install_deps.sh
WORKDIR build
RUN cmake -DBUILD_PYTHON=true -DGPU_MODE=CPU_ONLY -DBUILD_CAFFE=true ..
RUN make -j8
RUN make install -j8

WORKDIR /home/Dependencies
COPY requirements.txt requirements.txt
RUN python3 -m pip install -r requirements.txt

WORKDIR /home/tracker


