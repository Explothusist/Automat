
#include "Subsystem.h"

namespace atmt {

    Subsystem::Subsystem(): 
        m_default_command{ nullptr }
    {

    };
    Subsystem::~Subsystem() {
        delete m_default_command;
        m_default_command = nullptr;
    };

    void Subsystem::runPeriodic() {
        periodic();
    };

    void Subsystem::setDefaultCommand(Command* command) {
        if (m_default_command != nullptr) {
            delete m_default_command;
        }
        m_default_command = command;
    };
    Command* Subsystem::getDefaultCommand() {
        if (m_default_command == nullptr) {
            return nullptr;
        }
        Command* copy { new Command(*m_default_command) };
        return copy;
    };
    bool Subsystem::hasDefaultCommand() {
        return m_default_command != nullptr;
    };

    void Subsystem::periodic() {};

}