/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SPVR_CONTROLINTERFACE_H
#define SPVR_CONTROLINTERFACE_H

#include "glm/gtc/quaternion.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace spvr
{

class ControlInterface final
{
public:
    ControlInterface();
    ~ControlInterface();

    // logging
    void Log(std::string const &strMessage);
    std::string const PullLog();

    void SetRotation(glm::quat const &qRotation);
    glm::quat const GetRotation() const;

    void SetDistortionCoefficients(float k0, float k1);
    // returns true if non-default values
    bool GetDistortionCoefficients(float &k0, float &k1) const;

    void SetDistortionScale(float scale);
    float GetDistortionScale() const;

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

} // namespace spvr

#endif // SPVR_CONTROLINTERFACE_H
