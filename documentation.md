# Documentation Structure

Under `documentation/` there are files for each class, organized in the same file structure as the the library itself (i.e. the documentation for `command_based/TimedRobot.h` is found under `documentation/command_based/TimedRobot.md`).

## Supported Environments

The code automatically detects the environment, but this feature is not exhaustively tested. The auto detection is tested in the environments listed below. If detection errors occur, manually specify environment in `automat_platform.h`.

### **VEX V5**

Tested in VEX V5 using the VSCode extension. VEXcode is not supported because it does not, to my knowledge, allow multiple source files. May work for other VEX environments, but not explicitly tested or supported.

The ESPNow and HTTP Server submodules is not available on VEX platforms. On VEX platforms, the Serial submodule uses RS485 via the Smart Cable ports (RJ12 with straight-through wiring). RS485 to TTL converters are available to interface with Automat devices on other platforms.

### **ESPIDF** 

Tested in PlatformIO VSCode extension using the ESPIDF framework, run on an ESP32-S3.

The HTTP Server submodule natively supports asynchronous functionality in this framework. The Serial submodule uses UART in this framework.

### **Arduino**

Tested in PlatformIO VSCode extension using the Arduino framework, run on an ESP32-S3.

The HTTP Server submodule requires esphome/ESPAsyncWebServer-esphome and esphome/AsyncTCP-esphome for asynchronous functionality in the Arduino framework, as asynchronous HTTP is not supported by the baseline interface in the Arduino framework. The Serial submodule uses UART in this framework. ESPNow is only available on ESP32 devices.

## Submodules

### **Command-based**

This is the part of the library derived from WPILib. This submodule creates a task handling system based around Commands and Subsystems. A Subsystem is a discrete set of motors or functionality of the robot. A Command is a single specific action with a defined start and end. Commands are set to require specific Subsystems and have exclusive control over those Subsystem while being executed. Joysticks provide event Triggers to bind Commands to, causing them to run on user input. A Robot Container encapsulates the Subsystems, while a Timed Robot class encapsulates the Robot Container and manages Subsystems and Command scheduling. A master Robot State allows for Disabled, Autonomous, and Teleoperated modes for safety. Timed Robot, Subsystems, and Commands all contain periodic functions which run on loop in designated Robot States and trigger functions which run when certain Robot State events happen.

### **Packet Handling**

This submodule is used by the communication protocol submodules, like ESPNow and Serial, both to encode and parse packets and to integrate with the Command-based system with Joystick-like Triggers for Commands. Automat uses a custom packet format, which is used universally across all transmissions. Users need no knowledge of the underlying format, as a all encoding and parsing is handled by the library.

These classes are exposed so that developers may build custom tools for other protocols if needed.

### **ESPNow**

This submodule is only available on ESP32 devices. This lightweight protocol allows such devices to communicate directly without need for a WiFi network or router. The submodule supports single-cast and multi-cast and allows for multiple endpoints from a single MAC address via the Packet Handling address system, with each handler instance able to be tied to send to and listen from a single MAC address. Uses the Packet Handling submodule to encode and parse packets and interface with the Command-based submodule with Joystick-like Triggers.

### **Serial**

This submodule uses RS485 on VEX platforms and UART on all other platforms. This widely available protocol allows communication with other Automat devices with different platforms. The submodule supports single-cast and multi-case and allows for multiple endpoints on a single pin and a number of devices on a single bus via the Packet Handling address system. Uses the Packet Handling submodule to encode and parse packets and interface with the Command-based submodule with Joystick-like Triggers.

### **HTTP Server** (Arduino/ESPIDF only, requires a plugin for async functionality on Arduino)

This submodule requires either ESPIDF or Arduino environments. The HTTP Server submodule requires esphome/ESPAsyncWebServer-esphome and esphome/AsyncTCP-esphome for asynchronous functionality in the Arduino framework, as asynchronous HTTP is not supported by the baseline interface in the Arduino framework. This submodule allows for the hosting of simple web servers for robot connection, diagnostics, camera streaming, etc.

### **Other Utilies** (Arduino/ESPIDF only, requires a plugin for async functionality on Arduino)

Automat also includes a number of other utilities, including:

**KinematicPredictor**: Wraps a simple algorithm to predict future movement of an object in 1D based on past movement. Use multiple instances for 2D, 3D predictions. Originally developed for ROI (Region of Interest) tracking for AprilTag tracking cameras

**PID_Controller**: Provides basic PID loop calculation for environments without other PID tools.

# **SlewRateLimiter**: ***WORKING HERE AND EXAMPLES IN OTHERS***

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
