#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_ROBOT_
#define AUTOMAT_ROBOT_

#include <vector>
#include <functional>

#include "../automat_platform.h"

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

#include "../utils.h"


namespace atmt {

    class Subsystem;
    class Command;
    class Joystick;
    class EventHandler;
#ifdef ATMT_SUBMODULE_SERIAL_
    class SerialReader;
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_
    class RobotDashboardServer;
#endif

    class TimedRobot {
        public:
            TimedRobot();
            TimedRobot(double autonomous_length);
            ~TimedRobot();

            virtual void robotInit(); // User-made
            virtual void robotPeriodic(); // User-made
            virtual void robotExit(); // User-made

            virtual void disabledInit(); // User-made
            virtual void disabledPeriodic(); // User-made
            virtual void disabledExit(); // User-made

            virtual void autonomousInit(); // User-made
            virtual void autonomousPeriodic(); // User-made
            virtual void autonomousExit(); // User-made 

            virtual void teleopInit(); // User-made
            virtual void teleopPeriodic(); // User-made
            virtual void teleopExit(); // User-made

            void pollState();

            void runCommand(Command* command);
            void registerSubsystem(Subsystem* subsystem);
            void addJoystick(Joystick* joystick);
#ifdef ATMT_SUBMODULE_SERIAL_
            void addSerialReader(SerialReader* serial);
#endif
#ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_
            void addRobotDashboard(RobotDashboardServer* server);
#endif

            void setAutonomousCommand(Command* command);
            void setAutonomousCommandGetter(std::function<Command*(int, void*)> command_getter, std::function<int(void*)> routine_getter, void* arg);
            Command* getAutoCommand();

            void endCommand(int command_id); // Use the global Id assigned by Joystick

            void startLoop();

#ifdef AUTOMAT_VEX_
            void setUsesCompetition(bool uses_competition); // VEX-specific
#endif
#ifdef AUTOMAT_ESP32_
            void runLoop(); // Esp32-specific
#endif
        private:
            EventHandler* m_event_handler;

            std::vector<Subsystem*> m_subsystems;
            std::vector<Command*> m_commands;
#ifdef AUTOMAT_ESP32_
            std::vector<Joystick*> m_joysticks;
#endif
            // std::vector<SerialReader*> m_serial_handlers;
            Command* m_autonomous_command;
            bool m_auto_command_getter_set;
            std::function<Command*(int, void*)> m_auto_getter;
            std::function<int(void*)> m_routine_getter;
            void* m_auto_getter_arg;

#ifdef AUTOMAT_VEX_
            // vex::brain m_brain;
            bool m_uses_vex_competition;
#endif

            RobotState m_state;
            RobotState m_old_state;
            bool m_reseting_state_loop;
            bool m_had_state_change;

            int m_frame_delay;

            // bool m_first_auto_trigger;
            double m_autonomous_length;
            Timestamp m_start_of_auto;

            void robotInternal();
            void disabledInternal();
            void autonomousInternal();
            void teleopInternal();

            void commandScheduler();
            void clearCommands();
            // void runDefaultCommands();
            void pollEvents();

            bool robotHasSubsystem(Subsystem* subsystem);
            // bool robotHasJoystick(Joystick* joystick);
            bool subsystemHasCommand(Subsystem* subsystem);
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use TimedRobot"
#endif