
#include "SequentialCommandGroup.h"

namespace atmt {

    SequentialCommandGroup::SequentialCommandGroup(std::vector<Command*> commands):
        Command(),
        m_commands{ commands }
    {

    };
    SequentialCommandGroup::SequentialCommandGroup(SequentialCommandGroup& command):
        Command(command)
    {
        m_commands = command.m_commands;
    };
    SequentialCommandGroup::~SequentialCommandGroup() {
        // Will run ~Command() after this is complete
        for (Command* command : m_commands) {
            delete command;
        }
        m_commands.clear();
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

    };
    bool SequentialCommandGroup::is_finished() {
        return (m_run_index >= static_cast<int>(m_commands.size()));
    };

    void SequentialCommandGroup::addCommand(Command* command) {
        m_commands.push_back(command);
    };

};