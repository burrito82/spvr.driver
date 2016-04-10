/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "PoseUpdater.h"

#include "Context.h"
#include "ControlInterface.h"
#include "HmdDriver.h"
#include "Logger.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <chrono>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>

namespace spvr
{

namespace
{

struct SpvrPacket
{
    float f[4];
    std::int32_t m_iCounter;
};

union SpvrPacketByteHack
{
    char c[sizeof(SpvrPacket)];
    SpvrPacket data;
};

void NtoH(SpvrPacketByteHack &packet)
{
    // not the way to to... UB!
    for (int iByte = 0; iByte < sizeof(packet.c); iByte += 4)
    {
        std::swap(packet.c[iByte], packet.c[iByte + 3]);
        std::swap(packet.c[iByte + 1], packet.c[iByte + 2]);
    }
}

} // unnamed namespace

class PoseUpdater::PoseUpdaterImpl
{
public:
    PoseUpdaterImpl(Logger &rLogger, HmdDriver &rHmdDriver):
        m_rLogger(rLogger),
        m_rHmdDriver(rHmdDriver),
        m_rControlInterface(Context::GetInstance().GetControlInterface()),
        m_bIsConnected{},
        m_bNetworkThreadActive{true},
        m_oNetworkThread{}
    {
        m_oNetworkThread = std::thread{
            std::bind(&PoseUpdaterImpl::ReceiveUdp, this)
        };
    }
    ~PoseUpdaterImpl()
    {
        Shutdown();
    }
    bool GetIsConnected() const
    {
        return m_bIsConnected;
    }
    void Shutdown()
    {
        m_bNetworkThreadActive = false;
        if (m_oNetworkThread.joinable())
        {
            m_oNetworkThread.join();
        }
    }
    void ProcessPacket(SpvrPacket const &packet)
    {
        m_rLogger.Debug("received: {"
            + std::to_string(packet.f[0]) + ", \t"
            + std::to_string(packet.f[1]) + ", \t"
            + std::to_string(packet.f[2]) + ", \t"
            + std::to_string(packet.f[3]) + "}" + "                          \r");//<< std::endl;

        glm::quat qRotation{packet.f[0], packet.f[1], packet.f[2], packet.f[3]};
        qRotation = glm::normalize(qRotation);

        m_rControlInterface.SetRotation(qRotation);
    }

    void ReceiveUdp()
    {
        std::int32_t m_iLastMsg = -1;
        while (m_bNetworkThreadActive)
        {
            try
            {
                using boost::asio::ip::udp;
                boost::asio::io_service oIoService{};
                udp::endpoint oEndpoint{udp::v4(), 4321};
                udp::socket oSocket{oIoService, oEndpoint};

                SpvrPacketByteHack oPacket;
                std::memset(oPacket.c, 0, sizeof(oPacket.c));
                boost::system::error_code oError{};

                while (m_bNetworkThreadActive && oError != boost::asio::error::eof)
                {
                    //auto uBytesRead = oSocket.read_some(boost::asio::buffer(aBuffer.c), oError);
                    //auto uBytesRead = oSocket.receive_from(boost::asio::buffer(aBuffer.c), oEndpoint, 0, oError);
                    auto uBytesRead = oSocket.receive(boost::asio::buffer(oPacket.c));
                    NtoH(oPacket);
                    if (oPacket.data.m_iCounter > m_iLastMsg || oPacket.data.m_iCounter < 1000)
                    {
                        m_iLastMsg = oPacket.data.m_iCounter;
                    }
                    else
                    {
                        continue;
                    }
                    ProcessPacket(oPacket.data);
                }
            }
            catch (...)
            {
                m_rLogger.Log("PoseUpdater::ReceiveUdp => some error occurred...");
            }
        }
    }


private:
    Logger &m_rLogger;
    HmdDriver &m_rHmdDriver;
    ControlInterface &m_rControlInterface;
    bool m_bIsConnected;
    bool m_bNetworkThreadActive;
    std::thread m_oNetworkThread;
};

PoseUpdater::PoseUpdater(Logger &rLogger, HmdDriver &rHmdDriver):
    m_pImpl{std::make_unique<PoseUpdaterImpl>(rLogger, rHmdDriver)}
{

}

PoseUpdater::~PoseUpdater() = default;

bool PoseUpdater::GetIsConnected() const
{
    return m_pImpl->GetIsConnected();
}

void PoseUpdater::Shutdown()
{
    m_pImpl->Shutdown();
}

} // namespace spvr
