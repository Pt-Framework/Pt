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

namespace Pt {

namespace Forms {

class Control;

class PT_FORMS_API View : public Widget
{
    friend class Control;

    typedef Widget Base;

    public:
        enum FocusPolicy
        {
            NoFocus,
            AcceptFocus,
            KeepFocus
        };
    
    protected:
        View();
    
    public:
        virtual ~View();

        Gfx::PointF toControl(const Control& control, 
                              const Gfx::PointF& pos) const;

        Gfx::PointF fromControl(const Control& control,
                                const Gfx::PointF& pos) const;

        void setStyleOptions(const StyleOptions& opts);

        PaintSurface& surface();

        const PaintSurface& surface() const;

        void setSurface(PaintSurface* surface, 
                        const Gfx::PointF& pos = Gfx::PointF() );

    protected:
        virtual void onSetSurface(PaintSurface* surface, 
                                  const Gfx::PointF& pos);

        virtual void onPaint(PaintSurface& surface, 
                             const Gfx::RectF& rect);
    
    protected:
        virtual void onAttach(Control& control);
        
        virtual void onDetach(Control& control);

        virtual void onInit(Control& control);

        virtual void onRelease(Control& control);

        virtual Gfx::PointF onToControl(const Control& control, 
                                        const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromControl(const Control& control, 
                                          const Gfx::PointF& pos) const;

        virtual void onSetStyleOptions(const StyleOptions& o);

    protected:
        virtual void onRepaintRequest(Control& control, const Gfx::RectF& rect);

        virtual void onRelayoutRequest(Control& control);

        virtual void onEnableRequest(Control& control, bool isEnable);

        virtual void onActivateRequest(Control& control, bool active);

        virtual void onShowRequest(Control& control, bool isShown);

        virtual void onMoveRequest(Control& control, const Gfx::PointF& pos);

        virtual void onResizeRequest(Control& control, const Gfx::SizeF& size);

        virtual void onRaiseRequest(Control& control);

    //
    // Widget
    //
    protected:
        virtual void onConnect(Screen& screen);

        virtual void onDisconnect();

        
        virtual void onPaintEvent(const PaintEvent& ev) override;

        virtual void onMoveEvent(const MoveEvent& ev) override;

        virtual void onResizeEvent(const ResizeEvent& ev) override;

    private:
        class ViewSurface* _surface;
};

/** @internal
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
        virtual void onDrawPixmap(const Gfx::PointF& to,
                                  const Pixmap& pm,
                                  const Gfx::Paint& paint,
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
            region.shift( _position.x(), _position.y() );
            region.setSize( _view->size() );

            canvas->setRegion(region);
            return canvas;
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