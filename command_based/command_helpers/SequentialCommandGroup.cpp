#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "SequentialCommandGroup.h"

namespace atmt {

    SequentialCommandGroup::SequentialCommandGroup():
        Command(),
        m_commands{ },
        m_run_index{ 0 },
        m_max_execute{ 0 },
        m_execute_count{ 0 },
        m_is_continuous{ false },
        m_is_random{ false },
        m_copy{ nullptr }
    {
        
    };
    SequentialCommandGroup::SequentialCommandGroup(std::vector<Command*> commands):
        Command(),
        m_commands{ commands },
        m_run_index{ 0 },
        m_max_execute{ 0 },
        m_execute_count{ 0 },
        m_is_continuous{ false },
        m_is_random{ false },
        m_copy{ nullptr }
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
        m_max_execute = command.m_max_execute;
        m_is_continuous = command.m_is_continuous;
        m_is_random = command.m_is_random;
    };
    SequentialCommandGroup::~SequentialCommandGroup() {
        // Will run ~Command() after this is complete
        for (Command* command : m_commands) {
            delete command;
        }
        m_commands.clear();
        if (m_copy != nullptr) {
            delete m_copy;
            m_copy = nullptr;
        }
    };
    Command* SequentialCommandGroup::cloneSelf() const {
        // std::vector<Command*> cloned;
        // for (Command* command : m_commands) {
        //     cloned.push_back(command->clone());
        // }
        // return new SequentialCommandGroup(cloned);
        return new SequentialCommandGroup(*this);
    };
    void SequentialCommandGroup::resetFromClone(SequentialCommandGroup* clone) {
        for (Command* command : m_commands) {
            delete command;
        }
        m_commands.clear();
        for (Command* command : clone->m_commands) {
            m_commands.push_back(command->clone());
        }
        m_max_execute = clone->m_max_execute;
        m_is_continuous = clone->m_is_continuous;
        m_is_random = clone->m_is_random;
        m_run_index = 0;
        m_execute_count = 0;
    };

    void SequentialCommandGroup::initialize() {
        if (m_is_random) {
            m_indexes.clear();
            for (int i = 0; i < static_cast<int>(m_commands.size()); i++) {
                m_indexes.push_back(i);
            }
            std::shuffle(m_indexes.begin(), m_indexes.end(), g_random_generator);
        }
        if (m_is_continuous) {
            m_copy = clone();
        }
    };
    void SequentialCommandGroup::execute() {
        bool done = m_commands[!m_is_random ? m_run_index : m_indexes[m_run_index]]->runLoop();
        if (done) {
            m_run_index += 1; // is_finished will end before error occurs
            m_execute_count += 1;
            if (m_is_continuous && m_run_index >= static_cast<int>(m_commands.size())) {
                // m_run_index = 0;
                // if (m_is_random) {
                //     m_indexes.clear();
                //     for (int i = 0; i < static_cast<int>(m_commands.size()); i++) {
                //         m_indexes.push_back(i);
                //     }
                //     std::shuffle(m_indexes.begin(), m_indexes.end(), g_random_generator);
                // }
                // this = m_copy;
                resetFromClone(m_copy);
            }
        }
    };
    void SequentialCommandGroup::end(bool interrupted) {
        if (interrupted) {
            // for (Command* command : m_commands) {
            //     if (!command->hasFinished()) {
            //         command->end(true);
            //     }
            // }
            m_commands[!m_is_random ? m_run_index : m_indexes[m_run_index]]->end(true);
        }
    };
    bool SequentialCommandGroup::is_finished() {
        return (m_run_index >= static_cast<int>(m_commands.size()) || (m_max_execute != 0 && m_execute_count >= m_max_execute));
    };

    void SequentialCommandGroup::addCommand(Command* command) {
        m_commands.push_back(command);
        for (Subsystem* subsystem : command->getSubsystems()) {
            usesSubsystem(subsystem);
        }
    };

    SequentialCommandGroup* SequentialCommandGroup::race(bool race) {
        if (race) {
            m_max_execute = 1;
        }else {
            m_max_execute = 0;
        }
        return this;
    }
    SequentialCommandGroup* SequentialCommandGroup::only_execute(int count) {
        m_max_execute = count;
        return this;
    }
    SequentialCommandGroup* SequentialCommandGroup::continuous(bool continuous) {
        m_is_continuous = continuous;
        return this;
    }
    SequentialCommandGroup* SequentialCommandGroup::random(bool random) {
        m_is_random = random;
        return this;
    }

};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use SequentialCommandGroup"
#endif