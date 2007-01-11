/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_GFX_DRAWTEXT_H
#define PT_GFX_DRAWTEXT_H

#include "freetype/include/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Math/Point.h"
#include "Pt/Text/String.h"
#include "Pt/Gfx/ARgbColor.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/Pen.h"

namespace Pt{
namespace Gfx{

class DrawText
{
    public:    
        DrawText();
        ~DrawText();
        
        void setFont( const Font& font );
        void draw( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text);       
        void draw( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor& backGround );               
    
    private:            
         void drawGlyph( ARgbImage& image, const ARgbColor& color,  int glyphPosX, int glyphPosY, FT_Bitmap& bm );
         void _drawText( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor* backGround = 0); 

         //Depricated
         inline void mixColor( ARgbColor& dst, ARgbColor src, float factor)
         {
            dst *= ( 1.0f - factor );
            dst += ( src *= factor );
         }
         
        inline void mixColor( ARgbColor& dst, const ARgbColor& src, unsigned char factor)
        {
            const unsigned char rfactor(255 - factor);

            dst.setRed( (( dst.red() * rfactor ) / 255)
                      + (( src.red() * factor)   / 255) );

            dst.setGreen( (( dst.green() * rfactor ) / 255)
                        + (( src.green() * factor)   / 255) );

            dst.setBlue( (( dst.blue() * rfactor ) / 255)
                       + (( src.blue() * factor)   / 255) );
        }
               
         FT_Library  _ft;
         FT_Face     _face; 
         Font        _font;   
         FT_Matrix   _matrix;
         
};

} //namespace Gfx
} //namespace Pt

#endif 
