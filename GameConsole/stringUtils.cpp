#include "stringUtils.h"

#if defined(WIN32)

wchar_t* ansi_to_unicode(const char* pcMbcs)
{
    //assert(pcMbcs != NULL);
    int iWcsLength;
    int iBufferLength;
    wchar_t* pcWcs;

    iWcsLength = ::MultiByteToWideChar(
        CP_ACP,
        MB_ERR_INVALID_CHARS, 
        pcMbcs, 
        strlen(pcMbcs),
        NULL,
        0);
    if (iWcsLength) 
    {
        iBufferLength = sizeof(*pcWcs) * (iWcsLength + 1);
        pcWcs = (wchar_t*)malloc(iBufferLength);
        memset(pcWcs, 0, iBufferLength);
        ::MultiByteToWideChar(
            CP_ACP,
            MB_ERR_INVALID_CHARS, 
            pcMbcs, 
            strlen(pcMbcs),
            pcWcs,
            iWcsLength);
    }
    // TODO error handle
    //else {
    //    String s;
    //    s.Format("WideCharToMultiByte ß∞‹ %d\n", ::GetLastError());
    //    std::cout << s << std::endl;
    //    exit(1);
    //}
    return pcWcs;
}

#endif // WIN32