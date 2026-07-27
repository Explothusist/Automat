# command_based/TimedRobot

File contents:

- Class `atmt::TimedRobot`

## Class `atmt::TimedRobot`

    class TimedRobot {
        public:
            TimedRobot();
            TimedRobot(double autonomous_length);
            ~TimedRobot();
            
            virtual void environmentInit(); // User-made

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

            void registerSubsystem(Subsystem* subsystem);
            void registerJoystick(Joystick* joystick);
    #ifdef ATMT_SUBMODULE_SERIAL_
            void registerSerialReader(SerialReader* serial);
    #endif
    #ifdef ATMT_SUBMODULE_ESPNOW_
            void registerESPNowHandler(ESPNowHandler* espnow);
    #endif
    #ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_
            void registerRobotDashboard(RobotDashboardServer* server);
    #endif
            void registerHeartbeat(Heartbeat* heartbeat);
            void registerHeartbeatMaker(HeartbeatMaker* heartbeat_maker);

            void setAutonomousCommand(Command* command);
            void setAutonomousCommandGetter(std::function<Command*(int, void*)> command_getter, std::function<int(void*)> routine_getter, void* arg);

            void startLoop();

    #ifdef AUTOMAT_VEX_
            void setUsesCompetition(bool uses_competition); // VEX-specific
    #endif
    #ifdef AUTOMAT_ESP32_
            void runLoop(); // Esp32-specific
    #endif
    };

This is a base class which handles the organization, events, commands, and main loop of the command based system. Often, a supporting `RobotContainer` is used to contain the `Joystick` and `Subsystem` instances, which solves scope problems (see examples and starter code in `documentation.md` for more details).

The `TimedRobot` class also tracks state between Disabled, Autonomous, and Teleop modes, with some triggers only firing in certain modes. `Joystick` and `Joystick`-likes can have state changes bound to a `Trigger`. Alternately, the handling of robot states can be delegated to `vex::competition` on VEX platforms to allow the library to be used in competition, but this feature has not been tested at an actual VEX competition.

The `*Init()` virtual methods will be run once when `TimedRobot` enters the given mode. The `*Periodic()` virtual methods will be run every cycle of the main loop while `TimedRobot` is in the given mode. The `*Exit()` virtual methods will be run once when `TimedRobot` leaves the given mode. The `robot*()` virtual methods are run when the main loop begins, every loop, and when the main loop ends, respectively.

Usage example (taken from `TimedRobot-Kampf-Starter-Esp32/src/Robot.h` in [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)):

    class Robot : public atmt::TimedRobot {
        public:
            Robot();
            ~Robot();

            void environmentInit() override;

            void robotInit() override;
            void robotPeriodic() override;
            void robotExit() override;
            
            void disabledInit() override;
            void disabledPeriodic() override;
            void disabledExit() override;
            
            void autonomousInit() override;
            void autonomousPeriodic() override;
            void autonomousExit() override;
            
            void teleopInit() override;
            void teleopPeriodic() override;
            void teleopExit() override;

        private:
            RobotContainer* m_bot_cont;
    };

