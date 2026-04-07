#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_SEQUENTIAL_COMMAND_GROUP_
#define AUTOMAT_SEQUENTIAL_COMMAND_GROUP_

#include <vector>

#include "../Command.h"

namespace atmt {

    class SequentialCommandGroup : public Command {
        public:
            SequentialCommandGroup(std::vector<Command*> commands);
            SequentialCommandGroup(const SequentialCommandGroup& command); // Copy constructor
            ~SequentialCommandGroup();
            Command* cloneSelf() const override;

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

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use SequentialCommandGroup"
#endif