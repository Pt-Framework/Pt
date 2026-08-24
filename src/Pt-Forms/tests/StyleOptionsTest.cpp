/* Copyright (C) 2026 Marc Boris Duerner

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

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

class StyleOptionsTest : public Unit::TestSuite
{
    public:
        StyleOptionsTest()
        : Unit::TestSuite("Pt::Forms::StyleOptionsTest")
        {
            Unit::TestSuite::registerMethod("ConstructDefault", *this,
                                            &StyleOptionsTest::ConstructDefault);
            Unit::TestSuite::registerMethod("Defaults", *this,
                                            &StyleOptionsTest::Defaults);
            Unit::TestSuite::registerMethod("SetReset", *this,
                                            &StyleOptionsTest::SetReset);
            Unit::TestSuite::registerMethod("FontPartial", *this,
                                            &StyleOptionsTest::FontPartial);
            Unit::TestSuite::registerMethod("FontReplaceDropsFull", *this,
                                            &StyleOptionsTest::FontReplaceDropsFull);
            Unit::TestSuite::registerMethod("SetReplacesSameType", *this,
                                            &StyleOptionsTest::SetReplacesSameType);
            Unit::TestSuite::registerMethod("HasOverridesEmptyBag", *this,
                                            &StyleOptionsTest::HasOverridesEmptyBag);
            Unit::TestSuite::registerMethod("Generation", *this,
                                            &StyleOptionsTest::Generation);
            Unit::TestSuite::registerMethod("CopyAssign", *this,
                                            &StyleOptionsTest::CopyAssign);
            Unit::TestSuite::registerMethod("Get", *this,
                                            &StyleOptionsTest::Get);

        }

    protected:
        void ConstructDefault()
        {
            StyleOptions options;

            PT_UNIT_ASSERT( ! options.hasOverrides() );
            PT_UNIT_ASSERT(options.generation() == 0);
            PT_UNIT_ASSERT(options.find<BackgroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<ForegroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<ContourOption>() == 0);
            PT_UNIT_ASSERT(options.find<AccentColorOption>() == 0);
            PT_UNIT_ASSERT(options.find<ViewBackgroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<HighlightColorOption>() == 0);
            PT_UNIT_ASSERT(options.find<HoverBackgroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<TextBackgroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<TextColorOption>() == 0);
            PT_UNIT_ASSERT(options.find<PlaceholderTextColorOption>() == 0);
            PT_UNIT_ASSERT(options.find<HighlightedTextColorOption>() == 0);
            PT_UNIT_ASSERT(options.find<AlternateViewBackgroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<PopupBackgroundOption>() == 0);
            PT_UNIT_ASSERT(options.find<PopupTextColorOption>() == 0);
            PT_UNIT_ASSERT(options.find<FontOption>() == 0);
        }


        void Defaults()
        {
            StyleOptions options = StyleOptions::defaults();

            PT_UNIT_ASSERT( options.hasOverrides() );
            PT_UNIT_ASSERT(options.find<BackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<ForegroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<ContourOption>() != 0);
            PT_UNIT_ASSERT(options.find<AccentColorOption>() != 0);
            PT_UNIT_ASSERT(options.find<ViewBackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<HighlightColorOption>() != 0);
            PT_UNIT_ASSERT(options.find<HoverBackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<TextBackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<TextColorOption>() != 0);
            PT_UNIT_ASSERT(options.find<PlaceholderTextColorOption>() != 0);
            PT_UNIT_ASSERT(options.find<HighlightedTextColorOption>() != 0);
            PT_UNIT_ASSERT(options.find<AlternateViewBackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<PopupBackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<PopupTextColorOption>() != 0);
            PT_UNIT_ASSERT(options.find<FontOption>() != 0);

            PT_UNIT_ASSERT(options.get<BackgroundOption>().value().color() ==
                           Gfx::Color(229, 229, 229));
            PT_UNIT_ASSERT(options.get<ForegroundOption>().value().color() ==
                           Gfx::Color(210, 210, 210));
            PT_UNIT_ASSERT(options.get<ContourOption>().value().color() ==
                           Gfx::Color(150, 150, 150));
            PT_UNIT_ASSERT(options.get<AccentColorOption>().value() ==
                           Gfx::Color(20, 140, 250));
            PT_UNIT_ASSERT(options.get<ViewBackgroundOption>().value().color() ==
                           Gfx::Color(255, 255, 255));
            PT_UNIT_ASSERT(options.get<HighlightColorOption>().value() ==
                           Gfx::Color(190, 190, 190));
            PT_UNIT_ASSERT(options.get<HoverBackgroundOption>().value().color() ==
                           Gfx::Color(190, 190, 190));
            PT_UNIT_ASSERT(options.get<TextBackgroundOption>().value().color() ==
                           Gfx::Color(255, 255, 255));
            PT_UNIT_ASSERT(options.get<TextColorOption>().value() ==
                           Gfx::Color(0, 0, 0));
            PT_UNIT_ASSERT(options.get<PlaceholderTextColorOption>().value() ==
                           Gfx::Color(150, 150, 150));
            PT_UNIT_ASSERT(options.get<HighlightedTextColorOption>().value() ==
                           Gfx::Color(0, 0, 0));
            PT_UNIT_ASSERT(options.get<AlternateViewBackgroundOption>().value().color() ==
                           Gfx::Color(255, 255, 255));
            PT_UNIT_ASSERT(options.get<PopupBackgroundOption>().value().color() ==
                           Gfx::Color(229, 229, 229));
            PT_UNIT_ASSERT(options.get<PopupTextColorOption>().value() ==
                           Gfx::Color(0, 0, 0));

            const FontOption& font = options.get<FontOption>();
            PT_UNIT_ASSERT( font.isSet() );
            PT_UNIT_ASSERT(font.value().family().empty());
            PT_UNIT_ASSERT(font.value().size() == 10);

            Gfx::Font otherBase("serif", 20);
            Gfx::Font resolved = font.getFont(otherBase);
            PT_UNIT_ASSERT(resolved.family().empty());
            PT_UNIT_ASSERT(resolved.size() == 10);
        }


        void SetReset()
        {
            StyleOptions options;
            BackgroundOption background(Gfx::Color(1, 2, 3));

            options.set(background);
            PT_UNIT_ASSERT( options.hasOverrides() );
            PT_UNIT_ASSERT(options.find<BackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.get<BackgroundOption>().value().color() ==
                           Gfx::Color(1, 2, 3));

            options.reset<BackgroundOption>();
            PT_UNIT_ASSERT( ! options.hasOverrides() );
            PT_UNIT_ASSERT(options.find<BackgroundOption>() == 0);

            TextColorOption textColor(Gfx::Color(9, 8, 7));
            options.set(textColor);
            PT_UNIT_ASSERT( options.hasOverrides() );
            PT_UNIT_ASSERT(options.get<TextColorOption>().value() ==
                           Gfx::Color(9, 8, 7));

            options.reset<TextColorOption>();
            PT_UNIT_ASSERT( ! options.hasOverrides() );
            PT_UNIT_ASSERT(options.find<TextColorOption>() == 0);
        }


        void FontPartial()
        {
            StyleOptions options;
            Gfx::Font base("", 10);

            FontOption font;
            font.setSize(18);
            options.set(font);

            const FontOption* stored = options.find<FontOption>();
            PT_UNIT_ASSERT(stored != 0);
            PT_UNIT_ASSERT( stored->isSet() );

            Gfx::Font resolved = stored->getFont(base);
            PT_UNIT_ASSERT(resolved.size() == 18);
            PT_UNIT_ASSERT(resolved.family() == base.family());

            options.reset<FontOption>();
            PT_UNIT_ASSERT(options.find<FontOption>() == 0);
        }


        void FontReplaceDropsFull()
        {
            StyleOptions options;
            FontOption complete;
            complete.setFont(Gfx::Font("serif", 14));
            options.set(complete);

            FontOption sizeOnly;
            sizeOnly.setSize(18);
            options.set(sizeOnly);

            Gfx::Font base("sans", 10);
            Gfx::Font resolved = options.get<FontOption>().getFont(base);
            PT_UNIT_ASSERT(resolved.size() == 18);
            PT_UNIT_ASSERT(resolved.family() == base.family());
        }


        void SetReplacesSameType()
        {
            StyleOptions options;
            const std::size_t g0 = options.generation();

            ForegroundOption first(Gfx::Color(1, 1, 1));
            options.set(first);
            ForegroundOption second(Gfx::Color(2, 2, 2));
            options.set(second);

            PT_UNIT_ASSERT(options.get<ForegroundOption>().value().color() ==
                           Gfx::Color(2, 2, 2));
            PT_UNIT_ASSERT(options.generation() == g0 + 2);
        }


        void HasOverridesEmptyBag()
        {
            StyleOptions options;
            PT_UNIT_ASSERT( ! options.hasOverrides() );

            BackgroundOption background(Gfx::Color(1, 1, 1));
            options.set(background);
            PT_UNIT_ASSERT( options.hasOverrides() );

            options.reset<BackgroundOption>();
            PT_UNIT_ASSERT( ! options.hasOverrides() );
        }


        void Generation()
        {
            StyleOptions options;
            const std::size_t g0 = options.generation();

            BackgroundOption background(Gfx::Color(1, 1, 1));
            options.set(background);
            PT_UNIT_ASSERT(options.generation() == g0 + 1);

            options.reset<BackgroundOption>();
            PT_UNIT_ASSERT(options.generation() == g0 + 2);

            options.reset<BackgroundOption>();
            PT_UNIT_ASSERT(options.generation() == g0 + 2);

            FontOption font;
            font.setSize(18);
            options.set(font);
            PT_UNIT_ASSERT(options.generation() == g0 + 3);
        }


        void CopyAssign()
        {
            StyleOptions options;
            BackgroundOption background(Gfx::Color(4, 5, 6));
            options.set(background);

            FontOption font;
            font.setSize(12);
            options.set(font);

            StyleOptions copy(options);
            PT_UNIT_ASSERT( copy.hasOverrides() );
            PT_UNIT_ASSERT(copy.find<BackgroundOption>() != 0);
            PT_UNIT_ASSERT(copy.get<BackgroundOption>().value().color() ==
                           Gfx::Color(4, 5, 6));
            PT_UNIT_ASSERT(copy.find<FontOption>() != 0);
            PT_UNIT_ASSERT( copy.get<FontOption>().isSet() );
            PT_UNIT_ASSERT(copy.generation() == options.generation());

            StyleOptions assigned;
            assigned = options;
            PT_UNIT_ASSERT( assigned.hasOverrides() );
            PT_UNIT_ASSERT(assigned.get<BackgroundOption>().value().color() ==
                           Gfx::Color(4, 5, 6));
            PT_UNIT_ASSERT( assigned.get<FontOption>().isSet() );

            assigned.reset<BackgroundOption>();
            assigned.reset<FontOption>();
            PT_UNIT_ASSERT( ! assigned.hasOverrides() );
            PT_UNIT_ASSERT( options.hasOverrides() );
            PT_UNIT_ASSERT(options.find<BackgroundOption>() != 0);
            PT_UNIT_ASSERT(options.find<FontOption>() != 0);
        }


        void Get()
        {
            StyleOptions options;
            PT_UNIT_ASSERT_THROW(options.get<BackgroundOption>(), std::logic_error);
            PT_UNIT_ASSERT_THROW(options.get<FontOption>(), std::logic_error);
        }
};

Unit::RegisterTest<StyleOptionsTest> _registerStyleOptionsTest;

} // namespace Forms

} // namespace Pt
