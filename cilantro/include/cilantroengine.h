#if defined _WIN32 || defined __CYGWIN__
  #ifdef CILANTRO_BUILDING_DLL
    #ifdef __GNUC__
      #define __EAPI __attribute__ ((dllexport))
    #else
      #define __EAPI __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define __EAPI __attribute__ ((dllimport))
    #else
      #define __EAPI __declspec(dllimport)
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4 || __clang__
    #define __EAPI __attribute__ ((visibility ("default")))
    #define __EAPI_LOCAL __attribute__ ((visibility ("hidden")))
  #else
    #define __EAPI
    #define __EAPI_LOCAL
  #endif
#endif
