#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "WaitCommand.h"

namespace atmt {

    WaitCommand::WaitCommand(double timeout):
        Command(),
        m_timeout_ms{ static_cast<int>(timeout * 1000.0) },
        m_starting_timestamp{ 0 }
    {
        
    };
    WaitCommand::WaitCommand(const WaitCommand& command):
        Command(command)
    {
        m_timeout_ms = command.m_timeout_ms;
    };
    WaitCommand::~WaitCommand() {
        // Will run ~Command() after this is complete
    };
    Command* WaitCommand::cloneSelf() const {
        return new WaitCommand(*this);
    };

    void WaitCommand::initialize() {
        m_starting_timestamp = getSystemTime();
    };
    void WaitCommand::execute() {
        
    };
    void WaitCommand::end(bool interrupted) {

    };
    bool WaitCommand::is_finished() {
        return m_starting_timestamp.getTimeDifferenceMS(getSystemTime()) > m_timeout_ms;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use WaitCommand"
#endif