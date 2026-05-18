/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner
    Copyright (C) 2017 Ilja Maier

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

#ifndef Pt_Forms_Style_h
#define Pt_Forms_Style_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Alignment.h>
#include <Pt/Forms/Spacing.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/TypeInfo.h>
#include <Pt/NonCopyable.h>

#include <map>
#include <cstddef>

namespace Pt {

namespace Forms {

class PaintContext;
class Painter;
class Pixmap;
class TextBlock;
class StyleOptions;
class LineEdit;
class PushButton;
class CheckBox;
class ComboBox;
class Menu;
class MenuItem;
class MenuBar;
class MenuBarItem;
class ScrollBar;
class Slider;
class SpinBox;
class SpinBoxButton;
class ProgressBar;
class ListBox;
class ListBoxItem;
class TabView;
class TabBar;
class TabItem;

template <typename T>
class FacetPtr
{
    public:
        FacetPtr(T* facet = 0)
        : _facet(facet)
        {
            if( _facet )
                _facet->ref();
        }

        FacetPtr(const FacetPtr& ptr)
        : _facet(ptr._facet)
        {
            if( _facet )
                _facet->ref();
        }

        ~FacetPtr()
        {
            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }
        }

        FacetPtr& operator=(const FacetPtr& ptr)
        {
            if(this == &ptr)
                return *this;

            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }

            _facet = ptr._facet;
            if( _facet )
                _facet->ref();

            return *this;
        }

        void reset(T* facet = 0)
        {
            if (_facet == facet)
                return;

            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }

            _facet = facet;
            if( _facet )
                _facet->ref();
        }

        T* operator->() const 
        { return _facet; }

        T& operator*() const
        { return *_facet; }

        bool operator! () const
        { return _facet == 0; }

        operator bool () const
        { return _facet != 0; }

        T* get()
        { return _facet; }

        const T* get() const
        { return _facet; }

    private:
        T* _facet;
};

class PT_FORMS_API Style
{
    public:
        class Facet : private NonCopyable
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

        void combine(const Style& style);

        void set(Facet* facet);

        template <typename FacetT> 
        FacetT* get() const
        {
            Facet* facet = find( typeid(FacetT) );
            return static_cast<FacetT*>(facet);
        }

    private:
        Facet* find(const std::type_info& ti) const;

    private:
        typedef std::map<TypeInfo, Facet*> FacetMap;
        FacetMap _facets;
};


class PT_FORMS_API PanelRenderer : public Style::Facet
{
    public:
        explicit PanelRenderer(std::size_t refs = 0);

        virtual ~PanelRenderer();

        PanelRenderer* create() const;

