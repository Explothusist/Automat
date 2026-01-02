
#include "TimedRobot.h"

#include "utils.h"

namespace atmt {

    TimedRobot::TimedRobot():
        TimedRobot(0)
    {

    };
    TimedRobot::TimedRobot(int autonomous_length):
        m_subsystems{ },
        m_commands{ },
        m_joysticks{ },
        m_autonomous_command{ new Command() },
#ifdef AUTOMAT_VEX_
        // m_brain{ vex::brain() },
        m_uses_vex_competition{ false },
#endif
        m_state{ Disabled },
        m_old_state{ Disabled },
        m_reseting_state_loop{ false },
        m_had_state_chage{ false },
        m_frame_delay{ 20 },
        m_first_auto_trigger{ true },
        m_autonomous_length{ autonomous_length },
        m_start_of_auto{ 0 }
    {

    };
    TimedRobot::~TimedRobot() {
        m_reseting_state_loop = true;
        clearCommands();
        for (Subsystem* subsystem : m_subsystems) {
            delete subsystem;
        }
        m_subsystems.clear();
        for (Joystick* joystick : m_joysticks) {
            delete joystick;
        }
        m_joysticks.clear();
        delete m_autonomous_command;
        m_autonomous_command = nullptr;
    };

    void TimedRobot::pollState() { // FANCY LOOP
#ifdef AUTOMAT_VEX_
        if (m_uses_vex_competition) { // Otherwise uses interrupts
            if (m_competition.isEnabled()) {
                if (m_competition.isAutonomous()) {
                    m_state = Autonomous;
                }else if (m_competition.isDriverControl()) {
                    m_state = Teleop;
                }
            }else {
                m_state = Disabled;
            }
        }else {
#endif
            if (m_first_auto_trigger) {
                bool triggered = false;
                for (Joystick* joystick : m_joysticks) {
                    triggered = (triggered || joystick->pollAutonomousTriggers());
                }

                if (triggered) {
                    m_state = Autonomous;
                    m_first_auto_trigger = false;
                    
#ifdef AUTOMAT_VEX_
                    m_start_of_auto = vex::timer::system();
#endif
#ifdef AUTOMAT_ESP_32_ // WORKING HERE
#endif
                }
            }else if (m_state == Autonomous) {
#ifdef AUTOMAT_VEX_
                int now = vex::timer::system();

                if (now - m_start_of_auto > (m_autonomous_length*1000)) {
                    m_state = Teleop;
                }
#endif
#ifdef AUTOMAT_ESP_32_ // WORKING HERE
#endif
            }
#ifdef AUTOMAT_VEX_
        }
#endif

        if (m_state != m_old_state) {
            m_had_state_chage = true;

            switch (m_old_state) {
                case Disabled:
                    disabledExit(); // User-made
                    break;
                case Autonomous:
                    autonomousExit(); // User-made
                    break;
                case Teleop:
                    teleopExit(); // User-made
                    break;
            }
        }
    };

#ifdef AUTOMAT_VEX_
    void TimedRobot::setUsesCompetition(bool uses_competition) {
        m_uses_vex_competition = uses_competition;
    };
#endif
    void TimedRobot::startLoop() {
#ifdef AUTOMAT_VEX_
        if (!m_uses_vex_competition) {
#endif
            for (Joystick* joystick : m_joysticks) {
                joystick->bindAutoTrigger(AButton, ButtonPressed);
            }
#ifdef AUTOMAT_VEX_
        }
#endif

        robotInternal();
    };

    void TimedRobot::robotInternal() { // bool is_original
        while (true) {
            // if (m_reseting_state_loop && !is_original) {
            //     return; // Kill the loop if it has been replaced by a new one
            // }

            pollState(); // FANCY LOOP

            // Experimental setup for approaching setInterval more closely, also somewhat dangerous
            // Replaces code at bottom
            // m_reseting_state_loop = false; // Up here is allowed because of wait in handleState()
            // vex::wait(m_frame_delay, vex::msec); // Wait for prior frame to finish
            // robotInternal(false); // Start timer for next frame

            robotPeriodic(); // User-made
            switch (m_state) {
                case Disabled:
                    disabledInternal();
                    break;
                case Autonomous:
                    autonomousInternal();
                    break;
                case Teleop:
                    teleopInternal();
                    break;
            }
            if (m_state != Disabled) {
                pollJoystickEvents();

                runDefaultCommands();

                commandScheduler();
            }

#ifdef AUTOMAT_VEX_
            vex::wait(m_frame_delay, vex::msec); // Some strange VEX wizardy that is very suspicious
#endif
#ifdef AUTOMAT_ESP32_
#endif
            // if (m_reseting_state_loop) {
            //     m_reseting_state_loop = false; // Prevent this loop from killing itself AFTER the wait so that other are cleared
            // }
            // robotInternal(false);
        }
    };
    void TimedRobot::disabledInternal() {
        if (m_had_state_chage) {
            clearCommands();
            disabledInit(); // User-made
        }

        disabledPeriodic(); // User-made
    };
    void TimedRobot::autonomousInternal() {
        if (m_had_state_chage) {
            clearCommands();
            runCommand(m_autonomous_command);
            autonomousInit(); // User-made function
        }

        autonomousPeriodic(); // User-made
    };
    void TimedRobot::teleopInternal() {
        if (m_had_state_chage) {
            clearCommands();
            teleopInit(); // User-made
        }

        teleopPeriodic(); // User-made
    };

