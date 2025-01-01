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

#include <Pt/Hmi/View.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>

#include <Pt/Gfx/PaintContext.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// ViewCanvas
///////////////////////////////////////////////////////////////////////

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
        virtual const Gfx::ImageFormat& ViewCanvas::onGetFormat() const
        {
            if (_surface)
            {
                const CanvasBase* canvas = _surface->canvas();
                if (canvas)
                    return canvas->format();
            }

            return Gfx::ImageFormat::argb32();
        }

        virtual const Gfx::SizeF& ViewCanvas::onGetSize() const
        {
            return _view->size();
        }

        virtual const Gfx::Scaling& ViewCanvas::onGetScaling() const
        {
            return _view->scaling();
        }

        virtual Gfx::PaintContext* ViewCanvas::onGetPaint(Gfx::PaintContext* context)
        {
            Gfx::PaintContext* paintContext = _surface ? _surface->getPaint(context)
                                                       : 0;
            if (paintContext)
            {
                Gfx::RectF r = paintContext->region();

                r.shift(position().x(), position().y());
                r.setSize(this->size());

                paintContext->setRegion(r);
            }

            return paintContext;
        }

    private:
        View*                _view;
        Gfx::PaintSurface*   _surface;
        Gfx::PointF          _position;
};

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

View::View()
: _canvas(0)
{
    _canvas = new ViewCanvas(*this);
    setCanvas(_canvas);
}


View::~View()
{
    delete _canvas;
}


Gfx::PaintSurface& View::surface()
{
    return *this;
}


const Gfx::PaintSurface& View::surface() const
{
    return *this;
}


void View::setSurface(Gfx::PaintSurface* surface, 
                      const Gfx::PointF& pos)
{
    _canvas->setSurface(surface, pos);

    onSetSurface(surface, pos);
}


void View::onSetSurface(Gfx::PaintSurface* surface, 
                        const Gfx::PointF& pos)
{
}


Gfx::PointF View::toWidget(const Widget& widget, 
                           const Gfx::PointF& pos) const
{ 
    return onToWidget(widget, pos); 
}


Gfx::PointF View::fromWidget(const Widget& widget,
                             const Gfx::PointF& pos) const
{ 
    return onFromWidget(widget, pos);
}


void View::setStyleOptions(const StyleOptions& o)
{
    onSetStyleOptions(o);
    invalidate();
}


void View::onSetStyleOptions(const StyleOptions& o)
{
}

//
// TODO: maintain a list of child widgets in onAttach() and onDetach(). 
//       This way View::onSetSurface() can setup client surfaces.
//

void View::onAttach(Widget& widget)
{
}


void View::onDetach(Widget& widget)
{
}


void View::onInit(Widget& widget)
{
    Gfx::PaintSurface* surface = _canvas->surface();
    Gfx::PointF surfacePos = _canvas->position() + widget.position();

    widget.setSurface(surface, surfacePos);
}


void View::onRelease(Widget& widget)
{
    widget.setSurface(0);
}


Gfx::PointF View::onToWidget(const Widget& widget, 
                             const Gfx::PointF& pos) const
{
    return pos - widget.position();
}


Gfx::PointF View::onFromWidget(const Widget& widget, 
                               const Gfx::PointF& pos) const
{
    return pos + widget.position();
}


void View::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    onPaint( *this, ev.rect() );
}


void View::onPaint(Gfx::PaintSurface&, const Gfx::RectF&)
{
}


void View::onRepaintRequest(Widget& widget, const Gfx::RectF& rect) 
{
}


void View::onRelayoutRequest(Widget& widget)
{
}


void View::onEnableRequest(Widget& widget, bool isEnable)
{
}


void View::onActivateRequest(Widget& w, bool active)
{
}


void View::onShowRequest(Widget& widget, bool isShown)
{
}


void View::onRaiseRequest(Widget& widget)
{
}


void View::onMoveRequest(Widget& widget, const Gfx::PointF& pos)
{
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = scaling().align(pos);

    //
    // update client surface
    //
    Gfx::PaintSurface* surface = _canvas->surface();
    Gfx::PointF surfacePos = _canvas->position() + aligedPos;

    widget.setSurface(surface, surfacePos);

    //
    // send move event
    //
    MoveEvent mev(widget, aligedPos);
    Application::instance().commitEvent(mev);
}


void View::onMoveEvent(const MoveEvent& ev)
{
    if( position() == ev.position() )
        return;

    Gfx::PointF delta = ev.position() - position();
    
    Gfx::RectF updateRect( size() );
    updateRect.unify( Gfx::RectF(delta, size()) );
    repaint(updateRect);

    Base::onMoveEvent(ev);
}


void View::onResizeRequest(Widget& widget, const Gfx::SizeF& size)
{
    Gfx::SizeF alignedSize = scaling().align(size);

    //_info.resize( ev.size() );

    ResizeEvent rev(widget, alignedSize);
    Application::instance().commitEvent(rev);
}


void View::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    //std::clog << "RESIZE: " << name() << ev.size().width() << std::endl;

    Gfx::RectF updateRect( size() );
    updateRect.unify( Gfx::RectF( ev.size() ) );
    repaint(updateRect);

    Base::onResizeEvent(ev);
}

} // namespace

} // namespace
