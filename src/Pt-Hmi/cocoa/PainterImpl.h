 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_Hmi_cocoa_PainterImpl_h
#define Pt_Hmi_cocoa_PainterImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/String.h>
#include <Pt/Utf8Codec.h>

#include <CoreText/CoreText.h>

namespace Pt {

namespace Hmi {

class PainterImpl
{
    public:
        PainterImpl();

        ~PainterImpl()
        {
          if(_font)
            CFRelease(_font);
        }

        void setPen(const Gfx::Pen& pen)
        {
        }

        void setBrush(const Gfx::Brush& brush)
        {
        }

        void setClip(const Gfx::RectF& rectF)
        {
        }

        void resetClip()
        {
        }

        void setCompositionMode(const Gfx::CompositionMode& mode)
        {
        }

        void setFont(const Gfx::Font& font)
        {
            CTFontRef f = createCTFont(font);

            if(_font)
              CFRelease(_font);

            _font = f;
        }

        CTFontRef ctFont() const
        {
            return _font;
        }
        
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text)
        {   
            CTFontRef f = createCTFont(font);
            Gfx::FontMetrics fm = fontMetrics(f, text);
            CFRelease(f);

            return fm;
        }
        
        static Gfx::FontMetrics fontMetrics(CTFontRef font, 
                                            const Pt::String& text)
        {   
            CFTypeRef keys[] = { kCTFontAttributeName };
            CFTypeRef values[] = { font };
            CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, 
                                                            keys, values, 1, 
                                                            &kCFTypeDictionaryKeyCallBacks, 
                                                            &kCFTypeDictionaryValueCallBacks);

            std::string utf8String = Utf8Codec::encode(text);
            const UInt8* stringData = reinterpret_cast<const UInt8*>( utf8String.c_str() );
            CFStringRef string = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, 
                                                               stringData, 
                                                               utf8String.length(), 
                                                               kCFStringEncodingUTF8, 
                                                               false, 
                                                               kCFAllocatorNull);

            CFAttributedStringRef attributedString = CFAttributedStringCreate(kCFAllocatorDefault, 
                                                                              string, 
                                                                              attributes);

            CTLineRef line = CTLineCreateWithAttributedString(attributedString);
            
            CGFloat ascent = 0.0;
            CGFloat descent = 0.0;
            double width = CTLineGetTypographicBounds(line, &ascent, &descent, NULL);
            
            CFRelease(line);
            CFRelease(attributedString);
            CFRelease(string);
            CFRelease(attributes);

            return Gfx::FontMetrics( ascent, 
                                     descent, 
                                     width, 
                                     ascent + descent );
        }

        static CTFontRef createCTFont(const Gfx::Font& font)
        {
            //std::clog << "font: " << font.name() 
            //          << "size: " << font.size() << std::endl;

            const UInt8* stringData = 0;
            std::size_t stringSize = 0;
            
            if( font.name().empty() )
            {
                stringData = reinterpret_cast<const UInt8*>( getDefaultFont().c_str() );
                stringSize = getDefaultFont().size();
            }
            else
            {
              stringData = reinterpret_cast<const UInt8*>( font.name().c_str() );
              stringSize = font.name().size();
            }

            CFStringRef fontName = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, 
                                                                 stringData, 
                                                                 stringSize, 
                                                                 kCFStringEncodingUTF8, 
                                                                 false, 
                                                                 kCFAllocatorNull);

            // CoreText uses 96 points per inch, but the typographic convention
            // is 72 dots per inch, so scale by 96.0 / 72.0
            CGFloat fontSize = static_cast<int>( font.size() * (96.0 / 72.0) );
        
            CGAffineTransform matrix = CGAffineTransformIdentity;
            CTFontRef f = CTFontCreateWithName(fontName, fontSize, &matrix);
            CFRelease(fontName);

            // TODO: use CTFontCreateCopyWithSymbolicTraits for bold and italic
            
            return f;
        }

        static std::string defaultFont()
        {
            return getDefaultFont();
        }

        static void setDefaultFont(const std::string& f)
        {
            getDefaultFont() = f;
        }

        static std::string& getDefaultFont()
        { 
            #if PT_IOS
                //"Helvetica"
                //"Times New Roman"
                //"Courier New"
                static std::string _defaultFont = "Helvetica";
            #else
                //"Lucida Grande"
                //"Times New Roman"
                //"Monaco"
                static std::string _defaultFont = "Helvetica";
            #endif
            
            return _defaultFont; 
        }

    private:
        CTFontRef _font;
};

} // namespace

} // namespace

#endif
