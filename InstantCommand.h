
#ifndef AUTOMAT_INSTANT_COMMAND_
#define AUTOMAT_INSTANT_COMMAND_

#include <functional>

#include "Command.h"

namespace atmt {

    class InstantCommand : public Command { // Subsystems? ...
        public:
            InstantCommand(std::function<void()> lambda); // Put subsystems as parameters
            InstantCommand(std::function<void()> lambda, bool continuous);
            InstantCommand(InstantCommand& command); // Copy constructor
            ~InstantCommand();
            Command* clone() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made
            
        private:
            std::function<void()> m_lambda;
            bool m_continuous;
    };

}

#endif