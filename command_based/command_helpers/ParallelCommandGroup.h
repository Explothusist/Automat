#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_PARALLEL_COMMAND_GROUP_
#define AUTOMAT_PARALLEL_COMMAND_GROUP_

#include <vector>

#include "../Command.h"

namespace atmt {

    class ParallelCommandGroup : public Command {
        public:
            ParallelCommandGroup(); // For custom versions of this class
            ParallelCommandGroup(std::vector<Command*> commands);
            ParallelCommandGroup(const ParallelCommandGroup& command); // Copy constructor
            ~ParallelCommandGroup();
            Command* clone() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made

            void addCommand(Command* command); // For custom versions of this class
            
        private:
            std::vector<Command*> m_commands;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use ParallelCommandGroup"
#endif