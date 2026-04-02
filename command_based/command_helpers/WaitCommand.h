#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_WAIT_COMMAND_
#define AUTOMAT_WAIT_COMMAND_

#include <vector>

#include "Command.h"

namespace atmt {

    class WaitCommand : public Command {
        public:
            WaitCommand(double timeout); // seconds
            WaitCommand(const WaitCommand& command); // Copy constructor
            ~WaitCommand();
            Command* clone() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made

        private:
            int m_timeout_ms;
            Timestamp m_starting_timestamp;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use WaitCommand"
#endif