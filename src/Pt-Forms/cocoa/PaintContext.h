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

#ifndef PT_FORMS_COCOA_PAINTCONTEXT_H
#define PT_FORMS_COCOA_PAINTCONTEXT_H

#include "PixmapImpl.h"

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/PaintContext.h>

#include <Pt/String.h>
#include <Pt/Utf8Codec.h>

#include <CoreText/CoreText.h>

namespace Pt {

namespace Forms {

class PaintContext : public Gfx::PaintContext
{
    public:
        PaintContext()
        : _font(0)
        , _fontAttributes(0)
        , _textColor(0)
        , _hasClip(false)
        { 
        }

        ~PaintContext()
        {
          if(_font)
            CFRelease(_font);

          if(_textColor)
            CFRelease(_textColor);

          if(_fontAttributes)
            CFRelease(_fontAttributes);
        }

        const Gfx::CompositionMode& compositionMode() const
        {
            return _compositionMode;
        }

        const Gfx::Pen& pen() const
        {
            return _pen;
        }

        const Gfx::Brush& brush() const
        {
            return _brush;
        }

        CTFontRef font() const
        {
            return _font;
        }

        CFMutableDictionaryRef fontAttributes() const
        {
            return _fontAttributes;
        }

        const Gfx::RectF* clipRect() const
        {
            return _hasClip ? &_clip : 0;
        }

    protected:
        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override
        {
            _compositionMode = mode;
        }

        virtual void onSetPen(const Gfx::Pen& pen) override
        {
            _pen = pen;

            if(_fontAttributes)
            {
                CGColorRef textColor = CGColorCreateGenericRGB(_pen.color().red() / 65535.0,
                                                               _pen.color().green() / 65535.0,
                                                               _pen.color().blue() / 65535.0,
                                                               _pen.color().alpha() / 65535.0);
                if(_textColor)
                  CFRelease(_textColor);

                _textColor = textColor;

                CFDictionarySetValue(_fontAttributes, kCTForegroundColorAttributeName, _textColor);
            }
        }

        virtual void onSetBrush(const Gfx::Brush& brush) override
        {
            _brush = brush;
        }

        virtual void onSetFont(const Gfx::Font& font) override
        {
            if( ! _fontAttributes )
            {
                _fontAttributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 2, 
                                                            &kCFTypeDictionaryKeyCallBacks, 
                                                            &kCFTypeDictionaryValueCallBacks);
                if( ! _fontAttributes )
                    return;
            }

            if( ! _textColor )
            {
                _textColor = CGColorCreateGenericRGB(_pen.color().red() / 65535.0,
                                                     _pen.color().green() / 65535.0,
                                                     _pen.color().blue() / 65535.0,
                                                     _pen.color().alpha() / 65535.0);
                if( ! _textColor )
                    return;

                CFDictionarySetValue(_fontAttributes, kCTForegroundColorAttributeName, _textColor);
            }

            CFMutableDictionaryRef descAttributes = 
                CFDictionaryCreateMutable(kCFAllocatorDefault, 3, 
                                          &kCFTypeDictionaryKeyCallBacks, 
                                          &kCFTypeDictionaryValueCallBacks);
            if( ! descAttributes )
                return;

            const std::string& fontName = font.name().empty() ? PixmapImpl::defaultFont().c_str()
                                                              : font.name().c_str();
            CFStringRef name = CFStringCreateWithCStringNoCopy(0, fontName.c_str(), 
                                                               kCFStringEncodingUTF8,
                                                               kCFAllocatorNull);
            CFDictionarySetValue(descAttributes, kCTFontFamilyNameAttribute, name);
            CFRelease(name);

            // CoreText uses 96 points per inch, but the typographic convention
            // is 72 dots per inch, so scale the size by 96.0 / 72.0
            float fontSize = static_cast<int>( font.size() * (96.0 / 72.0) );
            CFNumberRef size = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &fontSize);
            CFDictionarySetValue(descAttributes, kCTFontSizeAttribute, size);
            CFRelease(size);

            CFStringRef style = CFStringCreateWithCStringNoCopy(0, font.style().c_str(), 
                                                                kCFStringEncodingUTF8,
                                                                kCFAllocatorNull);
            CFDictionarySetValue(descAttributes, kCTFontStyleNameAttribute, style);
            CFRelease(style);

            CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes(descAttributes);
            CFRelease(descAttributes);

            if( ! descriptor )
                return;

            CTFontRef fontRef = CTFontCreateWithFontDescriptor(descriptor, 0, 0);
            CFRelease(descriptor);

            if( ! fontRef )
                return;

            if(_font)
                CFRelease(_font);

            _font = fontRef;
            CFDictionarySetValue(_fontAttributes, kCTFontAttributeName, _font);
        }

        virtual void onSetClip(const Gfx::RectF* clip) override
        {
            _hasClip = clip != 0;
            if(clip)
                _clip = *clip;
        }

    private:
        Gfx::CompositionMode      _compositionMode;
        Gfx::Pen                  _pen;
        Gfx::Brush                _brush;
        CTFontRef                 _font;
        CFMutableDictionaryRef    _fontAttributes;
        CGColorRef                _textColor;
        Gfx::RectF                _clip;
        bool                      _hasClip;
};

} // namespace

} // namespace

#endif
