/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#if defined(STREAM_MISSING)
#else
    #include <iostream>
#endif
#include <cstring>
#include <fstream>
#include "Common/Core.h"

#ifdef _WIN32
    #undef __TEXT
    #include <windows.h>
#endif
//---------------------------------------------------------------------------
//Strings: of the CLI (MEDIAINFO_CLI), wide on Windows, else narrow, see Core.h; else of the library for the GUI, which uses the parser of the
//command line, converted with ZenLib except the wide strings on Windows
#if !defined(MEDIAINFO_CLI)
    #include "ZenLib/Ztring.h"
#endif
//Wide console of the Microsoft C library, UTF-8 when the output is redirected
#if defined(_MSC_VER)
    #define CLI_CONSOLE_WIDE
#endif
#if defined(CLI_CONSOLE_WIDE) && !defined(STREAM_MISSING)
    #include <cstdio>
    #include <cwchar>
    #include <io.h>
    #include <vector>
#endif

//---------------------------------------------------------------------------
//Get command line args in main()
#ifdef _WIN32
    #define GETCOMMANDLINE() \
        MediaInfoNameSpace::Char** argv=CommandLineToArgvW(GetCommandLineW(), &argc); \

#else //_WIN32
    #define GETCOMMANDLINE() \
        MediaInfoNameSpace::Char** argv=argv_ansi; \

#endif //_WIN32

//---------------------------------------------------------------------------
//Text in the code page, for the narrow functions of the C library on Windows (not the Microsoft one, see CLI_CONSOLE_WIDE)
#if defined(_WIN32) && !defined(CLI_CONSOLE_WIDE)
inline std::string Wide_To_CodePage(const wchar_t* Text)
{
    int Size=WideCharToMultiByte(CP_ACP, 0, Text, -1, NULL, 0, NULL, NULL);
    if (Size<=1)
        return std::string();
    std::string ToReturn(Size, '\0');
    WideCharToMultiByte(CP_ACP, 0, Text, -1, &ToReturn[0], Size, NULL, NULL);
    ToReturn.resize(Size-1);
    return ToReturn;
}
#endif

//---------------------------------------------------------------------------
//Write to a file or a pipe the text and a line end, in UTF-8 with CRLF line endings as the C runtime does in _O_U8TEXT mode,
//but by blocks of 1 MiB instead of about 1 KiB per system call
#if defined(CLI_CONSOLE_WIDE) && !defined(STREAM_MISSING)
inline void STRINGOUT_UTF8(HANDLE Handle, const wchar_t* Data, size_t Data_Size)
{
    const size_t Buffer_Size=0x100000;
    std::vector<char> Buffer(Buffer_Size+8); //Room for the last character
    size_t Buffer_Pos=0;
    for (size_t i=0; i<=Data_Size; i++)
    {
        unsigned int C=i<Data_Size?Data[i]:L'\n';
        if (C<0x80)
        {
            if (C==L'\n')
                Buffer[Buffer_Pos++]='\r';
            Buffer[Buffer_Pos++]=(char)C;
        }
        else if (C<0x800)
        {
            Buffer[Buffer_Pos++]=(char)(0xC0|(C>>6));
            Buffer[Buffer_Pos++]=(char)(0x80|(C&0x3F));
        }
        else if (C>=0xD800 && C<0xE000)
        {
            if (C<0xDC00 && i+1<Data_Size && Data[i+1]>=0xDC00 && Data[i+1]<0xE000)
            {
                C=0x10000+((C-0xD800)<<10)+(Data[++i]-0xDC00);
                Buffer[Buffer_Pos++]=(char)(0xF0|(C>>18));
                Buffer[Buffer_Pos++]=(char)(0x80|((C>>12)&0x3F));
                Buffer[Buffer_Pos++]=(char)(0x80|((C>>6)&0x3F));
                Buffer[Buffer_Pos++]=(char)(0x80|(C&0x3F));
            }
            else
            {
                //Lone surrogate, replaced by U+FFFD as with WideCharToMultiByte()
                Buffer[Buffer_Pos++]=(char)0xEF;
                Buffer[Buffer_Pos++]=(char)0xBF;
                Buffer[Buffer_Pos++]=(char)0xBD;
            }
        }
        else
        {
            Buffer[Buffer_Pos++]=(char)(0xE0|(C>>12));
            Buffer[Buffer_Pos++]=(char)(0x80|((C>>6)&0x3F));
            Buffer[Buffer_Pos++]=(char)(0x80|(C&0x3F));
        }

        if (Buffer_Pos>=Buffer_Size || i==Data_Size)
        {
            const char* ToWrite=&Buffer[0];
            while (Buffer_Pos)
            {
                DWORD Written;
                if (!WriteFile(Handle, ToWrite, (DWORD)Buffer_Pos, &Written, NULL) || !Written)
                    return;
                ToWrite+=Written;
                Buffer_Pos-=Written;
            }
        }
    }
}
//---------------------------------------------------------------------------
//Write to the terminal the text and a line end: by blocks to a file or a pipe, in one call to the console
inline void STRINGOUT_Wide(const wchar_t* Text)
{
    std::wcout.flush();
    fflush(stdout);
    size_t Data_Size=wcslen(Text); //Up to the first NUL, as std::wcout
    HANDLE Handle=(HANDLE)_get_osfhandle(_fileno(stdout));
    DWORD Console_Mode;
    if (Handle!=INVALID_HANDLE_VALUE && !GetConsoleMode(Handle, &Console_Mode))
    {
        STRINGOUT_UTF8(Handle, Text, Data_Size);
        return;
    }

    //Console: written in one call instead of character by character with std::wcout, the C runtime converts it as stdout is in _O_U8TEXT mode
    const char* Data=(const char*)Text;
    Data_Size*=sizeof(wchar_t);
    while (Data_Size)
    {
        int Written=_write(_fileno(stdout), Data, (unsigned int)(Data_Size<0x1000000?Data_Size:0x1000000));
        if (Written<=0)
            break;
        Data+=Written;
        Data_Size-=Written;
    }
    _write(_fileno(stdout), L"\n", sizeof(wchar_t));
}
#endif

