/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "ControlInterface.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>

#include <algorithm>
#include <cstring>
#include <mutex>
#include <string>

namespace smartvr
{

namespace
{

using namespace boost::interprocess;

enum ShmConfig
{
    DRIVER,
    CONTROL
};

#ifdef SVR_SHM_DRIVER
static ShmConfig const S_eShmMode = ShmConfig::DRIVER;
#else // ! SVR_SHM_DRIVER
static ShmConfig const S_eShmMode = ShmConfig::CONTROL;
#endif // ! SVR_SHM_DRIVER
static const char S_aShmName[] = "SmartVR SHM";

class ShmLog final
{
    static std::size_t const S_iLineLength = 256;
    static std::size_t const S_iLines = 64;
public:
    ShmLog():
        m_aLogLines{} // important! initialized memory!
    {

    }

    void Log(std::string const &strMsg)
    {
        if (m_iLinesUnread >= S_iLines)
        {
            return;
        }
        auto const iCharsToCopy = std::min(S_iLineLength, strMsg.size());
        auto itOut = &m_aLogLines[m_iNextWrite * S_iLineLength];
        m_iNextWrite = (m_iNextWrite + 1) % S_iLines;
        ++m_iLinesUnread;
        std::copy(begin(strMsg), begin(strMsg) + iCharsToCopy, itOut);
        itOut[iCharsToCopy] = '\0';
    }

    std::string const GetLine(std::size_t iLine) const
    {
        return std::string{const_cast<char const *>(&m_aLogLines[iLine * S_iLineLength])};
    }

    std::string const GetNext()
    {
        if (m_iLinesUnread > 0)
        {
            auto const iLineIndex = m_iNextRead;
            m_iNextRead = (m_iNextRead + 1) % S_iLines;
            --m_iLinesUnread;
            return GetLine(iLineIndex);
        }
        return "";
    }

private:
    std::size_t volatile m_iLinesUnread = 0;
    std::size_t volatile m_iNextRead = 0;
    std::size_t volatile m_iNextWrite = 0;
    char volatile m_aLogLines[S_iLineLength * S_iLines];
};

struct SharedMemoryContent final
{
    SharedMemoryContent() = default;
    ~SharedMemoryContent() = default;

    ShmLog Log;
    glm::quat m_qRotation;
};

class SharedMemory final
{
public:
    SharedMemory():
        m_pShmObject{},
        m_pMappedRegion{},
        m_pMemoryContent{}
    {
        if (S_eShmMode == ShmConfig::CONTROL)
        {
            m_pShmObject = std::make_unique<windows_shared_memory>(create_only, S_aShmName, read_write, 2u * sizeof(SharedMemoryContent));
        }
        else
        {
            m_pShmObject = std::make_unique<windows_shared_memory>(open_only, S_aShmName, read_write);
        }
        m_pMappedRegion = std::make_unique<mapped_region>(*m_pShmObject, read_write);
        // XXX: dangerous, non-portable... region might not be perfectly aligned
        m_pMemoryContent = new (m_pMappedRegion->get_address()) SharedMemoryContent{};
    }

    ~SharedMemory()
    {
        if (m_pMemoryContent)
        {
            m_pMemoryContent->~SharedMemoryContent();
            m_pMemoryContent = nullptr;
        }
    }

    SharedMemoryContent *operator->()
    {
        return m_pMemoryContent;
    }

    SharedMemoryContent const *operator->() const
    {
        return m_pMemoryContent;
    }

private:
    std::unique_ptr<windows_shared_memory> m_pShmObject;
    std::unique_ptr<mapped_region> m_pMappedRegion;
    SharedMemoryContent *m_pMemoryContent;
};

} // unnamed namespace

class ControlInterface::ControlInterfaceImpl
{
public:
    ControlInterfaceImpl():
        m_oMutex{},
        m_oSharedMemory{}
    {

    }

    void Log(std::string const &strMessage);
    std::string const PullLog();

    void SetRotation(glm::quat const &qRotation);
    glm::quat const &GetRotation() const;
private:
    std::mutex m_oMutex;
    SharedMemory m_oSharedMemory;
};

ControlInterface::ControlInterface():
    m_pImpl{std::make_unique<ControlInterfaceImpl>()}
{

}

ControlInterface::~ControlInterface() = default;


void ControlInterface::Log(std::string const &strMessage)
{
    m_pImpl->Log(strMessage);
}

void ControlInterface::ControlInterfaceImpl::Log(std::string const &strMessage)
{
    m_oSharedMemory->Log.Log(strMessage);
}

std::string const ControlInterface::PullLog()
{
    return m_pImpl->PullLog();
}

std::string const ControlInterface::ControlInterfaceImpl::PullLog()
{
    return m_oSharedMemory->Log.GetNext();
}

void ControlInterface::SetRotation(glm::quat const &qRotation)
{
    m_pImpl->SetRotation(qRotation);
}

void ControlInterface::ControlInterfaceImpl::SetRotation(glm::quat const &qRotation)
{
    m_oSharedMemory->m_qRotation = qRotation;
}

glm::quat const ControlInterface::GetRotation() const
{
    return m_pImpl->GetRotation();
}

glm::quat const &ControlInterface::ControlInterfaceImpl::GetRotation() const
{
    return m_oSharedMemory->m_qRotation;
}

} // namespace smartvr
