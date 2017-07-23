#define TASK4DLL_EXPORTS //comment this line for load-time linking

#ifdef TASK4DLL_EXPORTS
#define DLLPART_API __declspec(dllexport) 
#else
#define DLLPART_API __declspec(dllimport) 
#endif

#include <cctype>

extern "C" DLLPART_API int __cdecl CountWords(const wchar_t* text);