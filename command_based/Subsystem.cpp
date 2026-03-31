#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Subsystem.h"

namespace atmt {

    Subsystem::Subsystem(): 
        m_default_command{ nullptr }
    {

    };
    Subsystem::~Subsystem() {
        // delete m_default_command;
        m_default_command = nullptr;
    };

    void Subsystem::runPeriodic(RobotState state) {
        systemPeriodic();
        switch (state) {
            case Disabled:
                disabledPeriodic();
                break;
            case Autonomous:
                autonomousPeriodic();
                break;
            case Teleop:
                teleopPeriodic();
                break;
        }
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
        Command* copy { m_default_command->clone() };
        return copy;
    };
    bool Subsystem::hasDefaultCommand() {
        return m_default_command != nullptr;
    };

    void Subsystem::init() {};
    void Subsystem::systemPeriodic() {};
    void Subsystem::disabledPeriodic() {};
    void Subsystem::autonomousPeriodic() {};
    void Subsystem::teleopPeriodic() {};

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Subsystem"
#endif