With the corresponding source code (taken from `TimedRobot-Kampf-Starter-Esp32/src/Robot.cpp` in [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)):

    Robot::Robot():
        atmt::TimedRobot(consts::robot::AutonomousLength), // Set Autonomous length here
        m_bot_cont{ new RobotContainer() } // There are important architectural reasons for RobotContainer, but they are too long to explain here
    {

    };
    Robot::~Robot() {
        delete m_bot_cont;
        m_bot_cont = nullptr;
    };

    void Robot::environmentInit() { // Runs once before anything else
        atmt::platform_println("Environment Init!");
        
        // Register all Subsystems
        // All Subsystems MUST be registered for periodics to run and for commands on those subsystems to run!
        registerSubsystem(m_bot_cont->m_receiver);
        registerSubsystem(m_bot_cont->m_drivetrain);
        registerSubsystem(m_bot_cont->m_weapon);

        // Add all Joysticks
        // All Joysticks MUST be registered for events to trigger!
        registerJoystick(m_bot_cont->m_joystick);

        // Add SerialReaders/ESPNowHandlers/HTTPServers (if applicable)
        // Add Heartbeats/HeartbeatMakers (if applicable)
        registerESPNowHandler(m_bot_cont->m_espnow_handler);
        
        // Set up Autonomous (Do not edit unless you understand what you are doing)
        setAutonomousCommandGetter(RobotContainer::getAutonomousCommand, RobotContainer::getWhichAutonomousRoutine, m_bot_cont);

        // Configure bindings (Do not edit unless you understand what you are doing)
        m_bot_cont->configure_auto_bindings();
        m_bot_cont->configure_bindings();
    };

    void Robot::robotInit() { // Runs once after the command-based environment has been set up
        atmt::platform_print("Robot Init!           ");
    };
    void Robot::robotPeriodic() { // Runs continuously regardless of mode
        // atmt::platform_print("Robot Periodic...     ");
    };
    void Robot::robotExit() { // Runs once when the the robot is reset or powered off
        atmt::platform_print("Robot Exit            ");
    };

    void Robot::disabledInit() { // Runs once whenever the robot enters the Disabled mode
        atmt::platform_println("Disabled Init!");
    };
    void Robot::disabledPeriodic() { // Runs continuously whenever the robot is in the Disabled mode
        // atmt::platform_println("Disabled Periodic...");
    };
    void Robot::disabledExit() { // Runs once whenever the robot leaves the Disabled mode
        atmt::platform_println("Disabled Exit");
    };

    void Robot::autonomousInit() { // Runs once whenever the robot enters the Autonomous mode
        atmt::platform_println("Autonomous Init!");
    };
    void Robot::autonomousPeriodic() { // Runs continuously whenever the robot is in the Autonomous mode
        // atmt::platform_println("Autonomous Periodic...");
    };
    void Robot::autonomousExit() { // Runs once whenever the robot leaves the Autonomous mode
        atmt::platform_println("Autonomous Exit");
    };

    void Robot::teleopInit() { // Runs once whenever the robot enters the Teleop mode
        atmt::platform_println("Teleop Init!");
    };
    void Robot::teleopPeriodic() { // Runs continuously whenever the robot is in the Teleop mode
        // atmt::platform_println("Teleop Periodic...");
    };
    void Robot::teleopExit() { // Runs once whenever the robot leaves the Teleop mode
        atmt::platform_println("Teleop Exit");
    };

And the corresponding main file (taken from `TimedRobot-Kampf-Starter-Esp32/src/main.cpp` in [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)):

    #include <Arduino.h>
    #include "Robot.h"

    Robot m_robot{ Robot() }; // Bindings and init configured here

    void setup() { // NO OTHER CODE GOES IN THIS FILE! Use Robot.cpp instead
        Serial.begin(115200);
        atmt::platform_println("Starting");

        m_robot.startLoop();
        
    };

    void loop() {

        m_robot.runLoop();

    };

### Constructor

`TimedRobot()`  
`TimedRobot(int autonomous_length)`

`autonomous_length` defines the number of seconds to spend in the Autonomous mode. Overriden by `vex::competition` if `setUsesCompetition(true)` is called.

### Virtual Methods

The `*Init()` virtual methods will be run once when `TimedRobot` enters the given mode. The `*Periodic()` virtual methods will be run every cycle of the main loop while `TimedRobot` is in the given mode. The `*Exit()` virtual methods will be run once when `TimedRobot` leaves the given mode. The `robot*()` virtual methods are run when the main loop begins, every loop, and when the main loop ends, respectively.

### `register*()` Methods

The `register*()` methods register a pointer of the indicated class into the command-based system, which is required for events, triggers, and periodics to run. If a class has a specific `register*()` method, like `registerSerialReader()` for `SerialReader`, use that method instead of `registerSubsystem()`.

### `setAutonomousCommand()` and `setAutonomousCommandGetter()` Methods

These methods control the command run when entering Autonomous, if any. Combined with command helpers like `SequentialCommandGroup`, this creates an extremely simple way to implement autonomous functionality.

# To staticly set the Autonomous Command, use `setAutonomousCommand()`. See the following example (taken from `***WORKING HERE***)



#### startLoop

`void TimedRobot::startLoop()`

This method starts the main robot loop and event handlers. Attempts to trigger Autonomous made before `startLoop()` will not register.

**Important:** To ensure consistent robot behavior, run only *after* all robot initialization (`TimedRobot.registerSubsystem()`, `TimedRobot.registerJoystick()`, `Joystick.bindKey()`, etc.) is complete.

#### setUsesCompetition (VEX-specific)

`void TimedRobot::setUsesCompetition(bool uses_competition)`

This method sets whether the robot reads `vex::competition` for robot state or handles its own state. If it handles its own state, Autonomous is triggered via pressing the A button on any connected joystick, and Teleop begins once `autonomous_length` seconds have elapsed (set by `TimedRobot(int autonomous_length)`).