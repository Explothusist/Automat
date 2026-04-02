#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "ParallelCommandGroup.h"

namespace atmt {

    ParallelCommandGroup::ParallelCommandGroup(std::vector<Command*> commands):
        Command(),
        m_commands{ commands }
    {
        for (Command* command : m_commands) {
            for (Subsystem* subsystem : command->getSubsystems()) {
                usesSubsystem(subsystem);
            }
        }
    };
    ParallelCommandGroup::ParallelCommandGroup(const ParallelCommandGroup& command):
        Command(command)
    {
        std::vector<Command*> cloned;
        for (Command* cmd : command.m_commands) {
            cloned.push_back(cmd->clone());
        }
        m_commands = cloned;
    };
    ParallelCommandGroup::~ParallelCommandGroup() {
        // Will run ~Command() after this is complete
        for (Command* command : m_commands) {
            delete command;
        }
        m_commands.clear();
    };
    Command* ParallelCommandGroup::clone() const {
        // std::vector<Command*> cloned;
        // for (Command* command : m_commands) {
        //     cloned.push_back(command->clone());
        // }
        // return new ParallelCommandGroup(cloned);
        return new ParallelCommandGroup(*this);
    };

    void ParallelCommandGroup::initialize() {

    };
    void ParallelCommandGroup::execute() {
        for (Command* command : m_commands) {
            command->runLoop(); // Checks internally if command is already over
        }
    };
    void ParallelCommandGroup::end(bool interrupted) {
        if (interrupted) {
            for (Command* command : m_commands) {
                if (!command->hasFinished()) {
                    command->end(true);
                }
            }
        }
    };
    bool ParallelCommandGroup::is_finished() {
        bool finished = true;
        for (Command* command : m_commands) {
            finished = finished && command->hasFinished();
        }
        return finished;
    };

    void ParallelCommandGroup::addCommand(Command* command) {
        m_commands.push_back(command);
        for (Subsystem* subsystem : command->getSubsystems()) {
            usesSubsystem(subsystem);
        }
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use ParallelCommandGroup"
#endif