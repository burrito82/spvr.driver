/*
* Copyright (c) 2016
*  Somebody
*/
#include "Logger.h"

#include "ControlInterface.h"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

namespace smartvr
{

Logger::Logger(ControlInterface *pControlInterface):
    m_pControlInterface{pControlInterface},
    m_vecDriverLogs{}
{

}

void Logger::AddDriverLog(vr::IDriverLog *pDriverLog)
{
    if (std::find(begin(m_vecDriverLogs), end(m_vecDriverLogs), pDriverLog) == std::end(m_vecDriverLogs))
    {
        m_vecDriverLogs.push_back(pDriverLog);
    }
}

void Logger::RemoveDriverLog(vr::IDriverLog *pDriverLog)
{
    auto it = std::find(begin(m_vecDriverLogs), end(m_vecDriverLogs), pDriverLog);
    if (it != std::end(m_vecDriverLogs))
    {
        m_vecDriverLogs.erase(it);
    }
}

void Logger::Log(const char *pchLogMessage)
{
    if (m_pControlInterface)
    {
        m_pControlInterface->Log(std::string{pchLogMessage});
    }
    for (auto pDriverLog : m_vecDriverLogs)
    {
        pDriverLog->Log(pchLogMessage);
    }
}

void Logger::Log(std::string const &strLogMessage)
{
    if (m_pControlInterface)
    {
        m_pControlInterface->Log(strLogMessage);
    }
    for (auto pDriverLog : m_vecDriverLogs)
    {
        pDriverLog->Log(strLogMessage.c_str());
    }
}

} // namespace smartvr
