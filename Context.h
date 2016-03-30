/*
* Copyright (c) 2016
*  Somebody
*/
#ifndef SPVR_CONTEXT_H
#define SPVR_CONTEXT_H

#include "openvr_driver.h"

#include <memory>

namespace spvr
{

class ControlInterface;
class Logger;

class Context final
{
    Context();
    Context(Context const &) = delete;
    Context(Context &&) = delete;

public:
    ~Context();
    static Context &GetInstance();
    static void Destroy();

    ControlInterface &GetControlInterface();
    Logger &GetLogger();

private:
    std::unique_ptr<ControlInterface> m_pControlInterface;
    std::unique_ptr<Logger> m_pLogger;
};

} // namespace spvr

#endif // SPVR_CONTEXT_H
