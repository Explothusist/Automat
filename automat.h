
#ifndef AUTOMAT_
#define AUTOMAT_

#include "automat_platform.h" // Determines whether VEX-specific or Esp32-specific code is used
#include "automat_submodules.h" // Determines whether certain extra submodules of the library are compiled

#include "utils.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "command_based/Command.h"
// #include "InstantCommand.h"
#include "command_based/EmptyCommand.h"
#include "command_based/SequentialCommandGroup.h"
#include "command_based/Trigger.h"
#include "command_based/Joystick.h"        // Uses atmt_platform
#include "command_based/Subsystem.h"
#include "command_based/TimedRobot.h"           // Uses atmt_platform
#endif

#ifdef ATMT_SUBMODULE_SERIAL_
#include "serial/SerialReader.h"
#endif

#ifdef ATMT_SUBMODULE_HTTP_SERVER_
#endif

#ifdef ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_
#include "utilities/PID_Controller.h"
#endif
#ifdef ATMT_SUBMODULE_UTILITIES_VECTOR_
#include "utilities/Vector2.h"
#include "utilities/Vector3.h"
#endif

#endif