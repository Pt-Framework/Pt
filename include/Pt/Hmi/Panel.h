/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Gfx/Color.h>

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
	  public:
		    enum BorderStyle
		    {
			      NoBorder,
			      Single,
			      Border3D
		    };

        enum Alignment
        {
            TopLeft,
            TopCenter,
            TopRight,
            MiddleLeft,
            MiddleCenter,
            MiddleRight,
            BottomLeft,
            BottomCenter,
            BottomRight
        };

        //enum TileMode
        //{
        //    Tile,
        //    Strech,
        //    Zoom
        //};

        // TODO: use TileMode and Alignment instead of ImageLayout

	  public:
        Panel();
		
        virtual ~Panel();	

        const Gfx::Brush& background() const
        {
            return _backgroundBrush;
        }

        void setBackground( const Gfx::Brush& c, bool doRepaint = true )
        {
            _backgroundBrush = c;

            if( doRepaint )
                update();
        }

        const Gfx::Pen& foreground() const
        {
            return _foregroundPen;
        }

        void setForeground(const Gfx::Pen& c )
        {
            _foregroundPen = c;
            update();
        }

        void setBackgroundImage(const Gfx::Image& image, ImageLayout layout)
        {
            if( layout == ImageLayout::Strech || layout ==  ImageLayout::Zoom)
             _backgroundImage = image;
            
            _backgroundImageLayout = layout;
            _backgroundPicture.set( image);
            update();
        }

        BorderStyle borderStyle() const
        {   
            return _borderStyle;
        }

        void setBorderStyle(BorderStyle t)
        {   
            _borderStyle = t;
            update();
        }

        bool isBorderRound() const
        {   
          return _borderRound;
        }

        void setBorderRound(bool b)
        {   
          _borderRound = b;
          update();
        }

        double borderWidth() const	  
        {   
            return _borderWidth;
        }

        void setBorderWidth( double w )
        {
          _borderWidth = w;
          update();
        }
         
        const Gfx::Color& borderColor() const
        {
          return _borderColor;
        }

        void setBorderColor(Gfx::Color b)
        {
          _borderColor = b;
          update();
        }

    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);
	
        virtual void onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect);

    private: 
        Gfx::Brush    _backgroundBrush;
        Gfx::Pen      _foregroundPen;
        Gfx::Image    _backgroundImage;
        Picture       _backgroundPicture;
        ImageLayout   _backgroundImageLayout;
        BorderStyle	  _borderStyle;
        bool	   	  _borderRound;	
        double			  _borderWidth;	  
        Gfx::Color	  _borderColor;
}; 

} // namespace

} // namespace

#endif