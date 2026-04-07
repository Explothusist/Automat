#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_PARALLEL_RACE_COMMAND_GROUP_
#define AUTOMAT_PARALLEL_RACE_COMMAND_GROUP_

#include <vector>

#include "../Command.h"

namespace atmt {

    class ParallelRaceCommandGroup : public Command {
        public:
            ParallelRaceCommandGroup(); // For custom versions of this class
            ParallelRaceCommandGroup(std::vector<Command*> commands);
            ParallelRaceCommandGroup(const ParallelRaceCommandGroup& command); // Copy constructor
            ~ParallelRaceCommandGroup();
            Command* cloneSelf() const override;

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
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use ParallelRaceCommandGroup"
#endif