/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SPVR_POSEUPDATER_H
#define SPVR_POSEUPDATER_H

#include <memory>

namespace spvr
{

class HmdDriver;
class Logger;

class PoseUpdater final
{
public:
    PoseUpdater(Logger &rLogger, HmdDriver &rHmdDriver);
    ~PoseUpdater();

    bool GetIsConnected() const;
    void Shutdown();

private:
    class PoseUpdaterImpl;
    std::unique_ptr<PoseUpdaterImpl> m_pImpl;
};

} // namespace spvr

#endif // SPVR_POSEUPDATER_H
