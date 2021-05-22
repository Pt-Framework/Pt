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

#ifndef PT_HMI_VISUAL_H
#define PT_HMI_VISUAL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Types.h>
#include <Pt/Event.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Hmi/Spacing.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <string>

namespace Pt {

namespace Hmi {

class Visual;

///////////////////////////////////////////////////////////////////////
// Responder
///////////////////////////////////////////////////////////////////////

class PT_HMI_API Responder
{
    public:
        virtual ~Responder();

        void mouseEvent(const MouseEvent& ev)
        {
            //bool consumed = ev.isPress() ? onMouseDown(ev) 
            //                             : onMouseUp(ev);
            
            bool consumed = onMouseEvent(ev);

            if(consumed)
                return;

            Responder* next = onNextResponder();
            if(next)
            {
                Gfx::PointF p = onToNextResponder( ev.position() );
                
                MouseEvent ev2(ev);
                ev2.setPosition(p);

                next->mouseEvent(ev);
            }
        }

        virtual void touchEvent(const TouchEvent& ev)
        {}

        virtual void scrollEvent(const ScrollEvent& ev)
        {}

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev) = 0;

        //virtual bool onMouseUp(const MouseEvent& ev) {}

        //virtual bool onMouseDown(const MouseEvent& ev) {}

        virtual Responder* onNextResponder() = 0;

        virtual Gfx::PointF onToNextResponder(const Gfx::PointF& pos) = 0;

    protected:
        Responder();
};

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

class PT_HMI_API Visual : public Responder
{
    public:
        virtual ~Visual();

        Pt::uint64_t vid() const
        {
            return _vid;
        }

        void setName(const std::string& n)
        {
            _name = n;
        }

        const std::string& name() const
        {
            return _name;
        }

        void processEvent(const Pt::Event& ev);

    protected:
        Visual();
        
        virtual void onEvent(const Pt::Event& ev) = 0;

    public:
        const Visual* parent() const
        {
          return onParent();
        }

        Visual* parent()
        {
          return onParent();
        }

        const Gfx::PointF& position() const
        {
            return onPosition();
        }

        const Gfx::SizeF& size() const
        {
            return onSize();
        }

        double scaleFactor() const
        {
            return onScaleFactor();
        }
        
        /** @brief Converts to parent coordinate.
        */
        Gfx::PointF toParent(const Gfx::PointF& pos) const;

        /** @brief Converts to client coordinate.
        */
        Gfx::PointF fromParent(const Gfx::PointF& pos) const;
        
        /** @brief Converts to client coordinate.
        */
        Gfx::PointF toClient(const Gfx::PointF& pos, const Visual& v) const;

        /** @brief Converts to parent coordinate.
        */
        Gfx::PointF fromClient(const Gfx::PointF& pos, const Visual& v) const;

        /** @brief Converts to global coordinate.
        */
        Gfx::PointF toScreen(const Gfx::PointF& pos) const;

        /** @brief Converts to client coordinate.
        */
        Gfx::PointF fromScreen(const Gfx::PointF& pos) const;

    protected:
        virtual const Gfx::PointF& onPosition() const = 0;

        virtual const Gfx::SizeF& onSize() const = 0;

        virtual double onScaleFactor() const = 0;

        virtual Visual* onParent() const = 0;

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const = 0;

    public:
        double toPhysical(double n) const
        {
            return n * scaleFactor();
        }

        Gfx::PointF toPhysical(const Gfx::PointF& p) const
        {
            return p * scaleFactor();
        }

        Gfx::SizeF toPhysical(const Gfx::SizeF& s) const
        {
            return s * scaleFactor();
        }

        Gfx::RectF toPhysical(const Gfx::RectF& r) const
        {
            return Gfx::RectF( toPhysical( r.topLeft() ), 
                               toPhysical( r.size() ) );
        }


