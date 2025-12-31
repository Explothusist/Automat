# Automat Documentation

Automat derives most of its interface and functionality from WPILib, the FRC (FIRST Robotics Competition) robot control library. For more information on WPILib, see the links below. For those already familiar with WPILib, this documentation highlights all differences in interface and syntax. For those unfamiliar, read WPILib's introduction to their command-based robot framework below.

To set the architecture, go to `automat_platform.h` and ensure that the appropriate `#define` statement, and only that statement, is uncommented.

Examples of the Automat library in use can be found in Explothusist/Stormbot-Codebase, linked below.

WPILib: https://docs.wpilib.org/en/stable/docs/software/what-is-wpilib.html

Command-based architecture: https://docs.wpilib.org/en/stable/docs/software/commandbased/what-is-command-based.html

Stormbot-Codebase (usage example): https://github.com/Explothusist/StormBot-Codebase

# Main Differences from WPILib

The functionality of `TimedRobot`, `RobotContainer`, and `CommandScheduler` are combined into the `TimedRobot` class (in progress: seperate `Robot` and `RobotContainer`). The `main.cpp` file looks more or less like `RobotContainer` did (for now). Read the `TimedRobot` class carefully, as Autonomous and Teleop are handled differently and setup is somewhat different.

The `Joystick` class is heavily modified, as is the `Trigger` class. The `Trigger` class itself is only used internally and `Joystick.bindKey()` is used instead. These changes are in part because of how different the `Joystick` class is on the different platform.

# Contents

+ Base classes
    + TimedRobot
    + Subsystem
    + Command
    + Joystick
+ Commands
    + SequentialCommandGroup
    + InstantCommand

# Base Classes

## TimedRobot

This class combines the functionality of `TimedRobot`, `RobotContainer`, and `CommandScheduler` (in progress: seperate `TimedRobot` and `RobotContainer`).

In the absence of the FRC FMS (Field Management System), Autonomous and Teleop are controlled either via `vex::competition` or the press of the A button on any connected controller. The first method is obviously only available in the VEX architecture, but the second is available in both architectures. To set which is used in the VEX architecture, use the VEX-specific `setUsesCompetition` method.

If the robot handles its own state, Autonomous is triggered via pressing the A button on any connected joystick, and Teleop begins once `autonomous_length` seconds have elapsed (set by `TimedRobot(int autonomous_length)`).

**Important:** After all setup has been completed, run the method `startLoop` to start the execution of the main robot loop and event handlers.

### Constructor

`TimedRobot()`  
`TimedRobot(int autonomous_length)`

`autonomous_length` defines the number of seconds to spend in the Autonomous mode. Overriden by `vex::competition` if `setUsesCompetition(true)` is called

### Methods

+ `virtual void robotPeriodic()`
+ `virtual void disabledInit()`
+ `virtual void disabledPeriodic()`
+ `virtual void disabledExit()`
+ `virtual void autonomousInit()`
+ `virtual void autonomousPeriodic()`
+ `virtual void autonomousExit()`
+ `virtual void teleopInit()`
+ `virtual void teleopPeriodic()`
+ `virtual void teleopExit()`

    These methods function like in WPILib, i.e. they are available to be overriden in the `TimedRobot` derived class specific to the project. `Init` methods run once when the robot enters the given state, `Periodic` methods run on loop while the robot is in the given state, and `Exit` methods run once when the robot leaves the given state. `robotPeriodic` always runs regardless of robot state. The equivalent of `robotInit` is the constructor and of `robotExit` is the destructor

+ `void runCommand(Command* command)`
+ `void registerSubsystem(Subsystem* subsystem)`
+ `void addJoystick(Joystick* joystick)`
+ `void setAutonomousCommand(Command* command)`
+ `void startLoop()`

    VEX specific:

+ `void setUsesCompetition(bool uses_competition)`

    Esp32 specific:

+ (In progress)

#### runCommand

`void TimedRobot::runCommand(Command* command)`

This method adds the given command to the command scheduler internal to `TimedRobot`, overriding and ending any commands which use any of the same subsystems. This method is primarily used internally, but is available if needed.

To implement keybindings, use `Joystick.bindKey()` instead, and to implement autonomous, use `TimedRobot.setAutonomousCommand()`.

Note: `runCommand` will fail to start the command if not all subsystems the command uses are registered, as it cannot ensure that there are no duplicate commands for unregistered subsystems (see `TimedRobot.registsterSubsystem()`).

**Important:** It is not recommended to keep track of the `Command*` pointer. `TimedRobot` will handle the disposal of the memory when the command is complete, which could cause reference to undefined memory.

#### registerSubsystem

`void TimedRobot::registerSubsystem(Subsystem* subsystem)`

This method adds a given subsystem to the robots list of subsystems. Registering the subsystem allows the `Subsystem.periodic()` to run and ensures that there are never two commands running on the same subsystem.

Note: If a subsystem is not registered, commands using that subsystem cannot be run.

**Important:** Keep track of the `Subsystem*` pointer and feed it into commands that use this subsystem. `~TimedRobot()` will handle the disposal of pointers to registered subsystems.

#### addJoystick

`void TimedRobot::addJoystick(Joystick* joystick)`

This method adds a given joystick to the robots list of joysticks. Registering the joystick allows `TimedRobot` to read events generated by the joystick.

**Important:** Keep track of the `Joystick*` pointer use it to create keybindings. `~TimedRobot()` will handle the disposal of pointers to registered joysticks.

#### setAutonomousCommand

`void TimedRobot::setAutonomousCommand(Command* command)`

This method sets the command to be run at the start of Autonomous. The command is run once, and automatically ended when the Autonomous period ends (specified either by `TimedRobot(int autonomous_length)` or by `vex::competition`).

Note: `runCommand`, which starts the command when autonomous begins, will fail to start the command if not all subsystems the command uses are registered, as it cannot ensure that there are no duplicate commands for unregistered subsystems (see `TimedRobot.registsterSubsystem()`).

**Important:** It is not recommended to keep track of the `Command*` pointer. `TimedRobot` will handle the disposal of the memory when the command is complete, which could cause reference to undefined memory.

#### startLoop

`void TimedRobot::startLoop()`

This method starts the main robot loop and event handlers. Attempts to trigger Autonomous made before `startLoop()` will not register.

**Important:** To ensure consistent robot behavior, run only *after* all robot initialization (`TimedRobot.registerSubsystem()`, `TimedRobot.addJoystick()`, `Joystick.bindKey()`, etc.) is complete.

#### setUsesCompetition (VEX-specific)

`void TimedRobot::setUsesCompetition(bool uses_competition)`

This method sets whether the robot reads `vex::competition` for robot state or handles its own state. If it handles its own state, Autonomous is triggered via pressing the A button on any connected joystick, and Teleop begins once `autonomous_length` seconds have elapsed (set by `TimedRobot(int autonomous_length)`).