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

#ifndef Pt_Forms_Style_h
#define Pt_Forms_Style_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Alignment.h>
#include <Pt/Forms/Direction.h>
#include <Pt/Forms/Spacing.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Forms/StyleOptions.h>
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
#include <Pt/SmartPtr.h>

#include <map>
#include <cstddef>

namespace Pt {

namespace Forms {

class PaintContext;
class Painter;
class Pixmap;
class TextBlock;
class PushButton;
class Menu;
class MenuItem;
class MenuBar;
class MenuBarItem;
class ProgressBar;

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

/** @brief Style for widgets.
 */
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

                /** @brief Resets the facet to the given global style options.
                */
                void reset(const StyleOptions& options)
                {
                    onReset(options);
                }

            protected:
                /** @brief Hook for facet-specific global reset logic.
                */
                virtual void onReset(const StyleOptions& /*options*/)
                {
                }

            private:
                const std::type_info* _typeId;
                std::size_t _refs;
        };

    public:
        /** @brief Constructs an empty style.
        */
        Style();

        /** @brief Constructs a style by copying another style.
        */
        Style(const Style& style);

        /** @brief Destroys the style and releases all registered facets.
        */
        virtual ~Style();

        /** @brief Assigns the contents of another style.
        */
        Style& operator=(const Style& style);

        /** @brief Replaces all facets with those from another style.
        */
        void assign(const Style& style);

        /** @brief Replaces the current facets with those from another style.
        */
        void combine(const Style& style);

        /** @brief Registers or replaces a facet.
        */
        void set(Facet* facet);

        /** @brief Resets all registered facets to the given global style options.
        */
        void reset(const StyleOptions& options);

        /** @brief Returns the current style generation.

            The generation changes whenever the style content of this instance
            changes and can be used to detect when a widget must rebind to a
            different renderer source.
        */
        std::size_t generation() const
        {
            return _generation;
        }

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
        std::size_t _generation;
};


/** @brief Common renderer-binding controller for %Forms style slices.

    Manages the active renderer, binding mode, and generation counters
    needed for local prepare bookkeeping. All %Forms style controllers
    that follow the extracted-slice pattern derive from this template.

    @tparam RendererT     Concrete %Style::Facet renderer type. Must expose
                          %create() and %prepare(const StyleOptions&, const OptionsT&).
    @tparam OptionsT      Widget-local style options type. Must expose
                          %hasOverrides() and %generation() const.
*/
template <typename RendererT,
          typename OptionsT>
class StyleBinder : private NonCopyable
{
    public:
        /** @brief Constructs an unbound style controller.
        */
        StyleBinder();

        /** @brief Returns true if a renderer is currently bound.
        */
        bool isBound() const;

        /** @brief Returns true if a custom renderer is currently bound.
        */
        bool isCustom() const;

        /** @brief Binds to the current style renderer path.

            Selects either the shared style renderer (%SharedRenderer) or a
            private override clone (%CustomOverrides). Shared style renderers
            are synchronized through %Style::reset() and are therefore not
            prepared locally here. Private override clones are prepared
            immediately with the current style and local options. This call
            always leaves custom renderer mode.
        */
        RendererT* bind(const Pt::Forms::Style& style,
                        const StyleOptions& options,
                        const OptionsT& localOptions);

        /** @brief Binds to an externally assigned renderer.

            Assigns a custom renderer explicitly and prepares it with the
            current options.
        */
        RendererT* bind(RendererT& renderer,
                        const StyleOptions& options,
                        const OptionsT& localOptions);

        /** @brief Refreshes the current renderer binding.

            Re-prepares the current custom renderer when the local prepare
            inputs changed. Shared style renderers and override clones are
            reacquired through the current %Style by delegating to the
            style-path bind.
        */
        RendererT* rebind(const Pt::Forms::Style& style,
                          const StyleOptions& options,
                          const OptionsT& localOptions);

        /** @brief Returns the currently bound renderer or 0 if none is available.
        */
        RendererT* renderer();

        /** @brief Returns the currently bound renderer or 0 if none is available.
        */
        const RendererT* renderer() const;

    protected:
        enum Binding
        {
            SharedRenderer,
            CustomOverrides,
            CustomRenderer
        };

