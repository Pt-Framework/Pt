///* Copyright (C) 2016 Marc Boris Duerner 
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
//  MA 02110-1301 USA
//*/
//
//#ifndef Pt_Hmi_PainterImpl_h
//#define Pt_Hmi_PainterImpl_h
//
//#include <Pt/Hmi/Api.h>
//#include <Pt/Hmi/Application.h>
//#include <Pt/Gfx/Pen.h>
//#include <Pt/Gfx/Brush.h>
//#include <Pt/Gfx/Font.h>
//#include <Pt/Gfx/FontMetrics.h>
//#include <Pt/Gfx/CompositionMode.h>
//#include <Pt/Gfx/ImagePainter.h>
//#include <Pt/Gfx/Rect.h>
//#include <Pt/String.h>
//
//namespace Pt {
//
//namespace Hmi {
//
//class PainterImpl
//{
//    public:
//        PainterImpl()
//        { }
//
//        ~PainterImpl()
//        {
//        }
//
//        void setPen(const Gfx::Pen& pen)
//        {
//        }
//
//        void setBrush(const Gfx::Brush& brush)
//        {
//        }
//
//        void setClip(const Gfx::RectF& rectF)
//        {
//        }
//
//        void resetClip()
//        {
//        }
//
//        void setCompositionMode(const Gfx::CompositionMode& mode)
//        {
//        }
//
//        void setFont(const Gfx::Font& font)
//        {
//        }
//        
//        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
//                                            const Pt::String& text)
//        {   
//            return Gfx::ImagePainter::fontMetrics(font, text); 
//        }
//        
//        static std::string defaultFont()
//        {
//            return Gfx::ImagePainter::defaultFont();
//        }
//
//        static void setDefaultFont(const std::string& f)
//        {
//            Gfx::ImagePainter::setDefaultFont(f);
//        }
//
//        static void setFontDir(const Pt::System::Path& path)
//        {
//            Gfx::ImagePainter::setFontDir(path);
//        }
//};
//
//} // namespace
//
//} // namespace
//
//#endif // include guard
