#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#include "TimedRobot.h"

#include "Subsystem.h"
#include "Command.h"
#include "Joystick.h"
#include "EventHandler.h"
#ifdef ATMT_SUBMODULE_SERIAL_
#include "../serial/SerialReader.h"
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_
#include "../http_server/RobotDashboardServer.h"
#endif
#include "heartbeat_helpers/Heartbeat.h"
#include "heartbeat_helpers/HeartbeatMaker.h"

namespace atmt {

    TimedRobot::TimedRobot():
        TimedRobot(0.0)
    {

    };
    TimedRobot::TimedRobot(double autonomous_length):
        m_event_handler{ new EventHandler() },
        m_subsystems{ },
        m_commands{ },
        m_heartbeats{ },
        m_heartbeat_makers{ },
        m_autonomous_command{ nullptr },
        m_auto_command_getter_set{ false },
        m_auto_getter{ },
        m_routine_getter{ },
        m_auto_getter_arg{ nullptr },
#ifdef AUTOMAT_VEX_
        // m_brain{ vex::brain() },
        m_uses_vex_competition{ false },
#endif
        m_state{ Disabled },
        m_old_state{ Disabled },
        m_reseting_state_loop{ false },
        m_had_state_change{ false },
        m_frame_delay{ 20 },
        m_has_init{ false },
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
        delete m_autonomous_command;
        m_autonomous_command = nullptr;
    };

    void TimedRobot::pollState() {
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
            if (m_state == Disabled) {
                bool triggered = false;
                triggered = m_event_handler->pollAutonomousTriggers();

                if (triggered) {
                    m_state = Autonomous;
                    
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
        for (Heartbeat* heartbeat : m_heartbeats) {
            if (heartbeat->isHeartbeatLost()) {
                m_state = Disabled;
            }else if (heartbeat->isStateControlling()) {
                m_state = heartbeat->getState();
            }
        }

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
        if (!m_has_init) {
            startLoop();
            // m_has_init = true;
        }else {
            robotInternal();
        }
    };
#endif
    void TimedRobot::startLoop() {
        if (!m_has_init) {
            m_has_init = true;
            environmentInit(); // User-made
            for (Subsystem* subsystem : m_subsystems) {
                subsystem->init(); // User-made
            }
            SetReadEvents(true);
            robotInit(); // User-made
        }

        robotInternal();
    };

    void TimedRobot::robotInternal() {
#ifdef AUTOMAT_VEX_
        while (true) {
#endif
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

            pollState();

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

            // Correction: Default Commands run during Autonomous
            for (Subsystem* subsystem : m_subsystems) {
                // Correction: Periodics run during all modes
                subsystem->runPeriodic(m_state); // Correction: Moded periodics
                if (m_state != Disabled) {
                    if (m_state != Disabled && subsystem->hasDefaultCommand() && !subsystemHasCommand(subsystem)) { // Checks and runs default command
                        runCommand(subsystem->getDefaultCommand());
                    }
                }
            }
            for (HeartbeatMaker* heartbeat_maker : m_heartbeat_makers) {
                heartbeat_maker->runLoop(m_state);
            }
            if (m_state != Disabled) {
                
                // if (m_state != Autonomous) { // Note: Default Commands do not run during Autonomous
                // }

                pollEvents(); // Auto starting events are polled in pollState() via another method

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
            Command* auto_command = getAutoCommand();
            if (auto_command != nullptr) {
                runCommand(auto_command);
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
                vectorDeleteUnordered(m_commands, i);
                // m_commands.erase(m_commands.begin() + i);
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
    void TimedRobot::pollEvents() {
#ifdef AUTOMAT_ESP32_
        for (Joystick* joystick : m_joysticks) {
            joystick->runPollState(); // For PollingMode = Continuous
        }
#endif

        std::vector<Command*> commands = m_event_handler->pollEvents();
        for (Command* command : commands) {
            runCommand(command);
        }

        std::vector<int> terminations = m_event_handler->pollEventTerminations();
        for (int termination : terminations) {
            endCommand(termination);
        }
    };

    void TimedRobot::runCommand(Command* command) {
        std::vector<Subsystem*> command_subs = command->getSubsystems();
        for (Subsystem* subsystem : command_subs) { // Ensure all subsystems are registered (primarily to force good practice)
            if (!robotHasSubsystem(subsystem)) { // TODO: Error out somehow!!!!
                // platform_println("ERROR: TimedRobot: runCommand: Command references unregistered Subsystem");
                return;
            }
        }
        for (int i = 0; i < static_cast<int>(m_commands.size()); i++) { // End all current commands using said subsystems
            if (command->hasMatchingSubsystems(m_commands[i])) {
                delete m_commands[i]; // Will run ~Command, which runs end()
                vectorDeleteUnordered(m_commands, i);
                // m_commands.erase(m_commands.begin() + i);
                i -= 1;
            }
        }

        m_commands.push_back(command);
    };
    void TimedRobot::registerSubsystem(Subsystem* subsystem) {
        if (!robotHasSubsystem(subsystem)) {
            m_subsystems.push_back(subsystem); // To ensure no duplicates
        }
    };
    void TimedRobot::addJoystick(Joystick* joystick) {
        if (!robotHasSubsystem(joystick)) {
            joystick->internal_init(&m_state, m_event_handler);
            m_subsystems.push_back(joystick); // To ensure no duplicates
        }
    };
#ifdef ATMT_SUBMODULE_SERIAL_
    void TimedRobot::addSerialReader(SerialReader* serial) {
        if (!robotHasSubsystem(serial)) {
            serial->internal_init(&m_state, m_event_handler);
            m_subsystems.push_back(serial); // To ensure no duplicates
        }
    };
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_
    void TimedRobot::addRobotDashboard(RobotDashboardServer* server) {
        if (!robotHasSubsystem(server)) {
            // Probably will do something more here eventually
            m_subsystems.push_back(server); // To ensure no duplicates
        }
    };
#endif
    void TimedRobot::addHeartbeat(Heartbeat* heartbeat) {
        if(heartbeat == nullptr){
            platform_println("ERROR: TimedRobot: addHeartbeat: heartbeat is nullptr");
            return;

        }
        
        if (!robotHasHeartbeat(heartbeat)) {
            m_heartbeats.push_back(heartbeat);
        }
    };
    void TimedRobot::addHeartbeatMaker(HeartbeatMaker* heartbeat_maker) {
        if(heartbeat_maker == nullptr){
            platform_println("ERROR: TimedRobot: addHeartbeatMaker: heartbeat_maker is nullptr");
            return;
        } 
        
        if (!robotHasHeartbeatMaker(heartbeat_maker)) {
            // if (heartbeat_maker->isStateControlling()) {
            //     heartbeat_maker->stateControllingInit(&m_state);
            // }
            m_heartbeat_makers.push_back(heartbeat_maker);
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
    bool TimedRobot::robotHasHeartbeat(Heartbeat* heartbeat) {
        for (Heartbeat* search : m_heartbeats) {
            if (search == heartbeat) {
                return true;
            }
        }
        return false;
    };
    bool TimedRobot::robotHasHeartbeatMaker(HeartbeatMaker* heartbeat_maker) {
        for (HeartbeatMaker* search : m_heartbeat_makers) {
            if (search == heartbeat_maker) {
                return true;
            }
        }
        return false;
    };

    void TimedRobot::endCommand(int command_id) {
        for (int i = 0; i < static_cast<int>(m_commands.size()); i++) {
            if (m_commands[i]->getId() == command_id) {
                delete m_commands[i]; // Will run ~Command, which runs end()
                vectorDeleteUnordered(m_commands, i);
                // m_commands.erase(m_commands.begin() + i);
                i -= 1;
            }
        }
    };

    void TimedRobot::setAutonomousCommand(Command* command) {
        delete m_autonomous_command;
        m_autonomous_command = command;
        m_auto_command_getter_set = false;
    };
    void TimedRobot::setAutonomousCommandGetter(std::function<Command*(int, void*)> command_getter, std::function<int(void*)> routine_getter, void* arg) {
        m_auto_getter = command_getter;
        m_routine_getter = routine_getter;
        m_auto_getter_arg = arg;
        m_auto_command_getter_set = true;
    };
    Command* TimedRobot::getAutoCommand() {
        if (m_auto_command_getter_set) {
            return m_auto_getter(m_routine_getter(m_auto_getter_arg), m_auto_getter_arg);
        }else {
            return m_autonomous_command;
        }
    };


    void TimedRobot::environmentInit() {};
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
};

#else
// #error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use TimedRobot"
#endif