/* Copyright (C) 2024 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include "CocoaFontProvider.h"

#include <Pt/Gfx/FontRegistry.h>
#include <Pt/System/FileInfo.h>

#include <CoreFoundation/CoreFoundation.h>

#include <algorithm>
#include <vector>

namespace Pt {

namespace Forms {

std::string CocoaFontProvider::toUtf8String(CFStringRef text)
{
    if( ! text )
        return std::string();

    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(text),
                                                        kCFStringEncodingUTF8) + 1;
    std::vector<char> buffer(static_cast<std::size_t>(maxSize), '\0');
    if( ! CFStringGetCString(text, &buffer[0], maxSize, kCFStringEncodingUTF8) )
        return std::string();

    return &buffer[0];
}


float CocoaFontProvider::ctFontWeight(Gfx::Font::Weight weight)
{
    switch(weight)
    {
        case Gfx::Font::Weight::Thin:
            return -0.8f;

        case Gfx::Font::Weight::ExtraLight:
            return -0.6f;

        case Gfx::Font::Weight::Light:
            return -0.4f;

        case Gfx::Font::Weight::Normal:
            return 0.0f;

        case Gfx::Font::Weight::Medium:
            return 0.23f;

        case Gfx::Font::Weight::SemiBold:
            return 0.3f;

        case Gfx::Font::Weight::Bold:
            return 0.4f;

        case Gfx::Font::Weight::ExtraBold:
            return 0.56f;

        case Gfx::Font::Weight::Black:
            return 0.62f;
    }

    return 0.0f;
}


float CocoaFontProvider::ctFontSlant(Gfx::Font::Slant slant)
{
    switch(slant)
    {
        case Gfx::Font::Slant::Normal:
            return 0.0f;

        case Gfx::Font::Slant::Italic:
            return 1.0f;

        case Gfx::Font::Slant::Oblique:
            return 0.5f;
    }

    return 0.0f;
}


Gfx::FontFace::Weight CocoaFontProvider::fontWeightFromCtWeight(float weight)
{
    if(weight >= 0.62f)
        return Gfx::FontFace::Weight::Black;

    if(weight >= 0.56f)
        return Gfx::FontFace::Weight::ExtraBold;

    if(weight >= 0.4f)
        return Gfx::FontFace::Weight::Bold;

    if(weight >= 0.3f)
        return Gfx::FontFace::Weight::SemiBold;

    if(weight >= 0.23f)
        return Gfx::FontFace::Weight::Medium;

    if(weight >= -0.2f)
        return Gfx::FontFace::Weight::Normal;

    if(weight >= -0.4f)
        return Gfx::FontFace::Weight::Light;

    if(weight >= -0.6f)
        return Gfx::FontFace::Weight::ExtraLight;

    return Gfx::FontFace::Weight::Thin;
}


Gfx::FontFace::Slant CocoaFontProvider::fontSlantFromCtTraits(float slant,
                                                              CTFontSymbolicTraits symbolicTraits)
{
    if(slant >= 0.75f || (symbolicTraits & kCTFontItalicTrait) != 0)
        return Gfx::FontFace::Slant::Italic;

    if(slant > 0.0f)
        return Gfx::FontFace::Slant::Oblique;

    return Gfx::FontFace::Slant::Normal;
}


float CocoaFontProvider::ctFontStretch(Gfx::Font::Stretch stretch)
{
    switch(stretch)
    {
        case Gfx::Font::Stretch::UltraCondensed:
            return -0.4f;

        case Gfx::Font::Stretch::ExtraCondensed:
            return -0.3f;

        case Gfx::Font::Stretch::Condensed:
            return -0.2f;

        case Gfx::Font::Stretch::SemiCondensed:
            return -0.1f;

        case Gfx::Font::Stretch::Normal:
            return 0.0f;

        case Gfx::Font::Stretch::SemiExpanded:
            return 0.1f;

        case Gfx::Font::Stretch::Expanded:
            return 0.2f;

        case Gfx::Font::Stretch::ExtraExpanded:
            return 0.3f;

        case Gfx::Font::Stretch::UltraExpanded:
            return 0.4f;
    }

    return 0.0f;
}


Gfx::FontFace::Stretch CocoaFontProvider::fontStretchFromCtWidth(float width)
{
    if(width <= -0.35f)
        return Gfx::FontFace::Stretch::UltraCondensed;

    if(width <= -0.25f)
        return Gfx::FontFace::Stretch::ExtraCondensed;

    if(width <= -0.15f)
        return Gfx::FontFace::Stretch::Condensed;

    if(width <= -0.05f)
        return Gfx::FontFace::Stretch::SemiCondensed;

    if(width <= 0.05f)
        return Gfx::FontFace::Stretch::Normal;

    if(width <= 0.15f)
        return Gfx::FontFace::Stretch::SemiExpanded;

    if(width <= 0.25f)
        return Gfx::FontFace::Stretch::Expanded;

    if(width <= 0.35f)
        return Gfx::FontFace::Stretch::ExtraExpanded;

    return Gfx::FontFace::Stretch::UltraExpanded;
}


Gfx::FontFace CocoaFontProvider::makeFontFace(const std::string& family,
                                              const std::string& style,
                                              CTFontDescriptorRef descriptor)
{
    float weight = 0.0f;
    float slant = 0.0f;
    float width = 0.0f;
    CTFontSymbolicTraits symbolicTraits = 0;

    CFTypeRef traitsAttribute = CTFontDescriptorCopyAttribute(descriptor, kCTFontTraitsAttribute);
    if(traitsAttribute)
    {
        CFDictionaryRef traits = reinterpret_cast<CFDictionaryRef>(traitsAttribute);

        CFNumberRef weightNumber = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(traits, kCTFontWeightTrait));
        if(weightNumber)
            CFNumberGetValue(weightNumber, kCFNumberFloatType, &weight);

        CFNumberRef slantNumber = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(traits, kCTFontSlantTrait));
        if(slantNumber)
            CFNumberGetValue(slantNumber, kCFNumberFloatType, &slant);

        CFNumberRef widthNumber = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(traits, kCTFontWidthTrait));
        if(widthNumber)
            CFNumberGetValue(widthNumber, kCFNumberFloatType, &width);

        CFNumberRef symbolicTraitsNumber = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(traits, kCTFontSymbolicTrait));
        if(symbolicTraitsNumber)
        {
            SInt32 value = 0;
            CFNumberGetValue(symbolicTraitsNumber, kCFNumberSInt32Type, &value);
            symbolicTraits = static_cast<CTFontSymbolicTraits>(value);
        }

        CFRelease(traitsAttribute);
    }

    return Gfx::FontFace(family,
                         fontWeightFromCtWeight(weight),
                         fontSlantFromCtTraits(slant, symbolicTraits),
                         fontStretchFromCtWidth(width),
                         style);
}

CocoaFontProvider& CocoaFontProvider::instance()
{
    static CocoaFontProvider provider;
    return provider;
}


CocoaFontProvider::CocoaFontProvider()
: _defaultFont()
{
    const std::vector<Pt::System::Path>& fontFiles = Pt::Gfx::FontRegistry::instance().fontFiles();
    for(std::vector<Pt::System::Path>::const_iterator it = fontFiles.begin(); it != fontFiles.end(); ++it)
        addFont(*it);
}


CocoaFontProvider::~CocoaFontProvider()
{
    while( ! _files.empty() )
    {
        unregisterFontFile(_files.back());
        _files.pop_back();
    }
}


const std::string& CocoaFontProvider::defaultFont() const
{
    return _defaultFont;
}


void CocoaFontProvider::setDefaultFont(const std::string& font)
{
    _defaultFont = font;
}


CTFontRef CocoaFontProvider::lookupFont(const Pt::Gfx::Font& font) const
{
    CFMutableDictionaryRef descAttributes =
        CFDictionaryCreateMutable(kCFAllocatorDefault, 3,
                                  &kCFTypeDictionaryKeyCallBacks,
                                  &kCFTypeDictionaryValueCallBacks);
    if( ! descAttributes )
        return 0;

    const std::string& family = font.family().empty() ? _defaultFont
                                                       : font.family();

    std::string categoryFamily;

    if(family.empty() && font.category() != Pt::Gfx::Font::Category::None)
    {
        switch(font.category())
        {
            case Pt::Gfx::Font::Category::Serif:     categoryFamily = "Times New Roman"; break;
            case Pt::Gfx::Font::Category::SansSerif:  categoryFamily = "Helvetica"; break;
            case Pt::Gfx::Font::Category::Monospace:  categoryFamily = "Menlo"; break;
            case Pt::Gfx::Font::Category::Cursive:    categoryFamily = "Snell Roundhand"; break;
            case Pt::Gfx::Font::Category::Fantasy:    categoryFamily = "Papyrus"; break;
            default: break;
        }
    }

    const std::string& resolvedFamily = categoryFamily.empty() ? family : categoryFamily;

    if( ! resolvedFamily.empty() )
    {
        CFStringRef familyName = CFStringCreateWithCString(kCFAllocatorDefault,
                                                           resolvedFamily.c_str(),
                                                           kCFStringEncodingUTF8);
        if(familyName)
        {
            CFDictionarySetValue(descAttributes, kCTFontFamilyNameAttribute, familyName);
            CFRelease(familyName);
        }
    }

    float fontSize = static_cast<int>( font.size() * (96.0 / 72.0) );
    CFNumberRef size = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &fontSize);
    if(size)
    {
        CFDictionarySetValue(descAttributes, kCTFontSizeAttribute, size);
        CFRelease(size);
    }

    if(font.weight() != Pt::Gfx::Font::Weight::Normal ||
       font.slant() != Pt::Gfx::Font::Slant::Normal ||
       font.stretch() != Pt::Gfx::Font::Stretch::Normal)
    {
        CFMutableDictionaryRef traits = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                                  4,
                                                                  &kCFTypeDictionaryKeyCallBacks,
                                                                  &kCFTypeDictionaryValueCallBacks);
        if(traits)
        {
            if(font.weight() != Pt::Gfx::Font::Weight::Normal)
            {
                float weight = ctFontWeight(font.weight());
                CFNumberRef weightNumber = CFNumberCreate(kCFAllocatorDefault,
                                                          kCFNumberFloatType,
                                                          &weight);
                if(weightNumber)
                {
                    CFDictionarySetValue(traits, kCTFontWeightTrait, weightNumber);
                    CFRelease(weightNumber);
                }
            }

            if(font.slant() != Pt::Gfx::Font::Slant::Normal)
            {
                float slant = ctFontSlant(font.slant());
                CFNumberRef slantNumber = CFNumberCreate(kCFAllocatorDefault,
                                                         kCFNumberFloatType,
                                                         &slant);
                if(slantNumber)
                {
                    CFDictionarySetValue(traits, kCTFontSlantTrait, slantNumber);
                    CFRelease(slantNumber);
                }
            }

            if(font.stretch() != Pt::Gfx::Font::Stretch::Normal)
            {
                float width = ctFontStretch(font.stretch());
                CFNumberRef widthNumber = CFNumberCreate(kCFAllocatorDefault,
                                                         kCFNumberFloatType,
                                                         &width);
                if(widthNumber)
                {
                    CFDictionarySetValue(traits, kCTFontWidthTrait, widthNumber);
                    CFRelease(widthNumber);
                }
            }

            CTFontSymbolicTraits symbolicTraits = 0;
            if(static_cast<int>(font.weight()) >= static_cast<int>(Pt::Gfx::Font::Weight::Bold))
                symbolicTraits |= kCTFontBoldTrait;

            if(font.slant() != Pt::Gfx::Font::Slant::Normal)
                symbolicTraits |= kCTFontItalicTrait;

            if(font.stretch() < Pt::Gfx::Font::Stretch::Normal)
                symbolicTraits |= kCTFontCondensedTrait;
            else if(font.stretch() > Pt::Gfx::Font::Stretch::Normal)
                symbolicTraits |= kCTFontExpandedTrait;

            if(symbolicTraits != 0)
            {
                SInt32 symbolicTraitsValue = static_cast<SInt32>(symbolicTraits);
                CFNumberRef symbolicTraitsNumber = CFNumberCreate(kCFAllocatorDefault,
                                                                  kCFNumberSInt32Type,
                                                                  &symbolicTraitsValue);
                if(symbolicTraitsNumber)
                {
                    CFDictionarySetValue(traits, kCTFontSymbolicTrait, symbolicTraitsNumber);
                    CFRelease(symbolicTraitsNumber);
                }
            }

            CFDictionarySetValue(descAttributes, kCTFontTraitsAttribute, traits);
            CFRelease(traits);
        }
    }

    CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes(descAttributes);
    CFRelease(descAttributes);
    if( ! descriptor )
        return 0;

    CTFontRef fontRef = CTFontCreateWithFontDescriptor(descriptor, 0, 0);
    CFRelease(descriptor);
    return fontRef;
}


std::vector<std::string> CocoaFontProvider::fontFamilies() const
{
    std::vector<std::string> families;

    CFArrayRef familyNames = CTFontManagerCopyAvailableFontFamilyNames();
    if( ! familyNames )
        return families;

    CFIndex familyCount = CFArrayGetCount(familyNames);
    for(CFIndex familyIndex = 0; familyIndex < familyCount; ++familyIndex)
    {
        CFStringRef familyName = reinterpret_cast<CFStringRef>(CFArrayGetValueAtIndex(familyNames, familyIndex));
        if(familyName)
            families.push_back(toUtf8String(familyName));
    }

    CFRelease(familyNames);

    std::sort(families.begin(), families.end());
    families.erase(std::unique(families.begin(), families.end()), families.end());
    return families;
}


std::vector<Gfx::FontFace> CocoaFontProvider::fontFaces(const std::string& family) const
{
    std::vector<Gfx::FontFace> faces;
    if(family.empty())
        return faces;

    CFStringRef familyName = CFStringCreateWithCString(kCFAllocatorDefault,
                                                       family.c_str(),
                                                       kCFStringEncodingUTF8);
    if( ! familyName )
        return faces;

    CFMutableDictionaryRef attributes = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                                  1,
                                                                  &kCFTypeDictionaryKeyCallBacks,
                                                                  &kCFTypeDictionaryValueCallBacks);
    if( ! attributes )
    {
        CFRelease(familyName);
        return faces;
    }

    CFDictionarySetValue(attributes, kCTFontFamilyNameAttribute, familyName);
    CFRelease(familyName);

    CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes(attributes);
    CFRelease(attributes);
    if( ! descriptor )
        return faces;

    CFArrayRef descriptors = CTFontDescriptorCreateMatchingFontDescriptors(descriptor, 0);
    CFRelease(descriptor);

    if( ! descriptors )
    {
        faces.push_back(Gfx::FontFace(family));
        return faces;
    }

    CFIndex descriptorCount = CFArrayGetCount(descriptors);
    if(descriptorCount == 0)
        faces.push_back(Gfx::FontFace(family));

    for(CFIndex descriptorIndex = 0; descriptorIndex < descriptorCount; ++descriptorIndex)
    {
        CTFontDescriptorRef match = reinterpret_cast<CTFontDescriptorRef>(CFArrayGetValueAtIndex(descriptors, descriptorIndex));
        CFTypeRef styleAttribute = CTFontDescriptorCopyAttribute(match, kCTFontStyleNameAttribute);
        std::string style;
        if(styleAttribute)
        {
            style = toUtf8String(reinterpret_cast<CFStringRef>(styleAttribute));
            CFRelease(styleAttribute);
        }

        faces.push_back(makeFontFace(family, style, match));
    }

    CFRelease(descriptors);

    std::sort(faces.begin(), faces.end());
    faces.erase(std::unique(faces.begin(), faces.end()), faces.end());
    return faces;
}


void CocoaFontProvider::onAddFont(const Pt::System::Path& path)
{
    if( ! Pt::System::FileInfo::exists(path) )
        return;

    if(std::find(_files.begin(), _files.end(), path) != _files.end())
        return;

    if( ! registerFontFile(path) )
        return;

    _files.push_back(path);
}


void CocoaFontProvider::onRemoveFont(const Pt::System::Path& path)
{
    std::vector<Pt::System::Path>::iterator pos = std::find(_files.begin(), _files.end(), path);
    if(pos == _files.end())
        return;

    unregisterFontFile(*pos);
    _files.erase(pos);
}


bool CocoaFontProvider::registerFontFile(const Pt::System::Path& path)
{
    const std::string localPath = path.toLocal();
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                           reinterpret_cast<const UInt8*>(localPath.c_str()),
                                                           localPath.size(),
                                                           false);
    if( ! url )
        return false;

    CFErrorRef error = 0;
    bool ok = CTFontManagerRegisterFontsForURL(url,
                                               kCTFontManagerScopeProcess,
                                               &error);
    if(error)
        CFRelease(error);

    CFRelease(url);
    return ok;
}


void CocoaFontProvider::unregisterFontFile(const Pt::System::Path& path)
{
    const std::string localPath = path.toLocal();
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                           reinterpret_cast<const UInt8*>(localPath.c_str()),
                                                           localPath.size(),
                                                           false);
    if( ! url )
        return;

    CFErrorRef error = 0;
    CTFontManagerUnregisterFontsForURL(url,
                                       kCTFontManagerScopeProcess,
                                       &error);
    if(error)
        CFRelease(error);

    CFRelease(url);
}

} // namespace Forms

} // namespace Pt