        double toLogical(double n) const
        {
            return n / scaleFactor();
        }
        Gfx::PointF toLogical(const Gfx::PointF& p) const
        {
            return p / scaleFactor();
        }

        Gfx::SizeF toLogical(const Gfx::SizeF& s) const
        {
            return s / scaleFactor();
        }

        Gfx::RectF toLogical(const Gfx::RectF& r) const
        {
            return Gfx::RectF(toLogical(r.topLeft()), toLogical(r.size()));
        }


        double align(double n) const
        {
            // better name: alignGrid()

            double p = toPhysical(n);
            p = lround(p);
            return toLogical(p);
        }

        double alignPixel(double n) const
        {
            double p = toPhysical(n);
            p = lround(p + 0.5) - 0.5;
            return toLogical(p);
        }

        double alignContour(size_t n) const
        {
            const double scaling = scaleFactor();
            // keep contour size when downscaling
            if (scaling < 1.0)
                return toLogical(n);

            double p = toPhysical(n);
            size_t s = static_cast<size_t>(p);
            return toLogical(s);
        }

        Gfx::PointF align(const Gfx::PointF& p) const
        {
            Gfx::PointF pos = toPhysical(p);
            pos.setX(lround(pos.x()));
            pos.setY(lround(pos.y()));
            return toLogical(pos);
        }

        Gfx::SizeF align(const Gfx::SizeF& s) const
        {
            Gfx::SizeF size = toPhysical(s);
            size.setWidth(lround(size.width()));
            size.setHeight(lround(size.height()));
            return toLogical(size);
        }

        Gfx::RectF align(const Gfx::RectF& rect) const
        {
            Gfx::PointF pos = toPhysical(rect.topLeft());
            pos.setX(lround(pos.x()));
            pos.setY(lround(pos.y()));

            Gfx::SizeF size = toPhysical(rect.size());
            size.setWidth(lround(size.width()));
            size.setHeight(lround(size.height()));

            return toLogical(Gfx::RectF(pos, size));
        }

        Spacing align(const Spacing& spacing) const
        {
            Spacing alignedSpacing(align(spacing.left()),
                align(spacing.top()),
                align(spacing.right()),
                align(spacing.bottom()));

            return alignedSpacing;
        }

        public:
            void repaint()
            {
                Gfx::RectF rect( Gfx::PointF(0, 0), size() );
                onRepaint(rect);
            }

            void repaint(const Gfx::RectF& rect)
            {
                onRepaint(rect);
            }

        protected:
            virtual void onRepaint(const Gfx::RectF& rect) = 0;

    private:
        Pt::uint64_t _vid;
        std::string  _name;
};

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

class Widget;
class Window;
class Screen;

class View : public virtual Visual
{
    friend class Widget;

    protected:
        View()
        {}

    public:
        virtual ~View()
        {}

        Window* window();

        Screen* screen();

        Gfx::PointF toWidget(const Widget& widget, const Gfx::PointF& pos) const
        { 
            return onToWidget(widget, pos); 
        }

        Gfx::PointF fromWidget(const Widget& widget, const Gfx::PointF& pos) const
        { 
            return onFromWidget(widget, pos); 
        }

        void relayout();

    protected:
        virtual void onRelayout() = 0;

    protected:
        void add(Widget& widget);

        void remove(Widget& widget);

    private:
        virtual Window* onGetWindow() = 0;

        virtual Screen* onGetScreen() = 0;

        virtual Gfx::PointF onToWidget(const Widget& widget, const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromWidget(const Widget& widget, const Gfx::PointF& pos) const = 0;

    private:
        virtual void onAttach(Widget& widget) = 0;

        virtual void onDetach(Widget& widget) = 0;

        virtual void onRaise(Widget& widget) = 0;

        virtual void onEnable(Widget& widget, bool isEnable) = 0;

        virtual void onShow(Widget& widget, bool isShown) = 0;
};

} // namespace

} // namespace

#endif // include guard