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

#include <Pt/Forms/View.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Application.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// ViewSurface
///////////////////////////////////////////////////////////////////////

void ViewSurface::onDrawPixmap(Gfx::Canvas& canvas,
                               const Gfx::PointF& to,
                               const Pixmap& pixmap,
                               const Gfx::RectF* rect)
{
    if( ! _surface )
        return;

    // no position offset needed — the canvas region already
    // contains the view translation set in onGetCanvas
    _surface->drawPixmap(canvas, to, pixmap, rect);
}

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

View::View()
: _surface(0)
{
    _surface = new ViewSurface(*this);
}


View::~View()
{
    delete _surface;
}


PaintSurface& View::surface()
{
    return *_surface;
}


const PaintSurface& View::surface() const
{
    return *_surface;
}


void View::setSurface(PaintSurface* surface, 
                      const Gfx::PointF& pos)
{
    _surface->setSurface(surface, pos);

    onSetSurface(surface, pos);
}


void View::onSetSurface(PaintSurface* surface, 
                        const Gfx::PointF& pos)
{
}


Gfx::PointF View::toControl(const Control& control, 
                           const Gfx::PointF& pos) const
{ 
    return onToControl(control, pos); 
}


Gfx::PointF View::fromControl(const Control& control,
                             const Gfx::PointF& pos) const
{ 
    return onFromControl(control, pos);
}

//
// TODO: maintain a list of child controls in onAttach() and onDetach(). 
//       This way View::onSetSurface() can setup client surfaces.
//

void View::onAttach(Control& control)
{
}


void View::onDetach(Control& control)
{
}


void View::onInit(Control& control)
{
    PaintSurface* surface = _surface->surface();
    Gfx::PointF surfacePos = _surface->position() + control.position();

    control.setSurface(surface, surfacePos);
}


void View::onRelease(Control& control)
{
    control.setSurface(0);
}


Gfx::PointF View::onToControl(const Control& control, 
                             const Gfx::PointF& pos) const
{
    return pos - control.position();
}


Gfx::PointF View::onFromControl(const Control& control, 
                               const Gfx::PointF& pos) const
{
    return pos + control.position();
}


void View::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void View::onDisconnect()
{
    Base::onDisconnect();
}


void View::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    PaintContext ctx( *_surface );
    onPaint( ctx, ev.rect() );
}


void View::onPaint(PaintContext&, const Gfx::RectF&)
{
}


void View::onRepaintRequest(Control& control, const Gfx::RectF& rect) 
{
}


void View::onRelayoutRequest(Control& control)
{
}


void View::onEnableRequest(Control& control, bool isEnable)
{
}


void View::onActivateRequest(Control& control, bool active)
{
}


void View::onShowRequest(Control& control, bool isShown)
{
}


void View::onRaiseRequest(Control& control)
{
}


void View::onMoveRequest(Control& control, const Gfx::PointF& pos)
{
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = scaling().align(pos);

    //
    // update client surface
    //
    PaintSurface* surface = _surface->surface();
    Gfx::PointF surfacePos = _surface->position() + aligedPos;

    control.setSurface(surface, surfacePos);

    //
    // send move event
    //
    MoveEvent mev(control, aligedPos);
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


void View::onResizeRequest(Control& control, const Gfx::SizeF& size)
{
    Gfx::SizeF alignedSize = scaling().align(size);

    //_info.resize( ev.size() );

    ResizeEvent rev(control, alignedSize);
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
