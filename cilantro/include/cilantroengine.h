#include <cstddef>

// typedefs
typedef size_t handle_t;

// defines
#define CILANTRO_VBO_COUNT                  7
#define CILANTRO_UBO_COUNT                  5
#define CILANTRO_MAX_TEXTURE_UNITS          16
#define CILANTRO_MAX_POINT_LIGHTS           32
#define CILANTRO_MAX_DIRECTIONAL_LIGHTS     32
#define CILANTRO_MAX_SPOT_LIGHTS            32
#define CILANTRO_MAX_FRAMEBUFFER_TEXTURES   8
#define CILANTRO_MAX_BONES                  128
#define CILANTRO_MAX_BONE_INFLUENCES        4
#define CILANTRO_SHADOW_MAP_SIZE            4096
#define CILANTRO_MULTISAMPLE                4

// linking
#if defined _WIN32 || defined __CYGWIN__
  #if defined CILANTRO_BUILDING_DLL && !defined CILANTRO_BUILDING_PYTHON_MODULE
    #ifdef __GNUC__
      #define __EAPI __attribute__ ((dllexport))
    #else
      #define __EAPI __declspec (dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define __EAPI __attribute__ ((dllimport))
    #else
      #define __EAPI __declspec (dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4 || __clang__
    #define __EAPI __attribute__ ((visibility ("default")))
    #define __EAPI_LOCAL __attribute__ ((visibility ("hidden")))
  #else
    #define __EAPI
    #define __EAPI_LOCAL
  #endif
#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define __CEAPI __EAPI
  #else
    #define __CEAPI
  #endif
#endif