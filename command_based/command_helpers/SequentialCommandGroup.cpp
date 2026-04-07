#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "SequentialCommandGroup.h"

namespace atmt {

    SequentialCommandGroup::SequentialCommandGroup():
        Command(),
        m_commands{ },
        m_run_index{ 0 }
    {
        
    };
    SequentialCommandGroup::SequentialCommandGroup(std::vector<Command*> commands):
        Command(),
        m_commands{ commands },
        m_run_index{ 0 }
    {
        for (Command* command : m_commands) {
            for (Subsystem* subsystem : command->getSubsystems()) {
                usesSubsystem(subsystem);
            }
        }
    };
    SequentialCommandGroup::SequentialCommandGroup(const SequentialCommandGroup& command):
        Command(command)
    {
        std::vector<Command*> cloned;
        for (Command* cmd : command.m_commands) {
            cloned.push_back(cmd->clone());
        }
        m_commands = cloned;
    };
    SequentialCommandGroup::~SequentialCommandGroup() {
        // Will run ~Command() after this is complete
        for (Command* command : m_commands) {
            delete command;
        }
        m_commands.clear();
    };
    Command* SequentialCommandGroup::cloneSelf() const {
        // std::vector<Command*> cloned;
        // for (Command* command : m_commands) {
        //     cloned.push_back(command->clone());
        // }
        // return new SequentialCommandGroup(cloned);
        return new SequentialCommandGroup(*this);
    };

    void SequentialCommandGroup::initialize() {
        
    };
    void SequentialCommandGroup::execute() {
        bool done = m_commands[m_run_index]->runLoop();
        if (done) {
            m_run_index += 1; // is_finished will end before error occurs
        }
    };
    void SequentialCommandGroup::end(bool interrupted) {
        if (interrupted) {
            for (Command* command : m_commands) {
                if (!command->hasFinished()) {
                    command->end(true);
                }
            }
        }
    };
    bool SequentialCommandGroup::is_finished() {
        return (m_run_index >= static_cast<int>(m_commands.size()));
    };

    void SequentialCommandGroup::addCommand(Command* command) {
        m_commands.push_back(command);
        for (Subsystem* subsystem : command->getSubsystems()) {
            usesSubsystem(subsystem);
        }
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use SequentialCommandGroup"
#endif