    public:
        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& f);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Pen& p);

    protected:
        const StyleOptions& prepare();

        virtual PanelRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

    public:
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              StyleFlags state);

    protected:
        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        StyleFlags state) = 0;

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect);

        void renderFrame(PaintContext& context,
                         const Gfx::RectF& rect,
                         StyleFlags state);

    protected:
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect) = 0;

        virtual void onRenderFrame(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const StyleOptions& options,
                                   StyleFlags state) = 0;

    public:
        const Painter& textPainter(PaintSurface& surface);

        void renderText(PaintContext& context,
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& pos,
                        StyleFlags state);

    protected:
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  StyleFlags state) = 0;

    public:
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& rect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        StyleFlags state);

    protected:
        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  StyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API ButtonRenderer : public Style::Facet
{
    public:
        explicit ButtonRenderer(std::size_t refs = 0);

        virtual ~ButtonRenderer();

        ButtonRenderer* create() const;

    public: 
        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& accentColor() const;

        void setAccentColor(const Gfx::Color& c);

        const Gfx::Color& highlightColor() const;

        void setHighlightColor(const Gfx::Color& c);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& f);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Pen& p);

    protected:
        const StyleOptions& prepare();

        virtual ButtonRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

    public:
        Gfx::SizeF measureSurface(PaintSurface& surface,
                                  const Gfx::SizeF& contentSize);

        Gfx::RectF layoutSurface(PaintSurface& surface,
                                 const Gfx::RectF& surfaceRect);

        void renderSurface(PaintContext& context,
                           const Gfx::RectF& rect,
                           ButtonStyleFlags state);

    protected:
        virtual Gfx::SizeF onMeasureSurface(PaintSurface& surface,
                                            const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutSurface(PaintSurface& surface,
                                           const Gfx::RectF& surfaceRect) = 0;

        virtual void onRenderSurface(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     const StyleOptions& options,
                                     ButtonStyleFlags state) = 0;

    public:
        const Painter& textPainter(PaintSurface& surface);

        void renderText(PaintContext& context,
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& pos,
                        ButtonStyleFlags state);

    protected:
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  ButtonStyleFlags state) = 0;

    public:
        Gfx::RectF layoutMnemonic(PaintSurface& surface,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& fontMetrics,
                                  String::size_type mnemonicIndex);

        void renderMnemonic(PaintContext& context,
                            const Gfx::RectF& rect,
                            const Gfx::RectF& mnemonic,
                            ButtonStyleFlags state);

    protected:
        virtual Gfx::RectF onLayoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex) = 0;

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const StyleOptions& options,
                                      const Gfx::RectF& mnemonic,
                                      ButtonStyleFlags state) = 0;

    public:
        void prepareIcon(const Gfx::Image& icon,
                         Pixmap& picture,
                         ButtonStyleFlags state) const;

        void renderIcon(PaintContext& context,
                        const Gfx::RectF& rect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        ButtonStyleFlags state);

    protected:
        virtual void onPrepareIcon(const StyleOptions& options,
                                   const Gfx::Image& icon,
                                   Pixmap& picture,
                                   ButtonStyleFlags state) const = 0;

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const StyleOptions& options,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  ButtonStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _foreground;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Color> _accentColor;
        AutoPtr<Gfx::Color> _highlightColor;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API CheckBoxRenderer : public Style::Facet
{
    public:
        CheckBoxRenderer(std::size_t refs = 0);

        virtual ~CheckBoxRenderer();

        void prepare(const CheckBox& cb,
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen,
                     Gfx::SizeF& boxSize) const;

        void renderBox(const CheckBox& cb,
                       const StyleOptions& options,
                       Painter& painter, 
                       const Gfx::RectF& rect,
                       const Gfx::RectF& boxRect,
                       const Gfx::Brush& brush,
                       const Gfx::Pen& pen) const;

        void renderText(const CheckBox& cb,
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::TextMetrics& textMetric,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen,
                        const Gfx::RectF& mnemonic) const;

    protected:
        virtual void onPrepare(const CheckBox& cb,
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen,
                               Gfx::SizeF& boxSize) const = 0;

        virtual void onRenderBox(const CheckBox& cb,
                                 const StyleOptions& options,
                                 Painter& painter, 
                                 const Gfx::RectF& rect,
                                 const Gfx::RectF& boxRect,
                                 const Gfx::Brush& brush,
                                 const Gfx::Pen& pen) const = 0;

        virtual void onRenderText(const CheckBox& cb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::TextMetrics& textMetric,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& mnemonic) const = 0;
};




class PT_FORMS_API LineEditRenderer : public Style::Facet
{
    public:
        LineEditRenderer(std::size_t refs = 0);

        virtual ~LineEditRenderer();

        void prepare(const LineEdit& le, 
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen) const;
        
        void renderBackground(const LineEdit& le, 
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Pen& contour,
                              const Gfx::Brush& brush) const;

        void renderText(const LineEdit& le, 
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::Font& font,
                        const Gfx::Pen& textPen) const;

        void renderCursor(const LineEdit& le, 
                          const StyleOptions& options,
                          Painter& painter, 
                          const Gfx::RectF& rect,
                          const Gfx::RectF& cursorRect ) const;
    
