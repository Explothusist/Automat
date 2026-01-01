
#include "automat_platform.h" // Determines whether VEX-specific or Esp32-specific code is used

#include "utils.h"
#include "Command.h"
#include "InstantCommand.h"
#include "Trigger.h"
#include "Joystick.h"        // Uses atmt_platform
#include "Subsystem.h"
#include "TimedRobot.h"           // Uses atmt_platform