#include "../../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_SEQUENTIAL_COMMAND_GROUP_
#define AUTOMAT_SEQUENTIAL_COMMAND_GROUP_

#include <vector>

#include "../Command.h"
#include "../../utils.h"

namespace atmt {

    class SequentialCommandGroup : public Command {
        public:
            SequentialCommandGroup(); // For custom versions of this class
            SequentialCommandGroup(std::vector<Command*> commands);
            SequentialCommandGroup(const SequentialCommandGroup& command); // Copy constructor
            ~SequentialCommandGroup();
            Command* cloneSelf() const override;
            void resetFromClone(SequentialCommandGroup* clone);

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made

            void addCommand(Command* command); // For custom versions of this class

            SequentialCommandGroup* race(bool race); // Only complete one command
            SequentialCommandGroup* only_execute(int count); // Only completes x commands
            SequentialCommandGroup* continuous(bool continuous); // Never ends
            SequentialCommandGroup* random(bool random); // Randomizes order
            
        private:
            std::vector<Command*> m_commands;
            std::vector<int> m_indexes;
            int m_run_index;

            int m_max_execute;
            int m_execute_count;
            bool m_is_continuous;
            bool m_is_random;

            SequentialCommandGroup* m_copy;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use SequentialCommandGroup"
#endif