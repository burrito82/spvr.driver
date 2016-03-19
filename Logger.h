/*
* Copyright (c) 2016
*  Somebody
*/
#ifndef SVR_LOGGER_H
#define SVR_LOGGER_H

#include "openvr_driver.h"

#include <cstdint>
#include <string>
#include <vector>

namespace smartvr
{

class ControlInterface;

class Logger final : public vr::IDriverLog
{
public:
    explicit Logger(ControlInterface *pControlInterface);

    void AddDriverLog(vr::IDriverLog *pDriverLog);
    void RemoveDriverLog(vr::IDriverLog *pDriverLog);

    /** Writes a log message to the log file prefixed with the driver name */
    virtual void Log(const char *pchLogMessage) override;
    void Log(std::string const &strLogMessage);
private:
    ControlInterface *m_pControlInterface;
    std::vector<vr::IDriverLog *> m_vecDriverLogs;
    std::string m_strLinePrefix;
};

} // namespace smartvr

#endif // SVR_LOGGER_H
