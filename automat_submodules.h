
#ifndef AUTOMAT_SUBMODULES_
#define AUTOMAT_SUBMODULES_

// #define ATMT_SUBMODULE_COMMAND_BASED_ // Enables/Disables TimedRobot, Subsystem, Command, etc.
#define ATMT_SUBMODULE_SERIAL_ // Enables/Disables SerialReader
#define ATMT_SUBMODULE_HTTP_SERVER_ // Enables/Disables HTTPServer, RobotDashboard, CameraServer

// Server Additional Tools
// #define ATMT_SUBMODULE_HTTP_SERVER_JSON_PARSING_ // Enables/Disables Parsing POST application/json type (requires bblanchon/ArduinoJson in Arduino framework)
#define ATMT_SUBMODULE_HTTP_SERVER_JPEG_STREAMING_ // Enables/Disables HTMLPage_Dynamic_JPEGStreamer

// Utilities
// #define ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_ // Enables/Disables PID_Controller
// #define ATMT_SUBMODULE_UTILITIES_VECTOR_ // Enables/Disables Vector2, Vector3
#define ATMT_SUBMODULE_UTILITIES_THREADSAFE_BUFFER_ // Enables/Disables ThreadsafeBuffer
#define ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_ // Enables/Disables KinematicPredictor

#endif