    protected:
        virtual void onPrepare(const LineEdit& le, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const = 0;

        virtual void onRenderBackground(const LineEdit& le, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const = 0;

        virtual void onRenderText(const LineEdit& le, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font,
                                  const Gfx::Pen& textPen) const = 0;

        virtual void onRenderCursor(const LineEdit& le, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& cursorRect ) const = 0;
};

class PT_FORMS_API MenuRenderer : public Style::Facet
{
    public:
        MenuRenderer(std::size_t refs = 0);

        virtual ~MenuRenderer();

        void prepare(const Menu& m, 
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour) const;

        void prepareItem(const MenuItem& m, 
                         const StyleOptions& options,
                         const Gfx::Image& icon,
                         Pixmap& picture,
                         Gfx::Brush& brush,
                         Gfx::Pen& contour,
                         Gfx::Font& font,
                         Gfx::Pen& textPen) const;

        void renderBackground(const Menu& m, 
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Brush& brush,
                              const Gfx::Pen& contour) const;

        void renderItem(const MenuItem& m, 
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        Gfx::Brush& brush,
                        Gfx::Pen& contour) const;
        
        void renderIndicator(const MenuItem& m, 
                             const StyleOptions& options,
                             Painter& painter, 
                             const Gfx::RectF& rect) const;
    
    protected:
        virtual void onPrepare(const Menu& m, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour) const = 0;

        virtual void onPrepareItem(const MenuItem& m, 
                                   const StyleOptions& options,
                                   const Gfx::Image& icon,
                                   Pixmap& picture,
                                   Gfx::Brush& brush,
                                   Gfx::Pen& contour,
                                   Gfx::Font& font,
                                   Gfx::Pen& textPen) const = 0;

        virtual void onRenderBackground(const Menu& m, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& contour) const = 0;

        virtual void onRenderItem(const MenuItem& m, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  Gfx::Brush& brush,
                                  Gfx::Pen& contour) const = 0;
        
        virtual void onRenderIndicator(const MenuItem& m, 
                                       const StyleOptions& options,
                                       Painter& painter, 
                                       const Gfx::RectF& rect) const = 0;
};

class PT_FORMS_API MenuBarRenderer : public Style::Facet
{
    public:
        MenuBarRenderer(std::size_t refs = 0);

        virtual ~MenuBarRenderer();

        void prepare(const MenuBar& m, 
                     const StyleOptions& options,
                     Gfx::Brush& brush,
                     Gfx::Pen& contour) const;

        void renderBackground(const MenuBar& m, 
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Brush& brush,
                              const Gfx::Pen& contour) const;

        void prepareItem(const MenuBarItem& m, 
                         const StyleOptions& options, 
                         Gfx::Brush& brush,
                         Gfx::Pen& contour,
                         Gfx::Font& font,
                         Gfx::Pen& textPen) const;

        void renderItem(const MenuBarItem& m, 
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const Gfx::Brush& brush,
                        const Gfx::Pen& contour) const;

        void renderItemText(const MenuBarItem& m,
                            const StyleOptions& options,
                            Painter& painter, 
                            const Gfx::RectF& rect,
                            const String& text,
                            const Gfx::PointF& textPos,
                            const Gfx::Font& font, 
                            const Gfx::Pen& textPen,
                            const Gfx::RectF& mnemonic) const;

    protected:
        virtual void onPrepare(const MenuBar& m, 
                               const StyleOptions& options,
                               Gfx::Brush& brush,
                               Gfx::Pen& contour) const = 0;

        virtual void onRenderBackground(const MenuBar& m, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush,
                                        const Gfx::Pen& contour) const = 0;

        virtual void onPrepareItem(const MenuBarItem& m, 
                                   const StyleOptions& options, 
                                   Gfx::Brush& brush,
                                   Gfx::Pen& contour,
                                   Gfx::Font& font,
                                   Gfx::Pen& textPen) const = 0;

        virtual void onRenderItem(const MenuBarItem& m, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const Gfx::Brush& brush,
                                  const Gfx::Pen& contour) const = 0;

        virtual void onRenderItemText(const MenuBarItem& m,
                                      const StyleOptions& options,
                                      Painter& painter, 
                                      const Gfx::RectF& rect,
                                      const String& text,
                                      const Gfx::PointF& textPos,
                                      const Gfx::Font& font, 
                                      const Gfx::Pen& textPen,
                                      const Gfx::RectF& mnemonic) const = 0;
};


class PT_FORMS_API ScrollBarRenderer : public Style::Facet
{
    public:
        ScrollBarRenderer(std::size_t refs = 0);

        virtual ~ScrollBarRenderer();

        void prepare(const ScrollBar& s,
                     const StyleOptions& options,
                     Gfx::Brush& background,
                     Gfx::Brush& foreground,
                     Gfx::Pen& contour) const;
        
        void render(const ScrollBar& s,
                    const StyleOptions& options,
                    Painter& painter,
                    const Gfx::RectF& rect,
                    const Gfx::RectF& handleRect,
                    const Gfx::Brush& background,
                    const Gfx::Brush& foreground,
                    const Gfx::Pen& contour) const;
    
    protected:
        virtual void onPrepare(const ScrollBar& s,
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour) const = 0;
        
        virtual void onRender(const ScrollBar& s,
                              const StyleOptions& options,
                              Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::RectF& handleRect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour) const = 0;
};


class PT_FORMS_API ProgressBarRenderer : public Style::Facet
{
    public:
        ProgressBarRenderer(std::size_t refs = 0);

        virtual ~ProgressBarRenderer();

        void prepare(const ProgressBar&    p,
                     const StyleOptions&  options,
                     Gfx::Brush&          background,
                     Gfx::Brush&          foreground,
                     Gfx::Pen&            contour,
                     Gfx::Pen&            textPen,
                     Gfx::Font&            font
                     ) const;

       void render( const ProgressBar& p,
                               const StyleOptions& options,
                              Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour,
                              const Gfx::Pen&            textPen,
                              const Gfx::Font&            font
                                         ) const;

    protected:
        virtual void onPrepare(const ProgressBar&    p,
                               const StyleOptions&  options,
                               Gfx::Brush&          background,
                               Gfx::Brush&          foreground,
                               Gfx::Pen&            contour,
                               Gfx::Pen&            textPen,
                               Gfx::Font&            font
                               ) const = 0;

        virtual void onRender( const ProgressBar& p,
                               const StyleOptions& options,
                              Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour,
                              const Gfx::Pen&            textPen,
                              const Gfx::Font&            font
                                         ) const = 0;
};


class PT_FORMS_API SliderRenderer : public Style::Facet
{
    public:
        SliderRenderer(std::size_t refs = 0);

        virtual ~SliderRenderer();

        void prepare(  const Slider&        s,
                      const StyleOptions&  options,
                      Gfx::Brush&          background,
                      Gfx::Brush&          foreground,
                      Gfx::Pen&            contour,
                      Gfx::Pen&            textPen,
                      Gfx::Font&          font
                     ) const;

       void render( const Slider&        s,
                    const StyleOptions& options,
                    Painter&            painter,
                    const Gfx::RectF&    rect,
                    const Gfx::Brush&    background,
                    const Gfx::Brush&    foreground,
                    const Gfx::Pen&      contour,
                    const Gfx::Pen&      textPen,
                    const Gfx::Font&    font
                  ) const;

    protected:
        virtual void onPrepare( const Slider&        s,
                                const StyleOptions&  options,
                                Gfx::Brush&          background,
                                Gfx::Brush&          foreground,
                                Gfx::Pen&            contour,
                                Gfx::Pen&            textPen,
                                Gfx::Font&          font
                              ) const = 0;

        virtual void onRender( const Slider&         s,
                               const StyleOptions&   options,
                               Painter&               painter,
                               const Gfx::RectF&     rect,
                               const Gfx::Brush&     background,
                               const Gfx::Brush&     foreground,
                               const Gfx::Pen&       contour,
                               const Gfx::Pen&       textPen,
                               const Gfx::Font&       font
                             ) const = 0;
};


class PT_FORMS_API ListBoxRenderer : public Style::Facet
{
    public:
        ListBoxRenderer(std::size_t refs = 0);

        virtual ~ListBoxRenderer();

        void prepareLayout(Spacing& frameSize);

        void renderBackground(const ListBox& lb,
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Brush& brush) const;

        void renderFrame(const ListBox& lb,
                         const StyleOptions& options,
                         Painter& painter, 
                         const Gfx::RectF& rect, 
                         const Gfx::Pen& pen) const;

        void prepareItem(const ListBoxItem& item, 
                         const StyleOptions& options, 
                         Gfx::Brush& brush,
                         Gfx::Pen& contour,
                         Gfx::Font& font,
                         Gfx::Pen& textPen) const;

        void renderItem(const ListBoxItem& item, 
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        Gfx::Brush& brush,
                        Gfx::Pen& contour) const;

    protected:
        virtual void onPrepareLayout(Spacing& frameSize) = 0;

        virtual void onRenderBackground(const ListBox& lb,
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Brush& brush) const = 0;

        virtual void onRenderFrame(const ListBox& lb,
                                   const StyleOptions& options,
                                   Painter& painter, 
                                   const Gfx::RectF& rect, 
                                   const Gfx::Pen& pen) const = 0;

        virtual void onPrepareItem(const ListBoxItem& item, 
                                   const StyleOptions& options, 
                                   Gfx::Brush& brush,
                                   Gfx::Pen& contour,
                                   Gfx::Font& font,
                                   Gfx::Pen& textPen) const = 0;

        virtual void onRenderItem(const ListBoxItem& item, 
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  Gfx::Brush& brush,
                                  Gfx::Pen& contour) const = 0;
};


class PT_FORMS_API ComboBoxRenderer : public Style::Facet
{
    public:
        ComboBoxRenderer(std::size_t refs = 0);

        virtual ~ComboBoxRenderer();

        void prepare(const ComboBox& cb, 
                     const StyleOptions& options,
                     Gfx::Brush& background,
                     Gfx::Brush& foreground,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen) const;
        
        void prepareLayout(const ComboBox& cb,
                           Gfx::SizeF& buttonSize) const;
        
        void renderBackground(const ComboBox& cb, 
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Pen& contour,
                              const Gfx::Brush& brush) const;

        void renderButton(const ComboBox& cb, 
                          const StyleOptions& options,
                          Painter& painter, 
                          const Gfx::RectF& rect,
                          const Gfx::Pen& contour,
                          const Gfx::Brush& foreground) const;

        void renderText(const ComboBox& cb,
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen,
                        const Gfx::RectF& cursor) const;
    
    protected:
        virtual void onPrepare(const ComboBox& cb, 
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const = 0;

        virtual void onPrepareLayout(const ComboBox& cb,
                                     Gfx::SizeF& buttonSize) const = 0;

        virtual void onRenderBackground(const ComboBox& cb, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const = 0;

        virtual void onRenderButton(const ComboBox& cb, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::Pen& contour,
                                    const Gfx::Brush& foreground) const = 0;

        virtual void onRenderText(const ComboBox& cb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& cursor) const = 0;
};


class PT_FORMS_API SpinBoxRenderer : public Style::Facet
{
    public:
        SpinBoxRenderer(std::size_t refs = 0);

        virtual ~SpinBoxRenderer();

        void prepare(const SpinBox& sb, 
                     const StyleOptions& options,
                     Gfx::Brush& background,
                     Gfx::Pen& contour,
                     Gfx::Font& font,
                     Gfx::Pen& textPen) const;
        
        void prepareButton(const SpinBoxButton& sb, 
                           const StyleOptions& options,
                           Gfx::Brush& foreground,
                           Gfx::Pen& contour) const;

        void layout(const SpinBox& sb,
                    Gfx::RectF& downButton,
                    Gfx::RectF& upButton,
                    Gfx::RectF& textBox) const;
        
        void renderBackground(const SpinBox& sb, 
                              const StyleOptions& options,
                              Painter& painter, 
                              const Gfx::RectF& rect,
                              const Gfx::Pen& contour,
                              const Gfx::Brush& brush) const;

        void renderButton(const SpinBoxButton& sb, 
                          const StyleOptions& options,
                          Painter& painter, 
                          const Gfx::RectF& rect,
                          const Gfx::Brush& foreground,
                          const Gfx::Pen& contour) const;

        void renderText(const SpinBox& sb,
                        const StyleOptions& options,
                        Painter& painter, 
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen,
                        const Gfx::RectF& cursor) const;
    
    protected:
        virtual void onPrepare(const SpinBox& sb, 
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Pen& contour,
                               Gfx::Font& font,
                               Gfx::Pen& textPen) const = 0;

        virtual void onPrepareButton(const SpinBoxButton& sb, 
                                     const StyleOptions& options,
                                     Gfx::Brush& foreground,
                                     Gfx::Pen& contour) const = 0;

        virtual void onLayout(const SpinBox& sb,
                              Gfx::RectF& downButton,
                              Gfx::RectF& upButton,
                              Gfx::RectF& textBox) const = 0;

        virtual void onRenderBackground(const SpinBox& sb, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& contour,
                                        const Gfx::Brush& brush) const = 0;

        virtual void onRenderButton(const SpinBoxButton& sb, 
                                    const StyleOptions& options,
                                    Painter& painter, 
                                    const Gfx::RectF& rect,
                                    const Gfx::Brush& foreground,
                                    const Gfx::Pen& contour) const = 0;

        virtual void onRenderText(const SpinBox& sb,
                                  const StyleOptions& options,
                                  Painter& painter, 
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen,
                                  const Gfx::RectF& cursor) const = 0;
};


class PT_FORMS_API TabViewRenderer : public Style::Facet
{
    public:
        TabViewRenderer(std::size_t refs = 0);

        virtual ~TabViewRenderer();

        void prepare(const TabView& tv,
                     const StyleOptions& options,
                     Gfx::Brush& background,
                     Gfx::Brush& foreground,
                     Gfx::Pen& contour) const;

        void render(const TabView& tv,
                    const StyleOptions& options,
                    Painter& painter,
                    const Gfx::RectF& rect,
                    const Gfx::Brush& background,
                    const Gfx::Brush& foreground,
                    const Gfx::Pen& contour) const;

        Gfx::SizeF measureTabs(PaintSurface& surface,
                               const std::vector<TabItem>& tabs,
                               const Gfx::Font& font) const;

        void layoutTabs(PaintSurface& surface,
                        std::vector<TabItem>& tabs,
                        const Gfx::RectF& rect, 
                        const Gfx::Font& font) const;

        void prepareTabs(const TabBar& tabs,
                         const StyleOptions& options,
                         const Gfx::Brush& background,
                         const Gfx::Brush& foreground,
                         const Gfx::Pen& contour,
                         const Gfx::Font& font, 
                         const Gfx::Pen& textPen) const;

        void renderTabs(const std::vector<TabItem>& tabs,
                        const StyleOptions& options,
                        Painter& painter,
                        const Gfx::RectF& rect,
                        const Gfx::Brush& background,
                        const Gfx::Brush& foreground,
                        const Gfx::Pen& contour,
                        const Gfx::Font& font, 
                        const Gfx::Pen& textPen) const;
        
    protected:
        virtual void onPrepare(const TabView& tv,
                               const StyleOptions& options,
                               Gfx::Brush& background,
                               Gfx::Brush& foreground,
                               Gfx::Pen& contour) const = 0;

        virtual void onRender(const TabView& tv,
                              const StyleOptions& options,
                              Painter& painter,
                              const Gfx::RectF& rect,
                              const Gfx::Brush& background,
                              const Gfx::Brush& foreground,
                              const Gfx::Pen& contour) const = 0;

        virtual Gfx::SizeF onMeasureTabs(PaintSurface& surface,
                                         const std::vector<TabItem>& tabs,
                                         const Gfx::Font& font) const = 0;

        virtual void onLayoutTabs(PaintSurface& surface,
                                  std::vector<TabItem>& tabs,
                                  const Gfx::RectF& rect, 
                                  const Gfx::Font& font) const = 0;
        
        virtual void onPrepareTabs(const TabBar& tabs,
                                   const StyleOptions& options,
                                   const Gfx::Brush& background,
                                   const Gfx::Brush& foreground,
                                   const Gfx::Pen& contour,
                                   const Gfx::Font& font, 
                                   const Gfx::Pen& textPen) const = 0;

        virtual void onRenderTabs(const std::vector<TabItem>& tabs,
                                  const StyleOptions& options,
                                  Painter& painter,
                                  const Gfx::RectF& rect,
                                  const Gfx::Brush& background,
                                  const Gfx::Brush& foreground,
                                  const Gfx::Pen& contour,
                                  const Gfx::Font& font, 
                                  const Gfx::Pen& textPen) const = 0;
};

} // namespace

} // namespace

#endif