    private:
        FacetPtr<RendererT> _renderer;
        Binding             _binding;
        std::size_t         _boundStyleGeneration;
        std::size_t         _styleOptionsGeneration;
        std::size_t         _localOptionsGeneration;
};


template <typename RendererT,
          typename OptionsT>
StyleBinder<RendererT, OptionsT>::StyleBinder()
: _binding(SharedRenderer)
, _boundStyleGeneration( std::size_t(-1) )
, _styleOptionsGeneration( std::size_t(-1) )
, _localOptionsGeneration( std::size_t(-1) )
{
}


template <typename RendererT,
          typename OptionsT>
bool StyleBinder<RendererT, OptionsT>::isBound() const
{
    return _renderer != 0;
}


template <typename RendererT,
          typename OptionsT>
bool StyleBinder<RendererT, OptionsT>::isCustom() const
{
    return _binding == CustomRenderer;
}


template <typename RendererT,
          typename OptionsT>
RendererT*
StyleBinder<RendererT, OptionsT>::bind(const Pt::Forms::Style& style,
                                       const StyleOptions& options,
                                       const OptionsT& localOptions)
{
    _styleOptionsGeneration = std::size_t(-1);
    _localOptionsGeneration = std::size_t(-1);

    if( localOptions.hasOverrides() )
    {
        RendererT* renderer = style.get<RendererT>();
        if( renderer )
            renderer = renderer->create();

        _renderer.reset(renderer);
        _binding = CustomOverrides;

        if( _renderer )
        {
            _renderer->prepare(options, localOptions);
            _styleOptionsGeneration = options.generation();
            _localOptionsGeneration = localOptions.generation();
        }
    }
    else
    {
        _renderer.reset( style.get<RendererT>() );
        _binding = SharedRenderer;
    }

    _boundStyleGeneration = style.generation();

    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
RendererT*
StyleBinder<RendererT, OptionsT>::bind(RendererT& renderer,
                                       const StyleOptions& options,
                                       const OptionsT& localOptions)
{
    _renderer.reset(&renderer);

    _binding = CustomRenderer;
    _boundStyleGeneration = std::size_t(-1);
    _styleOptionsGeneration = std::size_t(-1);
    _localOptionsGeneration = std::size_t(-1);

    _renderer->prepare(options, localOptions);

    _styleOptionsGeneration = options.generation();
    _localOptionsGeneration = localOptions.generation();

    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
RendererT*
StyleBinder<RendererT, OptionsT>::rebind(const Pt::Forms::Style& style,
                                         const StyleOptions& options,
                                         const OptionsT& localOptions)
{
    if( ! _renderer || _binding != CustomRenderer )
        return bind(style, options, localOptions);

    bool needsPrepare = _styleOptionsGeneration != options.generation() ||
                        _localOptionsGeneration != localOptions.generation();

    if( needsPrepare )
        _renderer->prepare(options, localOptions);

    _styleOptionsGeneration = options.generation();
    _localOptionsGeneration = localOptions.generation();

    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
RendererT* StyleBinder<RendererT, OptionsT>::renderer()
{
    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
const RendererT* StyleBinder<RendererT, OptionsT>::renderer() const
{
    return _renderer.get();
}


class PT_FORMS_API LineEditRenderer : public Style::Facet
{
    public:
        explicit LineEditRenderer(std::size_t refs = 0);

        virtual ~LineEditRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        LineEditRenderer* create() const;

    public:
        /** @brief Returns the background brush for the entry area.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the background brush for the entry area.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the contour pen for the entry border.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the pen for the entry border.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Returns the font used for text rendering.
        */
        const Gfx::Font& font() const;

        /** @brief Sets the font for text rendering.
        */
        void setFont(const Gfx::Font& f);

        /** @brief Returns the text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the text color from a pen.
        */
        void setTextColor(const Gfx::Pen& p);

        /** @brief Returns the selection background brush, or null if unset.
        */
        const Gfx::Brush* selectionBackground() const;

        /** @brief Sets the background brush for selected text.
        */
        void setSelectionBackground(const Gfx::Brush& b);

        /** @brief Returns the selection text color pen, or null if unset.
        */
        const Gfx::Pen* selectionTextColor() const;

        /** @brief Sets the text color pen for selected text.
        */
        void setSelectionTextColor(const Gfx::Pen& p);

    public:
        /** @brief Returns the total widget size for the given contentSize hint.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the inner text rectangle for the given widget rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Returns a painter with the current font applied for surface.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints the complete line edit.

            The default implementation calls onRenderEntry,
            onRenderSelection, onRenderText, and onRenderCursor.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const Gfx::RectF& cursor,
                          const Gfx::RectF& selection,
                          LineEditStyleFlags state);

        /** @brief Paints only the background and border frame.

            Useful for custom widgets that reuse the line-edit box appearance
            (e.g. IP editors, search boxes).
        */
        void renderEntry(PaintContext& context,
                              const Gfx::RectF& rect,
                              LineEditStyleFlags state);

        /** @brief Paints the selection highlight behind selected text.
        */
        void renderSelection(PaintContext& context,
                             const Gfx::RectF& textRect,
                             const Gfx::RectF& selection,
                             LineEditStyleFlags state);

        /** @brief Paints only the text content.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        LineEditStyleFlags state);

        /** @brief Paints the cursor caret.
        */
        void renderCursor(PaintContext& context,
                          const Gfx::RectF& textRect,
                          const Gfx::RectF& cursor,
                          LineEditStyleFlags state);

    protected:
        const StyleOptions& prepare();

        virtual LineEditRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints the entire line edit as one integrated element.

            The default implementation decomposes into onRenderEntry,
            onRenderSelection, onRenderText, and onRenderCursor.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& textRect,
                                    const StyleOptions& options,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const Gfx::RectF& cursor,
                                    const Gfx::RectF& selection,
                                    LineEditStyleFlags state);

        virtual void onRenderEntry(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        LineEditStyleFlags state) = 0;

        virtual void onRenderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const StyleOptions& options,
                                       const Gfx::RectF& selection,
                                       LineEditStyleFlags state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  LineEditStyleFlags state) = 0;

        virtual void onRenderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const StyleOptions& options,
                                    const Gfx::RectF& cursor,
                                    LineEditStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        AutoPtr<Gfx::Brush> _selectionBackground;
        AutoPtr<Gfx::Pen>   _selectionTextColor;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API ProgressBarRenderer : public Style::Facet
{
    public:
        explicit ProgressBarRenderer(std::size_t refs = 0);

        virtual ~ProgressBarRenderer();

        ProgressBarRenderer* create() const;

    public:
        const Gfx::Brush& background() const;
        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;
        void setContour(const Gfx::Pen& p);

        const Gfx::Brush& foreground() const;
        void setForeground(const Gfx::Brush& b);

        const Gfx::Font& font() const;
        void setFont(const Gfx::Font& f);

        const Gfx::Color& textColor() const;
        void setTextColor(const Gfx::Pen& p);

    public:
        /** @brief Returns total outer size based on the layouted content constraints.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface, const Gfx::SizeF& contentSize);

        /** @brief Returns the intrinsic total size of the empty progress bar (without text).
        */
        Gfx::SizeF measureBar(PaintSurface& surface);

        /** @brief Partitions the full widget rect into the general bar area and the text area.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& barSize,
                          const Gfx::SizeF& textSize,
                          Gfx::RectF& barRect,
                          Gfx::RectF& textRect);

        /** @brief Evaluates the precise sub-rectangles for track and chunk based on the progress.
            @param progressRatio Normalized value [0.0 - 1.0] representing the progress.
        */
        void layoutBar(PaintSurface& surface,
                       const Gfx::RectF& barRect,
                       float progressRatio,
                       Gfx::RectF& trackRect,
                       Gfx::RectF& chunkRect);

        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints the frame chrome: bar and text. Orchestrates text color inversion. */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& chunkRect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          ProgressBarStyleFlags state);

        /** @brief Paints the progress bar (track + chunk combined) for native themes. */
        void renderBar(PaintContext& context,
                       const Gfx::RectF& trackRect,
                       const Gfx::RectF& chunkRect,
                       ProgressBarStyleFlags state);

        /** @brief Paints only the empty background track. */
        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         ProgressBarStyleFlags state);

        /** @brief Paints only the colored progress chunk. */
        void renderChunk(PaintContext& context,
                         const Gfx::RectF& chunkRect,
                         ProgressBarStyleFlags state);

        /** @brief Paints only the text content. Requires chunkRect to handle potential text color inversion. */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const Gfx::RectF& chunkRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        ProgressBarStyleFlags state);

    protected:
        const StyleOptions& prepare();

        virtual ProgressBarRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureBar(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    const Gfx::SizeF& barSize,
                                    const Gfx::SizeF& textSize,
                                    Gfx::RectF& barRect,
                                    Gfx::RectF& textRect) = 0;

        virtual void onLayoutBar(PaintSurface& surface,
                                 const Gfx::RectF& barRect,
                                 float progressRatio,
                                 Gfx::RectF& trackRect,
                                 Gfx::RectF& chunkRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const StyleOptions& options,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& chunkRect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    ProgressBarStyleFlags state);

        virtual void onRenderBar(PaintContext& context,
                                 const Gfx::RectF& trackRect,
                                 const Gfx::RectF& chunkRect,
                                 const StyleOptions& options,
                                 ProgressBarStyleFlags state);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const StyleOptions& options,
                                   ProgressBarStyleFlags state) = 0;

        virtual void onRenderChunk(PaintContext& context,
                                   const Gfx::RectF& chunkRect,
                                   const StyleOptions& options,
                                   ProgressBarStyleFlags state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const Gfx::RectF& chunkRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  ProgressBarStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Brush> _foreground;
        Gfx::Brush           _defaultForeground;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API ScrollBarRenderer : public Style::Facet
{
    public:
        explicit ScrollBarRenderer(std::size_t refs = 0);

        virtual ~ScrollBarRenderer();

        ScrollBarRenderer* create() const;

    public:
        const Gfx::Brush& background() const;
        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;
        void setContour(const Gfx::Pen& p);

        const Gfx::Brush& foreground() const;
        void setForeground(const Gfx::Brush& b);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize,
                                Direction direction);

        Gfx::SizeF measureTrack(PaintSurface& surface,
                                 Direction direction);

        Gfx::SizeF measureHandle(PaintSurface& surface,
                                  Direction direction);

        Gfx::SizeF measureButton(PaintSurface& surface,
                                  Direction direction);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::SizeF& buttonSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& decreaseRect,
                          Gfx::RectF& increaseRect);

        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          Direction direction,
                          float fraction,
                          float viewProportion,
                          Gfx::RectF& handleRect);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const Gfx::RectF& decreaseRect,
                          const Gfx::RectF& increaseRect,
                          ScrollBarStyleFlags state,
                          ButtonStyleFlags decreaseState,
                          ButtonStyleFlags increaseState);

        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         Direction direction,
                         ScrollBarStyleFlags state);

        void renderHandle(PaintContext& context,
                          const Gfx::RectF& handleRect,
                          Direction direction,
                          ScrollBarStyleFlags state);

        void renderDecreaseButton(PaintContext& context,
                                  const Gfx::RectF& buttonRect,
                                  Direction direction,
                                  ScrollBarStyleFlags state,
                                  ButtonStyleFlags buttonState);

        void renderIncreaseButton(PaintContext& context,
                                  const Gfx::RectF& buttonRect,
                                  Direction direction,
                                  ScrollBarStyleFlags state,
                                  ButtonStyleFlags buttonState);

    protected:
        const StyleOptions& prepare();

        virtual ScrollBarRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize,
                                          Direction direction) = 0;

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface,
                                           Direction direction) = 0;

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface,
                                            Direction direction) = 0;

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface,
                                            Direction direction) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::SizeF& buttonSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& decreaseRect,
                                    Gfx::RectF& increaseRect) = 0;

        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    Direction direction,
                                    float fraction,
                                    float viewProportion,
                                    Gfx::RectF& handleRect) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const StyleOptions& options,
                                    Direction direction,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const Gfx::RectF& decreaseRect,
                                    const Gfx::RectF& increaseRect,
                                    ScrollBarStyleFlags state,
                                    ButtonStyleFlags decreaseState,
                                    ButtonStyleFlags increaseState);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const StyleOptions& options,
                                   Direction direction,
                                   ScrollBarStyleFlags state) = 0;

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const StyleOptions& options,
                                    Direction direction,
                                    ScrollBarStyleFlags state) = 0;

        virtual void onRenderDecreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            const StyleOptions& options,
                                            Direction direction,
                                            ScrollBarStyleFlags state,
                                            ButtonStyleFlags buttonState) = 0;

        virtual void onRenderIncreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            const StyleOptions& options,
                                            Direction direction,
                                            ScrollBarStyleFlags state,
                                            ButtonStyleFlags buttonState) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Brush> _foreground;
        Gfx::Brush           _defaultForeground;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API ListBoxRenderer : public Style::Facet
{
    public:
        explicit ListBoxRenderer(std::size_t refs = 0);

        virtual ~ListBoxRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ListBoxRenderer* create() const;

    public:
        /** @brief Returns the custom background brush, or null if unset.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the background brush for the list box area.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the custom contour pen, or null if unset.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the pen for the list box border.
        */
        void setContour(const Gfx::Pen& p);

    public:
        /** @brief Returns the outer size including chrome for the given contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the content rectangle within the outer widget rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Paints the list box background within the widget rect.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              ListBoxStyleFlags state);

        /** @brief Paints the chrome border within the outer widget rect.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          ListBoxStyleFlags state);

    protected:
        const StyleOptions& prepare();

        virtual ListBoxRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                          const Gfx::RectF& rect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        ListBoxStyleFlags state) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const StyleOptions& options,
                                    ListBoxStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        std::size_t          _styleGeneration;
};


/** @brief Renders the visual appearance of a list item.

    Provides rendering primitives for item backgrounds, text, and icons.
    Widgets pass pre-calculated bounds and typed flags; the renderer never
    sees widget internals.
*/
class PT_FORMS_API ListItemRenderer : public Style::Facet
{
    public:
        explicit ListItemRenderer(std::size_t refs = 0);

        virtual ~ListItemRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ListItemRenderer* create() const;

    public:
        /** @brief Returns the custom background brush, or null if unset.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the background brush for normal item state.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the custom foreground brush, or null if unset.
        */
        const Gfx::Brush* foreground() const;

        /** @brief Sets the foreground brush for highlighted/selected state.
        */
        void setForeground(const Gfx::Brush& b);

        /** @brief Returns the font used for item text.
        */
        const Gfx::Font& font() const;

        /** @brief Sets the font for item text.
        */
        void setFont(const Gfx::Font& f);

        /** @brief Returns the text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the text color from a pen.
        */
        void setTextColor(const Gfx::Pen& p);

    public:
        /** @brief Returns the combined content size for icon and text.
        */
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize);

        /** @brief Returns the outer size including chrome for the given contentSize on surface.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns a painter with the current font and text color applied.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Returns the inner content rectangle within the outer widget rect on surface.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Partitions the content rect into icon and text sub-rectangles.
        */
        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect);

        /** @brief Paints the item background fill within the item rect.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              ListItemStyleFlags state);

        /** @brief Draws text at pos, clipped to textRect.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& pos,
                        ListItemStyleFlags state);

        /** @brief Draws picture at pos, clipped to iconRect.
        */
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& iconRect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        ListItemStyleFlags state);

    protected:
        const StyleOptions& prepare();

        virtual ListItemRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        ListItemStyleFlags state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state) = 0;

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const StyleOptions& options,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Brush> _foreground;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API ComboBoxRenderer : public Style::Facet
{
    public:
        explicit ComboBoxRenderer(std::size_t refs = 0);

        virtual ~ComboBoxRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ComboBoxRenderer* create() const;

    public:
        /** @brief Returns the background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the background brush for the entry area.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the pen for entry and button borders.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Returns the foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the foreground brush for the button indicator.
        */
        void setForeground(const Gfx::Brush& b);

        /** @brief Returns the font used for text rendering.
        */
        const Gfx::Font& font() const;

        /** @brief Sets the font for text rendering.
        */
        void setFont(const Gfx::Font& f);

        /** @brief Returns the text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the text color from a pen.
        */
        void setTextColor(const Gfx::Pen& p);

    public:
        /** @brief Returns the total widget size for the given contentSize hint.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                 const Gfx::SizeF& contentSize);

        /** @brief Returns the preferred size of the drop button.
        */
        Gfx::SizeF measureButton(PaintSurface& surface);

        /** @brief Partitions the widget bounding rect into sub-rectangles.

            The returned textRect is the usable area for text rendering.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& buttonRect,
                          Gfx::RectF& textRect);

        /** @brief Returns a painter with the current font applied for surface.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints the frame chrome: entry border and drop button.

            The default implementation calls onRenderEntry and onRenderButton.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& buttonRect,
                          ComboBoxStyleFlags state,
                          ButtonStyleFlags buttonState);

        /** @brief Paints the drop button.

            Public primitive for custom widgets that want to reuse the
            combo-box drop button appearance.
        */
        void renderButton(PaintContext& context,
                          const Gfx::RectF& buttonRect,
                          ComboBoxStyleFlags state,
                          ButtonStyleFlags buttonState);

        /** @brief Paints the text and cursor within the text area.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        ComboBoxStyleFlags state);

    protected:
        const StyleOptions& prepare();

        virtual ComboBoxRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& buttonRect,
                                    Gfx::RectF& textRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints the entire frame as one integrated element.

            The default implementation calls onRenderEntry then onRenderButton.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    const StyleOptions& options,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const StyleOptions& options,
                                   ComboBoxStyleFlags state) = 0;

        virtual void onRenderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    const StyleOptions& options,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const StyleOptions& options,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  ComboBoxStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Brush> _foreground;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        std::size_t          _styleGeneration;
};


class PT_FORMS_API TabViewRenderer : public Style::Facet
{
    public:
        explicit TabViewRenderer(std::size_t refs = 0);

        virtual ~TabViewRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        TabViewRenderer* create() const;

    public:
        /** @brief Returns the background brush, or null if unset.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the background brush for the content panel.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the contour pen, or null if unset.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the contour pen for the content panel border.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Returns the font used for tab labels.
        */
        const Gfx::Font& font() const;

        /** @brief Sets the font for tab labels.
        */
        void setFont(const Gfx::Font& f);

        /** @brief Returns the text color for tab labels.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the text color for tab labels.
        */
        void setTextColor(const Gfx::Pen& p);

    public:
        /** @brief Measures one tab for the given text.
        */
        Gfx::SizeF measureTab(PaintSurface& surface,
                              const Pt::String& text);

        /** @brief Returns the text rect within a tab cell.
        */
        Gfx::RectF layoutTab(PaintSurface& surface,
                             const Gfx::RectF& tabRect);

        /** @brief Returns a painter configured for tab label text.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Renders the content panel background.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& contentRect,
                              TabViewStyleFlags state);

        /** @brief Renders the content panel chrome possibly accounting for the active tab.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& contentRect,
                          const Gfx::RectF& activeTabRect,
                          TabViewStyleFlags state);

        /** @brief Renders a single tab.
        */
        void renderTab(PaintContext& context,
                       const Gfx::RectF& tabRect,
                       const Pt::String& text,
                       const Gfx::PointF& textPos,
                       TabItemStyleFlags state);

    protected:
        void prepare();

        virtual TabViewRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureTab(PaintSurface& surface,
                                         const Pt::String& text) = 0;

        virtual Gfx::RectF onLayoutTab(PaintSurface& surface,
                                        const Gfx::RectF& tabRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& contentRect,
                                        const StyleOptions& options,
                                        TabViewStyleFlags state) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& contentRect,
                                    const Gfx::RectF& activeTabRect,
                                    const StyleOptions& options,
                                    TabViewStyleFlags state) = 0;

        virtual void onRenderTab(PaintContext& context,
                                 const Gfx::RectF& tabRect,
                                 const Pt::String& text,
                                 const Gfx::PointF& textPos,
                                 const StyleOptions& options,
                                 TabItemStyleFlags state) = 0;

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Font>  _font;
        AutoPtr<Gfx::Pen>   _textColor;
        std::size_t          _styleGeneration;
};

} // namespace

} // namespace

#endif
