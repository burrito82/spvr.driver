/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SVR_CONTROLINTERFACE_H
#define SVR_CONTROLINTERFACE_H

#include <memory>
#include <stdexcept>
#include <string>

namespace smartvr
{

class ControlInterface final
{
public:
    static std::unique_ptr<ControlInterface> S_pInstance;

    ControlInterface();
    ~ControlInterface();

    // logging
    void Log(std::string const &strMessage);
    std::string const PullLog();

    class ControlInterfaceException final : std::runtime_error
    {
    public:
        explicit ControlInterfaceException(char const *pErrorMsg):
            std::runtime_error{pErrorMsg}
        {

        }

        explicit ControlInterfaceException(std::string const &strErrorMsg):
            std::runtime_error{strErrorMsg}
        {

        }
    };
private:
    class ControlInterfaceImpl;
    std::unique_ptr<ControlInterfaceImpl> m_pImpl;
};

} // namespace smartvr

#endif // SVR_CONTROLINTERFACE_H
