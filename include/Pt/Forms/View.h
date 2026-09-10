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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_FORMS_VIEW_H
#define PT_FORMS_VIEW_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Forms/Style.h>

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

#include <cassert>

namespace Pt {

namespace Forms {

class Control;
class PaintContext;

/** @brief A widget that connects controls to a paint surface.

    A %View owns the boundary between a control hierarchy and the
    %PaintSurface on which it is displayed. It establishes the surface and
    coordinate mapping for attached controls, then forwards their repaint,
    layout, visibility, geometry, and activation requests. %Form and
    %Control provide the usual view implementations; derive a view when a
    custom content host needs different forwarding or coordinate behavior.

    The view owns its surface adapter, not the %PaintSurface assigned through
    %setSurface(). The adapter presents the view's local size, scaling, and
    offset within the borrowed surface. When a control is initialized, the
    view gives it that surface with an offset derived from the control's
    position; release clears the control's surface. %Form and %Control extend
    this mechanism to their content and child controls.

    %toControl() and %fromControl() convert between view-local and attached
    control coordinates. Their default mapping uses the control position.
    View implementations also align child move and resize requests to the
    active scaling before sending the corresponding Forms event. Override the
    protected mapping and forwarding hooks only when a custom host has a
    different coordinate system, surface arrangement, or request policy.

    %View is an implementation boundary rather than the usual application
    base class. Derive visible application content from %Control. A custom
    view must arrange a complete attach, initialize, release, and detach
    lifecycle for every hosted control and must not retain a surface supplied
    by another view beyond that lifecycle.

    @ingroup Pt-Forms-Application
*/
class PT_FORMS_API View : public Widget
{
    friend class Control;

    typedef Widget Base;

    public:
        /** @brief Defines whether a control can receive or retain focus.
        */
        enum FocusPolicy
        {
            /** @brief The control cannot receive focus.
            */
            NoFocus,

            /** @brief The control participates in normal focus navigation.
            */
            AcceptFocus,

            /** @brief The control retains focus during focus navigation.
            */
            KeepFocus
        };

    protected:
        /** @brief Creates a view for a derived content host.
        */
        View();

    public:
        /** @brief Destroys the view and its surface adapter.
        */
        virtual ~View();

        /** @brief Converts @a pos from this view to @a control coordinates.

            The default conversion subtracts the control position. Override
            %onToControl() when the hosted control uses a different coordinate
            mapping.
        */
        Gfx::PointF toControl(const Control& control,
                              const Gfx::PointF& pos) const;

        /** @brief Converts @a pos from @a control to this view coordinates.

            The default conversion adds the control position. Override
            %onFromControl() when the hosted control uses a different coordinate
            mapping.
        */
        Gfx::PointF fromControl(const Control& control,
                                const Gfx::PointF& pos) const;

        /** @brief Returns this view's local paint-surface adapter.

            The adapter exposes this view's size, scaling, and position on the
            surface assigned with %setSurface(). It remains available while no
            parent surface is assigned, but cannot then obtain a canvas.
        */
        PaintSurface& surface();

        /** @brief Returns this view's local paint-surface adapter.
        */
        const PaintSurface& surface() const;

        /** @brief Assigns the surface on which this view displays its controls.

            The view borrows @a surface and does not destroy it. Pass null to
            detach the view from its parent surface. @a pos identifies this
            view's origin in the assigned surface and is used to establish
            child-control surface positions.
        */
        void setSurface(PaintSurface* surface,
                        const Gfx::PointF& pos = Gfx::PointF() );

    protected:
        /** @brief Notifies the view after its surface assignment changes.

            Override to propagate @a surface and @a pos to hosted controls or
            to establish a custom surface arrangement.
        */
        virtual void onSetSurface(PaintSurface* surface,
                                  const Gfx::PointF& pos);

        /** @brief Paints this view into @a context for the local @a rect.

            The default implementation does not draw. Use this hook for custom
            view content rather than overriding %onPaintEvent().
        */
        virtual void onPaint(PaintContext& context,
                             const Gfx::RectF& rect);

    protected:
        /** @brief Notifies the view that @a control was attached.

            The base implementation does not retain the control. A custom host
            stores controls or updates its content relationship here.
        */
        virtual void onAttach(Control& control);

        /** @brief Notifies the view that @a control was detached.

            The base implementation does not manage control ownership or a
            child list.
        */
        virtual void onDetach(Control& control);

        /** @brief Initializes @a control after it has been attached.

            The base implementation assigns the current parent surface at the
            control position. An override that retains the standard surface
            mapping must call the base implementation.
        */
        virtual void onInit(Control& control);

        /** @brief Releases runtime resources associated with @a control.

            The base implementation removes the control's surface assignment.
            An override that used %onInit() must call the base implementation
            or provide an equivalent release operation.
        */
        virtual void onRelease(Control& control);

