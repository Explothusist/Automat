# Documentation Structure

Under `documentation/` there are files for each class, organized in the same file structure as the the library itself (i.e. the documentation for `command_based/TimedRobot.h` is found under `documentation/command_based/TimedRobot.md`).

## Supported Environments

 - **VEX V5** (some functionality limited, not tested on other VEX platforms)
 - **ESPIDF** 
 - **Arduino** (so far, only tested on ESP32 using Arduino framework)

The code automatically detects the environment, but this feature is not exhaustively tested. If detection errors occur, manually specify environment in `automat_platform.h`.

## Submodules

 - **Command-based** (derived from WPILib)
 - **Packet Handling** (Used by ESPNow, Serial, available for custom protocols)
 - **ESPNow** (Arduino/ESPIDF on ESP32 only)
 - **Serial** (UART on Arduino/ESPIDF, RS485 on VEX V5)
 - **HTTP Server** (Arduino/ESPIDF only, requires a plugin for async functionality on Arduino)

## Examples and Starter Code

Automat-Example-Code: https://github.com/Explothusist/Automat-Example-Code

## WPILib and Command-Based Architectures

WPILib: https://docs.wpilib.org/en/stable/docs/software/what-is-wpilib.html

Command-based architecture: https://docs.wpilib.org/en/stable/docs/software/commandbased/what-is-command-based.html

## Main Differences from WPILib ***WORKING HERE***

The functionality of `CommandScheduler` are combined into the `TimedRobot` class (i.e. there is no need to directly call the `CommandScheduler`). The  `RobotContainer` file is more or less unchanged. Read the `TimedRobot` class carefully, as Autonomous and Teleop are handled differently and setup is somewhat different.

The `Joystick` class is heavily modified, as is the `Trigger` class. The `Trigger` class itself is only used internally and `Joystick.bindKey()` is used instead. These changes are in part because of how different the `Joystick` class is on the different platform.

Additional Serial and HTTP Server functionality has been added beyond WPILib's builtin functionality, as have a number of other helper classes in the utilities folder. See the documentation for more details. 

## General Structure

The general structure for an Automat project looks something like the following:

+ src/
    + commands/
        + DriveCommand.h
        + DriveCommand.cpp
        + LiftCommand.h
        + LiftCommand.cpp
        + etc.
    + subsystems/
        + Drivebase.h
        + Drivebase.cpp
        + Arm.h
        + Arm.cpp
        + etc.
    + Constants.h
    + RobotContainer.h
    + RobotContainer.cpp
    + Robot.h
    + Robot.cpp
    + main.cpp

The `src/commands/` folder holds various custom command classes, all derived from `Command`. Each command performs a specific action, like lifting the arm or driving in a direction, using the methods defined by the subsystems in `src/subsystems/`. Some code will be in these files, but mostly it will be calling the various subsystem methods at the proper times.  
The `src/subsystems/` folder holds custom subsystem classes, derived from `Subsystem`. Each subsystem contains the low-level code for a motor or small collection of motors such as the drivetrain, robot arm, or claw. The subsystem has methods which are run by the commands in the `src/commands/` folder. The bulk of the code will be in these files.  
The `src/Constants.h` file is exactly what it sounds like, a central location for all of the semi-arbitrary but nonetheless vitally important numbers.  
The `src/RobotContainer.h` and `src/RobotContainer.cpp` file holds a custom `RobotContainer` class, not derived, which contains instances of the subsystems, joysticks, and commands. This setup allows those components to be referenced inside the various loop and trigger methods of the `TimedRobot` class. Very little code will be in these files, merely instantiating classes and setting keybindings.  
The `src/Robot.h` and `src/Robot.cpp` file holds a class derived from `TimedRobot`, which contains the main loops and functions of the robot. Very little code will be in this file.  
The `src/main.cpp` file instantiates the class in `src/Robot.h` and starts the robot's main loop.  

This setup is by no means binding, but seems to work well. For an actual implementation as an example, see examples under `Explothusist/Automat-Example-Code` (linked above).
