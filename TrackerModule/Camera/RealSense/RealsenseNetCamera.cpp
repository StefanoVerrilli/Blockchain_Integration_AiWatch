#include "RealsenseNetCamera.hpp"

std::string RealSenseNetCamera::getIPAutomatically() {
    boost::asio::io_service service;
    boost::asio::ip::tcp::acceptor serverAcceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), LISTEN_PORT ));
     std::cout << "Waiting for camera connection. Please connect to this computer IP on port " << LISTEN_PORT << " from your Raspberry Pi. Remember to start rs-server before." << std::endl;
    boost::asio::ip::tcp::socket socket(service);
    serverAcceptor.accept(socket);
    boost::asio::ip::tcp::endpoint remoteEndPoint = socket.remote_endpoint();
    boost::asio::ip::address remoteAddress = remoteEndPoint.address();
    std::string IPAddress = remoteAddress.to_string();
    std::cout << "Found device " << IPAddress << std::endl;
    boost::asio::write( socket, boost::asio::buffer("Connected") );
    socket.close();
    return IPAddress;
}

RealSenseNetCamera::RealSenseNetCamera(std::string IP) : RealSenseCamera(640,480) {
    rs2::net_device dev(IP);
    rs2::context networkRS_context; 
    dev.add_to(networkRS_context);
    setContext(networkRS_context);
    cameraIP = IP;
}

std::string RealSenseNetCamera::getIP(void) {
    return this->cameraIP;
}