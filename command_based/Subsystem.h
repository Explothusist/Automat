#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

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
            virtual void systemPeriodic(); // User-made, Runs in all modes
            virtual void disabledPeriodic(); // User-made, Runs in Autonomous
            virtual void autonomousPeriodic(); // User-made, Runs in Autonomous
            virtual void teleopPeriodic(); // User-made, Runs in Telep
            // void periodic(); // User-made

            void runPeriodic(RobotState state);
            void setDefaultCommand(Command* command);
            Command* getDefaultCommand();
            bool hasDefaultCommand();
        private:
            Command* m_default_command;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Subsystem"
#endif