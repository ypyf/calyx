#ifndef __string_utils_h__
#define __string_utils_h__

#include <string>

#ifndef UNICODE  
typedef std::string TString; 
#else
typedef std::wstring TString; 
#endif

#if defined(WIN32)
#include <windows.h>
wchar_t* ansi_to_unicode(const char* pcMbcs);
#endif // WIN32

#endif	// __string_utils_h__