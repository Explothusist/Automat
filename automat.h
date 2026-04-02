
#ifndef AUTOMAT_
#define AUTOMAT_

#include "automat_platform.h" // Determines whether VEX-specific or Esp32-specific code is used
#include "automat_submodules.h" // Determines whether certain extra submodules of the library are compiled

#include "utils.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    #include "command_based/Command.h"
    #include "command_based/Trigger.h"
    #include "command_based/Joystick.h"        // Uses atmt_platform
    #include "command_based/Subsystem.h"
    #include "command_based/EventHandler.h"
    #include "command_based/TimedRobot.h"           // Uses atmt_platform

    #include "command_based/command_helpers/EmptyCommand.h"
    #include "command_based/command_helpers/InstantCommand.h"
    #include "command_based/command_helpers/ParallelCommandGroup.h"
    #include "command_based/command_helpers/ParallelRaceCommandGroup.h"
    #include "command_based/command_helpers/SequentialCommandGroup.h"
    #include "command_based/command_helpers/WaitCommand.h"

    #include "command_based/heartbeat_helpers/Heartbeat.h"
    #include "command_based/heartbeat_helpers/Heartbeat_StateMatcher.h"
    #include "command_based/heartbeat_helpers/HeartbeatMaker.h"
    #include "command_based/heartbeat_helpers/HeartbeatMaker_StateMatcher.h"
#endif

#ifdef ATMT_SUBMODULE_SERIAL_
    #include "serial/SerialReader.h"
#endif

#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    #include "http_server/HTMLPageInternals.h"
    #include "http_server/HTMLPage.h"
    #include "http_server/HTTPServer.h"

    #ifdef ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_
        #include "http_server/CameraStreamingServer.h"
    #endif
    #ifdef ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_
        #include "http_server/dashboard_tiles/DashboardTile.h"
        #include "http_server/RobotDashboardServer.h"
    #endif
#endif

#ifdef ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_
    #include "utilities/PID_Controller.h"
#endif
#ifdef ATMT_SUBMODULE_UTILITIES_VECTOR_
    #include "utilities/Vector2.h"
    #include "utilities/Vector3.h"
#endif
#ifdef ATMT_SUBMODULE_UTILITIES_THREADSAFE_BUFFER_
    #include "utilities/ThreadsafeBuffer.h"
#endif
#ifdef ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_
    #include "utilities/KinematicPredictor.h"
#endif


#endif