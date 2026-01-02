
#ifndef AUTOMAT_COMMAND_
#define AUTOMAT_COMMAND_

#include <vector>

// #include "Subsystem.h"

namespace atmt {

    class Subsystem;

    class Command {
        public:
            Command(); // Put subsystems as parameters
            Command(Command& command); // Copy constructor
            virtual ~Command();

            bool runLoop();

            virtual void initialize(); // User-made
            virtual void execute(); // User-made
            virtual void end(bool interrupted); // User-made
            virtual bool is_finished(); // User-made
            
            std::vector<Subsystem*> getSubsystems();
            bool hasMatchingSubsystems(Command* command);
            bool hasSubsystem(Subsystem* subsystem);

            void usesSubsystem(Subsystem* subsystem); // Call repeatedly for each subsystem used

            void setId(int id); // For use by atmt::Robot
            int getId();

            // void withTimeout(double seconds); // WORKING HERE
        private:
            bool m_is_first_run;
            bool m_was_interrupted;

            std::vector<Subsystem*> m_subsystems;

            int m_id;
    };

}

#endif