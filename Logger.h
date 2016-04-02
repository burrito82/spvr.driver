/*
* Copyright (c) 2016
*  Somebody
*/
#ifndef SPVR_LOGGER_H
#define SPVR_LOGGER_H

#include "openvr_driver.h"

#include <cstdint>
#include <string>
#include <vector>

namespace spvr
{

class ControlInterface;

class Logger final : public vr::IDriverLog
{
public:
    explicit Logger(ControlInterface *pControlInterface);
    ~Logger();

    void AddDriverLog(vr::IDriverLog *pDriverLog);
    void RemoveDriverLog(vr::IDriverLog *pDriverLog);

    /** Writes a log message to the log file prefixed with the driver name */
    virtual void Log(const char *pchLogMessage) override;
    void Log(std::string const &strLogMessage);
    void Debug(std::string const &strLogMessage);
private:
    ControlInterface *m_pControlInterface;
    std::vector<vr::IDriverLog *> m_vecDriverLogs;
    std::string m_strDebugLinePrefix;
};

} // namespace spvr

#endif // SPVR_LOGGER_H
