#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_INSTANT_COMMAND_
#define AUTOMAT_INSTANT_COMMAND_

#include <functional>
#include <utility>
#include <initializer_list>

#include "Command.h"

namespace atmt {

    /*
        Continuous means it runs on EVERY Execute
        Usage:

        InstantCommand cmd({&drive}, [] {
            resetEncoders();
        }); 

        InstantCommand cmd({&arm}, [](int pos) {
            moveArm(pos);
        }, 42);

        InstantCommand cmd({&drive}, true, [](double speed) {
            arcadeDrive(speed);
        }, 0.6);

        InstantCommand cmd({&arm}, &Arm::moveTo, &arm, 100);
    */
    class InstantCommand : public Command {
        public:
            template<typename Callable, typename... Args>
            InstantCommand(std::initializer_list<Subsystem*> subsystems, Callable&& function, Args&& ...args): 
                // m_lambda{ [function = std::forward<Callable>(function), ...args = std::forward<Args>(args)]() mutable { std::invoke(function, args...); } }, // C++20
                m_lambda{ std::bind(std::forward<Callable>(function), std::forward<Args>(args)...) }, // C++11
                m_continuous{ false }
            {
                usesSubsystem(subsystems);
            };
            template<typename Callable, typename... Args>
            InstantCommand(std::initializer_list<Subsystem*> subsystems, bool continuous, Callable&& function, Args&& ...args): 
                // m_lambda{ [function = std::forward<Callable>(function), ...args = std::forward<Args>(args)]() mutable { std::invoke(function, args...); } }, // C++20
                m_lambda{ std::bind(std::forward<Callable>(function), std::forward<Args>(args)...) }, // C++11
                m_continuous{ continuous }
            {
                usesSubsystem(subsystems);
            };
            InstantCommand(const InstantCommand& command) = default; // Copy constructor
            ~InstantCommand() = default;
            Command* clone() const override {
                return new InstantCommand(*this);
            };

            void initialize() override {

            };
            void execute() override {
                if (m_lambda) {
                    m_lambda();
                }
            };
            void end(bool interrupted) override {

            };
            bool is_finished() override {
                return !m_continuous;
            };
            
        private:
            std::function<void()> m_lambda;
            bool m_continuous;
    };

    // class InstantCommand : public Command { // Subsystems? ...
    //     public:
    //         InstantCommand(std::function<void(void*)> lambda, void* arg); // Put subsystems as parameters
    //         InstantCommand(std::function<void(void*)> lambda, void* arg, bool continuous);
    //         InstantCommand(InstantCommand& command); // Copy constructor
    //         ~InstantCommand();
    //         Command* clone() const override;

    //         void initialize() override; // User-made
    //         void execute() override; // User-made
    //         void end(bool interrupted) override; // User-made
    //         bool is_finished() override; // User-made
            
    //     private:
    //         std::function<void()> m_lambda;
    //         bool m_continuous;
    // };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use InstantCommand"
#endif