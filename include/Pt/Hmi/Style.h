/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner
 
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

#ifndef Pt_Hmi_Style_h
#define Pt_Hmi_Style_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/TypeInfo.h>
#include <Pt/NonCopyable.h>
#include <map>

namespace Pt {

namespace Hmi {

class Frame;
class Panel;
class Label;
class Button;
class CheckBox;
class PaintSurface;
class StyleOptions;

class PT_HMI_API Style
{
    public:
        class Facet : protected NonCopyable
        {
            public:
                explicit Facet(const std::type_info& ti, std::size_t refs = 0)
                : _typeId(&ti)
                , _refs(refs)
                {}

                virtual ~Facet()
                {}

                const std::type_info& typeId() const
                {
                    return *_typeId;
                }

                void ref()
                { 
                    ++_refs; 
                }

                std::size_t unref()
                { 
                    return --_refs; 
                }

            private:
                const std::type_info* _typeId;
                std::size_t _refs;
        };

    public:
        Style();

        Style(const Style& style);

        virtual ~Style();

        Style& operator=(const Style& style);

        void assign(const Style& style);

        void set(Facet* facet);

        template <typename FacetT> 
        const FacetT* get() const
        {
            const Facet* facet = find( typeid(FacetT) );
            return static_cast<const FacetT*>(facet);
        }

    private:
        const Facet* find(const std::type_info& ti) const;

    private:
        typedef std::map<TypeInfo, Facet*> FacetMap;
        FacetMap _facets;
};


class PT_HMI_API ButtonRenderer : public Style::Facet
{
    public:
        ButtonRenderer(std::size_t refs = 0);

        virtual ~ButtonRenderer();

        void render(const Button& button, 
                    PaintSurface& surface, 
                    const Gfx::RectF& rect) const;

    protected:
        virtual void onRender(const Button& button, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;
};


class PT_HMI_API CheckBoxRenderer : public Style::Facet
{
    public:
        CheckBoxRenderer(std::size_t refs = 0);

        virtual ~CheckBoxRenderer();

        void render(const CheckBox& cb, 
                    PaintSurface& surface, 
                    const Gfx::RectF& rect) const;

    protected:
        virtual void onRender(const CheckBox& cb, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;
};


class PT_HMI_API FrameRenderer : public Style::Facet
{
    public:
        FrameRenderer(std::size_t refs = 0);

        virtual ~FrameRenderer();

        void render(const Frame& f,
                    PaintSurface& surface, 
                    const Gfx::RectF& rect) const;

    protected:
        virtual void onRender(const Frame& f, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;
};

class PT_HMI_API PanelRenderer : public Style::Facet
{
    public:
        PanelRenderer(std::size_t refs = 0);

        virtual ~PanelRenderer();

        void render(const Panel& p,
                    PaintSurface& surface, 
                    const Gfx::RectF& rect) const;

    protected:
        virtual void onRender(const Panel& p, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;
};

class PT_HMI_API LabelRenderer : public Style::Facet
{
    public:
        LabelRenderer(std::size_t refs = 0);

        virtual ~LabelRenderer();

        void render(const Label& l,
                    PaintSurface& surface,
                    const Gfx::RectF& rect) const;

    protected:
        virtual void onRender(const Label& l, 
                              PaintSurface& surface, 
                              const Gfx::RectF& rect) const = 0;
};

} // namespace

} // namespace

#endif