    void TimedRobot::commandScheduler() {
        for (int i = 0; i < static_cast<int>(m_commands.size()); i++) {
            bool finished = m_commands[i]->runLoop();
            if (finished) {
                delete m_commands[i];
                m_commands.erase(m_commands.begin() + i);
                i -= 1;
            }
        }
    };
    void TimedRobot::clearCommands() {
        for (Command* command : m_commands) {
            delete command; // Remember, these are pointers!
        }
        m_commands.clear();
    };
    void TimedRobot::runDefaultCommands() {
        for (Subsystem* subsystem : m_subsystems) {
            subsystem->runPeriodic();
            if (subsystem->hasDefaultCommand() && !subsystemHasCommand(subsystem)) { // Checks and runs default command
                runCommand(subsystem->getDefaultCommand());
            }
        }
    };
    void TimedRobot::pollJoystickEvents() {
        for (Joystick* joystick : m_joysticks) {
            std::vector<Command*> commands = joystick->pollEvents();
            for (Command* command : commands) {
                runCommand(command);
            }

            std::vector<int> terminations = joystick->pollEventTerminations();
            for (int termination : terminations) {
                endCommand(termination);
            }
        }
    };

    void TimedRobot::runCommand(Command* command) {
        std::vector<Subsystem*> command_subs = command->getSubsystems();
        for (Subsystem* subsystem : command_subs) { // Ensure all subsystems are registered (primarily to force good practice)
            if (!robotHasSubsystem(subsystem)) { // TODO: Error out somehow!!!!
                // printf("ERROR: TimedRobot: runCommand: Command references unregistered Subsystem");
                // return -1;
                return;
            }
        }
        for (int i = 0; i < static_cast<int>(m_commands.size()); i++) { // End all current commands using said subsystems
            if (command->hasMatchingSubsystems(m_commands[i])) {
                delete m_commands[i]; // Will run ~Command, which runs end()
                m_commands.erase(m_commands.begin() + i);
                i -= 1;
            }
        }

        // Set Id for later reference
        // int command_id = m_command_id_counter;
        // command->setId(command_id);
        // m_command_id_counter += 1;

        m_commands.push_back(command);
        // return command_id;
    };
    void TimedRobot::registerSubsystem(Subsystem* subsystem) {
        if (!robotHasSubsystem(subsystem)) {
            m_subsystems.push_back(subsystem); // To ensure no duplicates
        }
    };
    void TimedRobot::addJoystick(Joystick* joystick) {
        if (!robotHasJoystick(joystick)) {
            m_joysticks.push_back(joystick); // To ensure no duplicates
        }
    };

    bool TimedRobot::robotHasSubsystem(Subsystem* subsystem) {
        for (Subsystem* search : m_subsystems) {
            if (search == subsystem) {
                return true;
            }
        }
        return false;
    };
    bool TimedRobot::subsystemHasCommand(Subsystem* subsystem) {
        for (Command* command : m_commands) {
            if (command->hasSubsystem(subsystem)) {
                return true;
            }
        }
        return false;
    };
    bool TimedRobot::robotHasJoystick(Joystick* joystick) {
        for (Joystick* search : m_joysticks) {
            if (search == joystick) {
                return true;
            }
        }
        return false;
    };

    void TimedRobot::endCommand(int command_id) {
        for (int i = 0; i < static_cast<int>(m_commands.size()); i++) {
            if (m_commands[i]->getId() == command_id) {
                delete m_commands[i]; // Will run ~Command, which runs end()
                m_commands.erase(m_commands.begin() + i);
                i -= 1;
            }
        }
    };

    void TimedRobot::setAutonomousCommand(Command* command) {
        m_autonomous_command = command;
    };


    void TimedRobot::robotPeriodic() {};
    void TimedRobot::disabledInit() {};
    void TimedRobot::disabledPeriodic() {};
    void TimedRobot::disabledExit() {};
    void TimedRobot::autonomousInit() {};
    void TimedRobot::autonomousPeriodic() {};
    void TimedRobot::autonomousExit() {};
    void TimedRobot::teleopInit() {};
    void TimedRobot::teleopPeriodic() {};
    void TimedRobot::teleopExit() {};
}