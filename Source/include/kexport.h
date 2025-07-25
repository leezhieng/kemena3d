// Export macro
#pragma once

// Detect Windows
#if defined(_WIN32) || defined(__CYGWIN__)
  #if defined(KEMENA3D_SHARED)
    #define KEMENA3D_API __declspec(dllexport)
  #elif defined(KEMENA3D_IMPORT)
    #define KEMENA3D_API __declspec(dllimport)
  #else
    #define KEMENA3D_API
  #endif
#else
  #if defined(KEMENA3D_SHARED) || defined(KEMENA3D_IMPORT)
    #define KEMENA3D_API __attribute__((visibility("default")))
  #else
    #define KEMENA3D_API
  #endif
#endif