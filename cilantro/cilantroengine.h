#ifdef BUILDING_DLL
#define __EAPI __declspec(dllexport)
#else
#define __EAPI __declspec(dllimport)
#endif
