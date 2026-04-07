#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_COMMAND_
#define AUTOMAT_COMMAND_

#include <vector>

#include "../utils.h"

namespace atmt {

    class Subsystem;

    class Command {
        public:
            Command(); // Put subsystems as parameters
            Command(const Command& command); // Copy constructor
            virtual ~Command();
            Command* clone() const;
            virtual Command* cloneSelf() const = 0;

            bool runLoop();

            virtual void initialize(); // User-made
            virtual void execute(); // User-made
            virtual void end(bool interrupted); // User-made
            virtual bool is_finished(); // User-made
            
            std::vector<Subsystem*> getSubsystems();
            bool hasMatchingSubsystems(Command* command);
            bool hasSubsystem(Subsystem* subsystem);
            bool hasFinished();

            void usesSubsystem(Subsystem* subsystem); // Call repeatedly for each subsystem used
            void usesSubsystems(std::initializer_list<Subsystem*> subsystems); // Call repeatedly for each subsystem used

            void setId(int id); // For use by atmt::Joystick
            int getId();
            void setSerialMessageId(int id);
            int getSerialMessageId();

            Command* withTimeout(double seconds);
        private:
            bool m_is_first_run;
            bool m_was_interrupted;
            bool m_has_finished;

            std::vector<Subsystem*> m_subsystems;

            int m_id;

            bool m_has_timeout;
            Timestamp m_command_start;
            double m_seconds_to_run;

            int m_serial_message_id;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Command"
#endif