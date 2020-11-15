///* Copyright (C) 2015 Marc Boris Duerner 
//   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  As a special exception, you may use this file as part of a free
//  software library without restriction. Specifically, if other files
//  instantiate templates or use macros or inline functions from this
//  file, or you compile this file and link it with other files to
//  produce an executable, this file does not by itself cause the
//  resulting executable to be covered by the GNU General Public
//  License. This exception does not however invalidate any other
//  reasons why the executable file might be covered by the GNU Library
//  General Public License.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
//  02110-1301 USA
//*/
//
//#include "win32.h"
//#include "PaintSurfaceImpl.h"
//#include "PictureImpl.h"
//#include <Pt/Hmi/Application.h>
//#include <tchar.h>
//
//namespace {
//
//std::string getDefaultFont()
//{
//    HDC dc = GetDC(NULL);
//
//    std::vector<TCHAR> buffer(32);
//    GetTextFace(dc, buffer.size(), &buffer[0]);
//
//    ReleaseDC(NULL, dc);
//
//    return Pt::win32::toMultiByte(&buffer[0]);
//}
//
//}
//
//namespace Pt {
//
//namespace Hmi {
//
//#ifdef _WIN32_WCE
//
//static int CALLBACK EnumFontsProc(LOGFONT *logFont, TEXTMETRIC *physFont, DWORD type, LPARAM param)
//{
//    WCHAR* faceName = logFont->lfFaceName;
//
//    // Ignore fonts with @ as first character.
//    if (faceName[0] != '@') 
//    {  
//        std::string name = win32::toMultiByte(faceName);
//        reinterpret_cast<std::vector<std::string>*>(param)->push_back(name);
//    }
//
//    return 1;
//}
//
//#else
//
//static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *physFont, DWORD type, LPARAM param)
//{
//    char* faceName = logFont->elfLogFont.lfFaceName;    
//
//    // Ignore fonts with @ as first character.
//    if (faceName[0] != '@') 
//    {  
//        reinterpret_cast<std::vector<std::string>*>(param)->push_back(faceName);
//    }
//
//    return 1;
//}
//
//#endif
//
//
//PaintSurfaceImpl::PaintSurfaceImpl()
//{
//
//}
//
//
//PaintSurfaceImpl::~PaintSurfaceImpl()
//{
//}
//
//
//std::vector<std::string> PaintSurfaceImpl::fontNames()
//{
//    std::vector<std::string> fonts;
//    HDC dc = GetDC(NULL);
//
//#ifdef _WIN32_WCE
//    EnumFonts(dc, 0, (FONTENUMPROC)&EnumFontsProc, (LPARAM)this);
//#else
//    LOGFONT lf;
//    lf.lfCharSet = DEFAULT_CHARSET;
//    lf.lfFaceName[0] = '\0';
//    lf.lfPitchAndFamily = 0;
//
//    EnumFontFamiliesEx(dc, &lf, (FONTENUMPROC)&EnumFontFamExProc, (LPARAM)(&fonts), 0);
//#endif
//
//    ReleaseDC(NULL, dc);
//
//    fonts.erase( std::unique(fonts.begin(), fonts.end()), fonts.end() );
//    return fonts;
//}
//
//} // namespace
//
//} // namespace
