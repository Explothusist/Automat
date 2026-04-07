#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "ParallelRaceCommandGroup.h"

namespace atmt {

    ParallelRaceCommandGroup::ParallelRaceCommandGroup():
        Command(),
        m_commands{ }
    {
        
    };
    ParallelRaceCommandGroup::ParallelRaceCommandGroup(std::vector<Command*> commands):
        Command(),
        m_commands{ commands }
    {
        for (Command* command : m_commands) {
            for (Subsystem* subsystem : command->getSubsystems()) {
                usesSubsystem(subsystem);
            }
        }
    };
    ParallelRaceCommandGroup::ParallelRaceCommandGroup(const ParallelRaceCommandGroup& command):
        Command(command)
    {
        std::vector<Command*> cloned;
        for (Command* cmd : command.m_commands) {
            cloned.push_back(cmd->clone());
        }
        m_commands = cloned;
    };
    ParallelRaceCommandGroup::~ParallelRaceCommandGroup() {
        // Will run ~Command() after this is complete
        for (Command* command : m_commands) {
            delete command;
        }
        m_commands.clear();
    };
    Command* ParallelRaceCommandGroup::clone() const {
        // std::vector<Command*> cloned;
        // for (Command* command : m_commands) {
        //     cloned.push_back(command->clone());
        // }
        // return new ParallelRaceCommandGroup(cloned);
        return new ParallelRaceCommandGroup(*this);
    };

    void ParallelRaceCommandGroup::initialize() {

    };
    void ParallelRaceCommandGroup::execute() {
        for (Command* command : m_commands) {
            bool done = command->runLoop(); // Checks internally if command is already over
            if (done) {
                break;
            }
        }
    };
    void ParallelRaceCommandGroup::end(bool interrupted) {
        // if (interrupted) {
        for (Command* command : m_commands) {
            if (!command->hasFinished()) {
                command->end(true);
            }
        }
        // }
    };
    bool ParallelRaceCommandGroup::is_finished() {
        bool finished = false;
        for (Command* command : m_commands) {
            finished = finished || command->hasFinished();
        }
        return finished;
    };

    void ParallelRaceCommandGroup::addCommand(Command* command) {
        m_commands.push_back(command);
        for (Subsystem* subsystem : command->getSubsystems()) {
            usesSubsystem(subsystem);
        }
    };

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use ParallelRaceCommandGroup"
#endif