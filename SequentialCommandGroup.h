
#ifndef AUTOMAT_INSTANT_COMMAND_
#define AUTOMAT_INSTANT_COMMAND_

#include <vector>

#include "Command.h"

namespace atmt {

    class SequentialCommandGroup : public Command {
        public:
            SequentialCommandGroup(std::vector<Command*> commands);
            SequentialCommandGroup(SequentialCommandGroup& command); // Copy constructor
            ~SequentialCommandGroup();
            Command* clone() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made

            void addCommand(Command* command); // For custom versions of this class
            
        private:
            std::vector<Command*> m_commands;
            int m_run_index;
    };

}

#endif