
#include "TimedRobot.h"

#include "utils.h"
#include "Joystick.h"

namespace atmt {

    TimedRobot::TimedRobot():
        TimedRobot(0.0)
    {

    };
    TimedRobot::TimedRobot(double autonomous_length):
        m_subsystems{ },
        m_commands{ },
        m_joysticks{ },
        m_autonomous_command{ nullptr },
#ifdef AUTOMAT_VEX_
        // m_brain{ vex::brain() },
        m_uses_vex_competition{ false },
#endif
        m_state{ Disabled },
        m_old_state{ Disabled },
        m_reseting_state_loop{ false },
        m_had_state_change{ false },
        m_frame_delay{ 20 },
        m_first_auto_trigger{ true },
        m_autonomous_length{ autonomous_length },
        m_start_of_auto{ Timestamp(0) }
    {

    };
    TimedRobot::~TimedRobot() {
        robotExit(); // User-made
        
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
        m_old_state = m_state;
        m_had_state_change = false;
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
                    
                    m_start_of_auto = getSystemTime();
                }
            }else if (m_state == Autonomous) {
                Timestamp now = getSystemTime();

                if (now.getTimeDifferenceMS(m_start_of_auto) > (m_autonomous_length*1000)) {
                    m_state = Teleop;
                }
            }
#ifdef AUTOMAT_VEX_
        }
#endif

        if (m_state != m_old_state) {
            m_had_state_change = true;

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
#ifdef AUTOMAT_ESP32_
    void TimedRobot::runLoop() {
        robotInternal();
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
        for (Subsystem* subsystem : m_subsystems) {
            subsystem->init(); // User-made
        }
        SetReadJoystickEvents(true);
        robotInit(); // User-made

        robotInternal();
    };

    void TimedRobot::robotInternal() { // bool is_original
#ifdef AUTOMAT_VEX_
        while (true) {
#endif
            // if (m_reseting_state_loop && !is_original) {
            //     return; // Kill the loop if it has been replaced by a new one
            // }
            // platform_print("Robot Internal");
#ifdef AUTOMAT_VEX_
            platform_clear_screen();
            m_brain.Screen.newLine();
            m_brain.Screen.newLine();
            m_brain.Screen.newLine();
            m_brain.Screen.newLine();
            m_brain.Screen.print("State: ");
            switch (m_state) {
                case Disabled:
                    m_brain.Screen.print("Disabled");
                    break;
                case Autonomous: {
                    m_brain.Screen.print("Autonomous    ");
                    Timestamp now = getSystemTime();
                    int left = m_autonomous_length - (now.getTimeDifferenceMS(m_start_of_auto)/1000);
                    m_brain.Screen.print("%d", left);
                    break; }
                case Teleop:
                    m_brain.Screen.print("Teleop");
                    break;
            }
            m_brain.Screen.newLine();
#endif

            Timestamp loopStart{ getSystemTime() };

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
            // platform_println("Internals Done, Starting triggers/commands"); // DEBUG
            if (m_state != Disabled) {
                pollJoystickEvents(); // Auto starting events are polled in pollState() via another method

                if (m_state != Autonomous) { // Note: Default Commands do not run during Autonomous
                    runDefaultCommands();
                }

                commandScheduler();
            }
            // platform_println("Triggers/Commands done"); // DEBUG

            Timestamp loopEnd{ getSystemTime() };
            int difference{ loopStart.getTimeDifferenceMS(loopEnd) };

            if (difference < m_frame_delay) {
                systemWait(m_frame_delay - difference);
            }
#ifdef AUTOMAT_VEX_
        }
#endif
    };
    void TimedRobot::disabledInternal() {
        if (m_had_state_change) {
            clearCommands();
            disabledInit(); // User-made
        }

        disabledPeriodic(); // User-made
    };
    void TimedRobot::autonomousInternal() {
        // platform_println("autonomousInternal Begins"); // DEBUG
        if (m_had_state_change) {
            clearCommands();
            if (m_autonomous_command != nullptr) {
                runCommand(m_autonomous_command);
            }
            autonomousInit(); // User-made function
        }

        autonomousPeriodic(); // User-made
        // platform_println("autonomousInternal Ends"); // DEBUG
    };
    void TimedRobot::teleopInternal() {
        if (m_had_state_change) {
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
// #ifdef AUTOMAT_VEX_ // DEBUG
//             m_brain.Screen.print("Subsystem: %p ", subsystem);
// #endif
            subsystem->runPeriodic(); // THE PROGRAM CRASHES ON THIS LINE
            if (subsystem->hasDefaultCommand() && !subsystemHasCommand(subsystem)) { // Checks and runs default command
                runCommand(subsystem->getDefaultCommand());
            }
        }
    };
    void TimedRobot::pollJoystickEvents() {
        for (Joystick* joystick : m_joysticks) {
#ifdef AUTOMAT_ESP32_
            joystick->runPollState(); // For PollingMode = Continuous
#endif

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
        delete m_autonomous_command;
        m_autonomous_command = command;
    };


    void TimedRobot::robotInit() {};
    void TimedRobot::robotPeriodic() {};
    void TimedRobot::robotExit() {};
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