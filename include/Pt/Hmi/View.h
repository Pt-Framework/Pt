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

#ifndef PT_HMI_VIEWL_H
#define PT_HMI_VIEWL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// ViewCanvas
///////////////////////////////////////////////////////////////////////

class Widget;
class View;

class ViewCanvas : public Gfx::CanvasBase
{
    public:
        explicit ViewCanvas(View& view)
        : _view(&view)
        , _surface(0)
        {
        }

        Gfx::PaintSurface* surface()
        {
            return _surface;
        }

        const Gfx::PointF& position() const
        {
            return _position;
        } 

        void setSurface(Gfx::PaintSurface* surface, 
                        const Gfx::PointF& pos)
        {
            _surface = surface;
            _position = pos;
        }

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const;

        virtual const Gfx::SizeF& onGetSize() const;

        virtual const Gfx::Scaling& onGetScaling() const;

        virtual Gfx::PaintContext* onGetPaint(Gfx::PaintContext* context);

    private:
        View*                _view;
        Gfx::PaintSurface*   _surface;
        Gfx::PointF          _position;
};

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

class PT_HMI_API View : public Visual
                      , private Gfx::PaintSurface
{
    friend class Widget;

    typedef Visual Base;

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

        Gfx::PointF toWidget(const Widget& widget, 
                             const Gfx::PointF& pos) const;

        Gfx::PointF fromWidget(const Widget& widget,
                               const Gfx::PointF& pos) const;

        Gfx::PaintSurface& surface();

        const Gfx::PaintSurface& surface() const;

        void setSurface(Gfx::PaintSurface* surface, 
                        const Gfx::PointF& pos = Gfx::PointF() );

    protected:
        virtual void onSetSurface(Gfx::PaintSurface* surface, 
                                  const Gfx::PointF& pos);

        virtual void onPaint(Gfx::PaintSurface& surface, 
                             const Gfx::RectF& rect);
    
    protected:
        virtual void onAttach(Widget& widget);
        
        virtual void onDetach(Widget& widget);

        virtual void onInit(Widget& widget);

        virtual void onRelease(Widget& widget);

        virtual Gfx::PointF onToWidget(const Widget& widget, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWidget(const Widget& widget, 
                                         const Gfx::PointF& pos) const;

    protected:
        virtual void onRepaintRequest(Widget& widget, const Gfx::RectF& rect);

        virtual void onRelayoutRequest(Widget& widget);

        virtual void onEnableRequest(Widget& widget, bool isEnable);

        virtual void onActivateRequest(Widget& w, bool active);

        virtual void onShowRequest(Widget& widget, bool isShown);

        virtual void onMoveRequest(Widget& widget, const Gfx::PointF& pos);

        virtual void onResizeRequest(Widget& widget, const Gfx::SizeF& size);

        virtual void onRaiseRequest(Widget& widget);

    //
    // Visual
    //
    protected:
        virtual void onPaintEvent(const PaintEvent& ev) override;

        virtual void onMoveEvent(const MoveEvent& ev) override;

        virtual void onResizeEvent(const ResizeEvent& ev) override;

    private:
        ViewCanvas* _canvas;
};

} // namespace

} // namespace

#endif // include guard