        /** @brief Converts @a pos from this view to @a control coordinates.
        */
        virtual Gfx::PointF onToControl(const Control& control,
                                        const Gfx::PointF& pos) const;

        /** @brief Converts @a pos from @a control to this view coordinates.
        */
        virtual Gfx::PointF onFromControl(const Control& control,
                                          const Gfx::PointF& pos) const;

    protected:
        /** @brief Receives a repaint request for @a rect in @a control coordinates.

            The base implementation discards the request. A content host
            converts and forwards the dirty region to its own repaint target.
        */
        virtual void onRepaintRequest(Control& control, const Gfx::RectF& rect);

        /** @brief Receives a request to recalculate @a control's layout.

            The base implementation discards the request.
        */
        virtual void onRelayoutRequest(Control& control);

        /** @brief Receives a request to enable or disable @a control.

            The base implementation discards the request.
        */
        virtual void onEnableRequest(Control& control, bool isEnable);

        /** @brief Receives a request to activate or deactivate @a control.

            The base implementation discards the request.
        */
        virtual void onActivateRequest(Control& control, bool active);

        /** @brief Receives a request to show or hide @a control.

            The base implementation discards the request.
        */
        virtual void onShowRequest(Control& control, bool isShown);

        /** @brief Receives a request to move @a control to @a pos.

            The base implementation aligns @a pos to the active scaling,
            updates the control surface position, and commits a %MoveEvent.
        */
        virtual void onMoveRequest(Control& control, const Gfx::PointF& pos);

        /** @brief Receives a request to resize @a control to @a size.

            The base implementation aligns @a size to the active scaling and
            commits a %ResizeEvent.
        */
        virtual void onResizeRequest(Control& control, const Gfx::SizeF& size);

        /** @brief Receives a request to raise @a control in its host.

            The base implementation discards the request.
        */
        virtual void onRaiseRequest(Control& control);

    //
    // Widget
    //
    protected:
        /** @brief Connects the view to @a screen.

            The base implementation connects the underlying widget.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Disconnects the view from its screen.

            The base implementation disconnects the underlying widget.
        */
        virtual void onDisconnect();


        /** @brief Processes a paint event and invokes %onPaint().

            The base implementation processes the widget event, then creates a
            %PaintContext using this view's surface adapter. Overrides that
            need the standard painting sequence must call the base method.
        */
        virtual void onPaintEvent(const PaintEvent& ev) override;

        /** @brief Repaints the union of the old and new view bounds.

            The base implementation then applies the move event to the widget.
        */
        virtual void onMoveEvent(const MoveEvent& ev) override;

        /** @brief Repaints the union of the old and new view bounds.

            The base implementation then applies the resize event to the
            widget.
        */
        virtual void onResizeEvent(const ResizeEvent& ev) override;

    private:
        class ViewSurface* _surface;
};

/** @internal @brief View paint surface
*/
class ViewSurface : public PaintSurface
{
    public:
        explicit ViewSurface(View& view)
        : _view(&view)
        , _surface(0)
        {
        }

        explicit ViewSurface(Widget& w)
        : _view(&w)
        , _surface(0)
        {
        }

        PaintSurface* surface()
        {
            return _surface;
        }

        const Gfx::PointF& position() const
        {
            return _position;
        }

        void setSurface(PaintSurface* surface,
                        const Gfx::PointF& pos)
        {
            _surface = surface;
            _position = pos;
        }

    protected:
        virtual void onDrawPixmap(Gfx::Canvas& canvas,
                                  const Gfx::PointF& to,
                                  const Pixmap& pm,
                                  const Gfx::RectF* rect) override;

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const override
        {
            if (_surface )
                return _surface->format();

            return Gfx::ImageFormat::argb32();
        }

        const Gfx::SizeF& onGetSize() const
        {
            return _view->size();
        }

        virtual const Gfx::Scaling& onGetScaling() const override
        {
            return _view->scaling();
        }

        virtual Gfx::Canvas* onGetCanvas(Gfx::Canvas* reuse) override
        {
            Gfx::Canvas* canvas = _surface ? _surface->getCanvas(reuse)
                                                  : 0;
            if( ! canvas )
                return canvas;

            Gfx::RectF region = canvas->region();
            region.move( _position.x(), _position.y() );
            region.setSize( _view->size() );

            canvas->setRegion(region);
            return canvas;
        }

        virtual Gfx::Canvas* onCreateCanvas(Gfx::Canvas* reuse) override
        {
            assert(false && "ViewSurface does not create its own canvas");
            return 0;
        }

        virtual void onReleaseCanvas() override
        {
            // context is released by parent surface
        }

        virtual void onSync() override
        {
            // sync is done by parent surface
        }

        virtual void onFinish() override
        {
            // sync is done by parent surface
        }

    private:
        Widget*          _view;
        PaintSurface*    _surface;
        Gfx::PointF      _position;
};

} // namespace

} // namespace

#endif // include guard