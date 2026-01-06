
#ifndef AUTOMAT_SUBSYSTEM_
#define AUTOMAT_SUBSYSTEM_

#include "Command.h"

namespace atmt {

    // class Command;

    class Subsystem {
        public:
            Subsystem();
            virtual ~Subsystem();

            virtual void init(); // User-made, runs when loop starts
            virtual void periodic(); // User-made

            void runPeriodic();
            void setDefaultCommand(Command* command);
            Command* getDefaultCommand();
            bool hasDefaultCommand();
        private:
            Command* m_default_command;
    };

}

#endif