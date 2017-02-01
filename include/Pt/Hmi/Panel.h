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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#ifndef Pt_Hmi_Panel_H
#define Pt_Hmi_Panel_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

class PaintEvent;

class ImageLayout
{
    public:
        enum Type
        {
            None,
            Tile,
            Center,
            Strech,
            Zoom
        };

    public:
        ImageLayout()
        : _type(None)        
        { }

        ImageLayout(Type type)
        : _type(type)        
        { }

        const ImageLayout& operator=(Type type)
        {
          _type = type;
          return *this;
        }

        Type type() const
        {
            return _type;
        }
    
        bool operator ==(const ImageLayout& l) const
        {
            return _type == l._type;
        }

        bool operator !=(const ImageLayout& l) const
        {
            return _type != l._type;
        }

    private:
        Type _type;        
};


class PT_HMI_API Panel : public Control
{
    typedef Control Base;

	  public:
        Panel();
		
        virtual ~Panel();	
   
        //enum TileMode
        //{
        //    Tile,
        //    Strech,
        //    Zoom
        //};

        // TODO: use TileMode and Alignment instead of ImageLayout
        //       Tile seems to be the same as a Brush with an image

        void setImage(const Gfx::Image& image, ImageLayout layout); 

    public:
        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        void setBackground(bool b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& pen);

        void setFrame(bool b);

        void setRenderer(PanelRenderer* renderer);

    protected:
        void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        Gfx::Image  _image;
        ImageLayout _layout;

        FacetPtr<PanelRenderer> _renderer;
        bool                    _hasRenderer;

        AutoPtr<Gfx::Brush>     _background;
        bool                    _hasBackground;
                                
        AutoPtr<Gfx::Pen>       _contour;
        bool                    _hasFrame;

        Picture                 _picture;
};

} // namespace

} // namespace

#endif