/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef CoreH
#define CoreH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//CLI without Unicode (MEDIAINFO_CLI_NOUNICODE, not for Windows): narrow strings, the arguments as the system gives them and the output in
//the encoding of the locale, with the classes of the library for these strings (or its C API), so whatever the Unicode mode of the library
//and without ZenLib
#if defined(MEDIAINFO_CLI_NOUNICODE)
    #if defined(_WIN32)
        #error MEDIAINFO_CLI_NOUNICODE is not for Windows
    #endif
    #undef UNICODE //Also if it comes from the flags of the libraries
    #undef _UNICODE
#endif

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL_RUNTIME
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_DLL_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfoList.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
using namespace MediaInfoNameSpace;

//---------------------------------------------------------------------------
//Classes of the library
#if defined(MEDIAINFO_CLI_NOUNICODE) && !defined(MEDIAINFO_DLL_RUNTIME) && !defined(MEDIAINFO_DLL_STATIC)
    typedef MediaInfoLib::MediaInfoA Core_MediaInfo;
    typedef MediaInfoLib::MediaInfoListA Core_MediaInfoList;
#else
    typedef MediaInfoNameSpace::MediaInfo Core_MediaInfo;
    typedef MediaInfoNameSpace::MediaInfoList Core_MediaInfoList;
#endif
//---------------------------------------------------------------------------

//***************************************************************************
// Class Core
//***************************************************************************

class Core
{
public:
    //Constructor/Destructor
    Core();
    ~Core();

    //Menu
    size_t  Menu_File_Open_File             (const MediaInfoNameSpace::String &FileName);
    void    Menu_File_Open_Files_Begin      (bool Close=true, bool WithThread=false);
    size_t  Menu_File_Open_Files_Continue   (const MediaInfoNameSpace::String &FileName);
    void    Menu_File_Open_Directory        (const MediaInfoNameSpace::String &DirectoryName);
    void    Menu_View_Easy                  ();
    void    Menu_View_Sheet                 ();
    void    Menu_View_Tree                  ();
    void    Menu_View_Text                  ();
    void    Menu_View_HTML                  ();
    void    Menu_View_XML                   ();
    void    Menu_View_JSON                  ();
    void    Menu_View_CSV                   ();
    void    Menu_View_MPEG7                 ();
    void    Menu_View_MPEG7_Strict          ();
    void    Menu_View_MPEG7_Relaxed         ();
    void    Menu_View_MPEG7_Extended        ();
    void    Menu_View_PBCore                ();
    void    Menu_View_PBCore2               ();
    void    Menu_View_EBUCore_1_5           ();
    void    Menu_View_EBUCore_1_6           ();
    void    Menu_View_EBUCore_1_8_ps        ();
    void    Menu_View_EBUCore_1_8_sp        ();
    void    Menu_View_EBUCore_1_8_ps_json   ();
    void    Menu_View_EBUCore_1_8_sp_json   ();
    void    Menu_View_FIMS_1_1              ();
    void    Menu_View_FIMS_1_2              ();
    void    Menu_View_FIMS_1_3              ();
    void    Menu_View_reVTMD                ();
    void    Menu_View_NISO_Z39_87           ();
    void    Menu_View_Graph_Svg             ();
    void    Menu_Option_Preferences_Inform  (const MediaInfoNameSpace::String &Inform);
    String  Menu_Option_Preferences_Option  (const MediaInfoNameSpace::String &Param, const MediaInfoNameSpace::String &Value);
    void    Menu_Debug_Complete             (bool Value);
    void    Menu_Debug_Details              (float Value);
    void    Menu_Debug_Demux                (const MediaInfoNameSpace::String &Value);
    void    Menu_Help_Version               ();
    void    Menu_Help_Info_Formats          ();
    void    Menu_Help_Info_Codecs           ();
    void    Menu_Help_Info_Parameters       ();
    void    Menu_Help_Info_CanHandleUrls    ();
    void    Menu_Language                   (const MediaInfoNameSpace::String &Language);

    //Datas
    MediaInfoNameSpace::String &Inform_Get();
    enum kind {Kind_Easy, Kind_Sheet, Kind_Tree, Kind_HTML, Kind_Text, Kind_XML, Kind_JSON, Kind_CSV, Kind_PBCore, Kind_PBCore2, Kind_reVTMD, Kind_MPEG7_Strict, Kind_MPEG7_Relaxed, Kind_MPEG7_Extended, Kind_EBUCore_1_5, Kind_EBUCore_1_6, Kind_EBUCore_1_8_ps, Kind_EBUCore_1_8_sp, Kind_EBUCore_1_8_ps_json, Kind_EBUCore_1_8_sp_json, Kind_FIMS_1_1, Kind_FIMS_1_2, Kind_FIMS_1_3, Kind_NISO_Z39_87, Kind_Graph_Svg};
    kind    Kind_Get();
    MediaInfoNameSpace::String  Inform_Get          (size_t Pos, stream_t StreamKind, size_t StreamPos);

    //Accessors to MI
    size_t Count_Get();
    size_t Count_Get (size_t FilePos, stream_t StreamKind, size_t StreamNumber=(size_t)-1);
    String Get (size_t FilePos, stream_t StreamKind, size_t StreamNumber, size_t Parameter, info_t InfoKind=Info_Text);
    String Get (size_t FilePos, stream_t StreamKind, size_t StreamNumber, const String &Parameter, info_t InfoKind=Info_Text);
    String Summary_Get(int File_Pos, stream_t StreamKind, size_t StreamPos);
    String Parameters();
    String StreamName (stream_t StreamKind);
    size_t State_Get();

    //Temp
    void    Data_Prepare();
    MediaInfoNameSpace::String &Text_Get() {return Text;};
    void* GUI_Main_Handler;

    Core_MediaInfoList* MI;
protected:
    MediaInfoNameSpace::String  Text;
    kind    Kind;
    float   Details;
    MediaInfoNameSpace::String  Demux;

private:
    Core (const Core&);
	Core &operator =(const Core &a);

    friend class GUI_Main_Common_Core;
    friend class GUI_Main_Easy_Core;
    friend class GUI_Main_Easy_Box_Core;
};

#endif
