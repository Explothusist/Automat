
#ifndef AUTOMAT_PLATFORM_
#define AUTOMAT_PLATFORM_

// // #define AUTOMAT_ESP32_ // Select which platform here
// // #define AUTOMAT_VEX_ 

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
    #define AUTOMAT_ESP32_
#elif defined(__VEX__)
    #define AUTOMAT_VEX_
#else
    #error "Automat: Unknown platform"
#endif

#if defined(AUTOMAT_ESP32_) + defined(AUTOMAT_VEX_) != 1
    #error "Automat: Exactly one AUTOMAT platform must be defined"
#endif

#endif