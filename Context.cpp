/*
* Copyright (c) 2016
*  Somebody
*/
#include "Context.h"

#include "ControlInterface.h"
#include "Logger.h"

namespace smartvr
{

static std::unique_ptr<Context> S_pContext{};

Context::Context():
m_pControlInterface{std::make_unique<ControlInterface>()},
m_pLogger{std::make_unique<Logger>(m_pControlInterface.get())}
{
    m_pLogger->Log("smartvr::Context initialized");
}

Context::~Context() = default;

Context &Context::GetInstance()
{
    if (!S_pContext)
    {
        S_pContext = std::unique_ptr<Context>{new Context};
    }
    return *S_pContext;
}

void Context::Destroy()
{
    if (S_pContext)
    {
        S_pContext.reset();
    }
}

ControlInterface &Context::GetControlInterface()
{
    return *m_pControlInterface;
}

Logger &Context::GetLogger()
{
    return *m_pLogger;
}

} // namespace smartvr
