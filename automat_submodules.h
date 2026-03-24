
#ifndef AUTOMAT_SUBMODULES_
#define AUTOMAT_SUBMODULES_

#include "automat_platform.h"

// #define ATMT_SUBMODULE_COMMAND_BASED_ // Enables/Disables TimedRobot, Subsystem, Command, etc.
#define ATMT_SUBMODULE_SERIAL_ // Enables/Disables SerialReader
#define ATMT_SUBMODULE_HTTP_SERVER_ // Enables/Disables HTTPServer, RobotDashboard, CameraServer

// Utilities
// #define ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_ // Enables/Disables PID_Controller
// #define ATMT_SUBMODULE_UTILITIES_VECTOR_ // Enables/Disables Vector2, Vector3
#define ATMT_SUBMODULE_UTILITIES_THREADSAFE_BUFFER_ // Enables/Disables ThreadsafeBuffer
#define ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_ // Enables/Disables KinematicPredictor

#ifdef ATMT_SUBMODULE_HTTP_SERVER_
    // Server Platform
    #ifdef AUTOMAT_ESP32_ARDUINO_
        #define ATMT_SUBMODULE_SERVER_ARUINO_WIFI_
        // #define ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
    #else
        #define ATMT_SUBMODULE_SERVER_ESP32_HTTPD_
    #endif

    #if defined(ATMT_SUBMODULE_SERVER_ARUINO_WIFI_) + defined(ATMT_SUBMODULE_SERVER_ESP32_HTTPD_) != 1
        #error "Automat: Exactly one AUTOMAT Server platform must be defined"
    #endif

    // Server Additional Tools
    // #define ATMT_SUBMODULE_HTTP_SERVER_JSON_PARSING_ // Enables/Disables Parsing POST application/json type (requires bblanchon/ArduinoJson in Arduino framework)
    #define ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_ // Enables/Disables HTMLPage_Dynamic_JPEGStreamer
    #define ATMT_SUBMODULE_HTTP_SERVER_CAMERA_STREAMING_SERVER_ // Enables/Disables CameraStreamingServer
    // #define ATMT_SUBMODULE_HTTP_SERVER_ROBOT_DASHBOARD_SERVER_ // Enables/Disables RobotDashboardServer
#endif

#endif