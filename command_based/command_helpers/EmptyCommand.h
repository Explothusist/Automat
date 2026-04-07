#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_EMPTY_COMMAND_
#define AUTOMAT_EMPTY_COMMAND_

#include "../Command.h"

namespace atmt {

    class EmptyCommand : public Command { // Subsystems? ...
        public:
            EmptyCommand(); // Put subsystems as parameters
            EmptyCommand(EmptyCommand& command); // Copy constructor
            ~EmptyCommand();
            Command* cloneSelf() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made
            
        private:

    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use EmptyCommand"
#endif