//---------------------------------------------------------------------------
//Write to terminal
inline void STRINGOUT(const MediaInfoNameSpace::String& Text)
{
    #if !defined(MEDIAINFO_CLI) && !(defined(_WIN32) && defined(UNICODE))
        //GUI: text of the library (wide, or UTF-8 text without Unicode)
        #if defined(STREAM_MISSING)
            fprintf(stdout, "%s\n", ZenLib::Ztring(Text).To_Local().c_str());
        #else
            std::cout<<ZenLib::Ztring(Text).To_Local().c_str()<<std::endl;
        #endif
    #elif defined(UNICODE)
        #if defined(STREAM_MISSING)
            wprintf(L"%ls\n", Text.c_str());
        #elif defined(CLI_CONSOLE_WIDE)
            STRINGOUT_Wide(Text.c_str());
        #else //CLI_CONSOLE_WIDE
            std::cout<<Wide_To_CodePage(Text.c_str())<<std::endl;
        #endif //CLI_CONSOLE_WIDE
    #else // UNICODE
        #if defined(STREAM_MISSING)
            fprintf(stdout, "%s\n", Text.c_str());
        #else
            std::cout<<Text.c_str()<<std::endl;
        #endif //STREAM_MISSING
    #endif // UNICODE
}
inline void STRINGERR(const MediaInfoNameSpace::String& Text)
{
    #if !defined(MEDIAINFO_CLI) && !(defined(_WIN32) && defined(UNICODE))
        //GUI: text of the library (wide, or UTF-8 text without Unicode)
        #if defined(STREAM_MISSING)
            fprintf(stderr, "%s\n", ZenLib::Ztring(Text).To_Local().c_str());
        #else
            std::cerr<<ZenLib::Ztring(Text).To_Local().c_str()<<std::endl;
        #endif
    #elif defined(UNICODE)
        #if defined(STREAM_MISSING)
            fwprintf(stderr, L"%ls\n", Text.c_str());
        #elif defined(CLI_CONSOLE_WIDE)
            std::wcerr<<Text.c_str()<<std::endl;
        #else //CLI_CONSOLE_WIDE
            std::cerr<<Wide_To_CodePage(Text.c_str())<<std::endl;
        #endif //CLI_CONSOLE_WIDE
    #else // UNICODE
        #if defined(STREAM_MISSING)
            fprintf(stderr, "%s\n", Text.c_str());
        #else
            std::cerr<<Text.c_str()<<std::endl;
        #endif //STREAM_MISSING
    #endif // UNICODE
}

//ASCII text
inline void TEXTOUT(const char* Text)
{
    STRINGOUT(MediaInfoNameSpace::String(Text, Text+strlen(Text)));
}

enum MI_Return
{
    MI_OK    = 0,
    MI_ERROR,
    MI_STOP,
    MI_ADD,
};
