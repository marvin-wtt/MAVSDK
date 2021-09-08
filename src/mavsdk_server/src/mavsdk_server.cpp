#include "mavsdk_server.h"

#include <memory>

#include "connection_initiator.h"
#include "mavsdk.h"
#include "grpc_server.h"

using namespace mavsdk::mavsdk_server;

class MavsdkServer::Impl {
public:
    Impl() {}
    ~Impl() {}

    bool connect(const std::string& connection_url)
    {
        _connection_initiator.start(_mavsdk, connection_url);
        return _connection_initiator.wait();
    }

    int startGrpcServer(const int port)
    {
        _server = std::make_unique<GrpcServer>(_mavsdk);
        _server->set_port(port);
        _grpc_port = _server->run();
        return _grpc_port;
    }

    void wait() { _server->wait(); }

    void stop()
    {
        _connection_initiator.cancel();
        _server->stop();
    }

    int getPort() { return _grpc_port; }

private:
    mavsdk::Mavsdk _mavsdk;
    ConnectionInitiator<mavsdk::Mavsdk> _connection_initiator;
    std::unique_ptr<GrpcServer> _server;
    int _grpc_port;
};

MavsdkServer::MavsdkServer() : _impl(std::make_unique<Impl>()) {}
MavsdkServer::~MavsdkServer() = default;

int MavsdkServer::startGrpcServer(const int port)
{
    return _impl->startGrpcServer(port);
}

bool MavsdkServer::connect(const std::string& connection_url)
{
    return _impl->connect(connection_url);
}

void MavsdkServer::wait()
{
    _impl->wait();
}

void MavsdkServer::stop()
{
    _impl->stop();
}

int MavsdkServer::getPort()
{
    return _impl->getPort();
}
