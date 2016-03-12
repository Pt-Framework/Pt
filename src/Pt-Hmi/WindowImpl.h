/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#ifndef Pt_Hmi_WindowImpl_H
#define Pt_Hmi_WindowImpl_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/WindowBorder.h>
#include <Pt/Hmi/WindowDecoration.h>
#include <Pt/Hmi/WindowStartPosition.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Connectable.h>

namespace Pt{

namespace Hmi{

class Window;

class WindowImpl : public Pt::Connectable
{
    public:
        WindowImpl(Window* api);
        
        virtual ~WindowImpl();

        virtual void setVisible(bool b) = 0;

        virtual void activate() = 0;

        virtual void update(const Gfx::RectF& rect) = 0;

        virtual void onUpdate(Window& child, const Gfx::RectF& childRect) = 0;

        virtual void move(const Gfx::PointF& pos) = 0;
   
        virtual void resize(const Gfx::SizeF& size) = 0;

        virtual void setState(WindowState::Type s) = 0;

        virtual void setBorder( WindowBorder::Type b ) = 0;

        virtual void setIcon( const Gfx::Image& i ) = 0;

        virtual void setEnabled( bool e ) = 0;

        virtual void setMinimumSize( const Gfx::SizeF& s ) = 0;

        virtual void setMaximumSize( const Gfx::SizeF& s ) = 0;

        virtual void setDecoration( WindowDecoration::Flags d ) = 0;

        virtual void setTitle( const std::string& t ) = 0;

        virtual void close() = 0;

    protected:
        Window*  _apiWindow;

};

} // namespace

} // namespace

#endif
