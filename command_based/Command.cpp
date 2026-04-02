#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "Command.h"

namespace atmt {

    Command::Command(): // Put Subsystems as parameters
        m_is_first_run{ true }, // Will be overriden, marks when to run initialize
        m_was_interrupted{ true }, // Set to false when exiting normally, ensures end() runs once
        m_has_finished{ false }, // Set to true when isFinished returns true
        m_id{ -1 },
        m_has_timeout{ false },
        m_command_start{ Timestamp(0) }
    {
        // usesSubsystem(ex_subsystem); // Call repeatedly for each Subsystem used
    };
    Command::Command(const Command& command) {
        m_is_first_run = command.m_is_first_run;
        m_was_interrupted = command.m_was_interrupted;
        m_has_finished = command.m_has_finished;
        // m_id = command.m_id; // Set for each instance
        m_subsystems = command.m_subsystems;
        m_has_timeout = command.m_has_timeout;
        m_seconds_to_run = command.m_seconds_to_run;
    };
    Command::~Command() {
        if (m_was_interrupted) {
            end(true);
        }
    };
    // DO NOT Define clone() here (pure virtual)


    bool Command::runLoop() {
        if (m_has_finished) {
            return true;
        }
        if (m_is_first_run) {
            m_command_start = getSystemTime();
            initialize(); // User-made
            m_is_first_run = false;
        }
        execute(); // User-made
        if (is_finished()) { // User-made
            m_was_interrupted = false;
            m_has_finished = true;
            end(false);
            return true;
        }
        if (m_has_timeout) {
            Timestamp now = getSystemTime();
            if (now.getTimeDifferenceMS(m_command_start) > (m_seconds_to_run * 1000)) {
                m_was_interrupted = true; // Timeout does indeed interrupt
                m_has_finished = true;
                // end(true); // Run by destructor
                return true;
            }
        }
        return false;
    };

    bool Command::hasMatchingSubsystems(Command* command) {
        std::vector<Subsystem*> subs = command->getSubsystems();
        for (Subsystem* subsystem : m_subsystems) {
            for (Subsystem* checking : subs) {
                if (subsystem == checking) {
                    return true;
                }
            }
        }
        return false;
    };
    bool Command::hasSubsystem(Subsystem* subsystem) {
        for (Subsystem* checking : m_subsystems) {
            if (subsystem == checking) {
                return true;
            }
        }
        return false;
    };
    bool Command::hasFinished() {
        return m_has_finished;
    };

    std::vector<Subsystem*> Command::getSubsystems() {
        return m_subsystems;
    };
    void Command::usesSubsystem(Subsystem* subsystem) {
        m_subsystems.push_back(subsystem);
    }
    void Command::usesSubsystems(std::initializer_list<Subsystem*> subsystems) {
        for (Subsystem* subsystem : subsystems) {
            usesSubsystem(subsystem);
        }
    };

    void Command::setId(int id) {
        m_id = id;
    };
    int Command::getId() {
        return m_id;
    };

    Command* Command::withTimeout(double seconds) {
        m_has_timeout = true;
        m_seconds_to_run = seconds;
        return this;
    };

    void Command::initialize() {};
    void Command::execute() {};
    void Command::end(bool interrupted) {};
    bool Command::is_finished() {return false;};

}

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Command"
#endif