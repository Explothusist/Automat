#ifndef AUTOMAT_EMPTY_COMMAND_
#define AUTOMAT_EMPTY_COMMAND_

#include "Command.h"

namespace atmt {

    class EmptyCommand : public Command { // Subsystems? ...
        public:
            EmptyCommand(); // Put subsystems as parameters
            EmptyCommand(EmptyCommand& command); // Copy constructor
            ~EmptyCommand();
            Command* clone() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made
            
        private:

    };

}

#endif