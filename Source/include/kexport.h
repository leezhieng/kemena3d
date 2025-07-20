// Export macro
#ifdef _WIN32
  #ifdef KEMENA3D_STATIC
    #define KEMENA3D_API
  #elif defined(KEMENA3D_EXPORTS)
    #define KEMENA3D_API __declspec(dllexport)
  #else
    #define KEMENA3D_API __declspec(dllimport)
  #endif
#else
  #define KEMENA3D_API
#endif