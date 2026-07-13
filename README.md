# Automat

Automat is a robot control library designed to be a simple and powerful standalone tool for rapidly developing robust robot codebases. The library's core is a command based framework, but the library also includes a number of utility features and built-in communication protocols.

Automat's command_based module, the original core of the library, is based on the FRC (FIRST Robotics Competition) robot control library WPILib, an open source library built and maintained by the Worchester Polytechnic Institute. WPILib's most outstanding benefit is its simple and efficient command-based robot framework dividing the robot into commands and subsystems. Automat provides most of the basic functionality of WPILib to VEX, Esp32, and Arduino environments.

For detailed documentation, see `documentation.md`.

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
 - **Other Utilies** (See `documentation.md`)

## Examples and Starter Code

Automat-Example-Code: https://github.com/Explothusist/Automat-Example-Code

## WPILib and Command-Based Architectures

WPILib: https://docs.wpilib.org/en/stable/docs/software/what-is-wpilib.html

Command-based architecture: https://docs.wpilib.org/en/stable/docs/software/commandbased/what-is-command-based.html