/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx_ARgbColor_h
#define Pt_Gfx_ARgbColor_h

#include <Pt/Api.h>
#include <Pt/Gfx/BasicColor.h>


namespace Pt {

	namespace Gfx {

		//! \brief ARgb color space
		struct ARgb {};


		//! \brief ARgb color
		template <>
		class PT_API BasicColor<ARgb> {
			public:
				typedef ARgb ColorSpaceT;

			public:
				//! Default ctor, will generate default color (black)
				inline BasicColor()
				: _a(0), _r(0), _g(0), _b(0)
				{}

				//! Copy ctor
				inline BasicColor(const BasicColor<ARgb>& c)
				: _a(c._a), _r(c._r), _g(c._g), _b(c._b)
				{}

				//! Construct color using the given source color (from any color space)
				template <typename SrcColorSpaceT>
				inline BasicColor(const BasicColor<SrcColorSpaceT>& c)
				: _a(0), _r(0), _g(0), _b(0)
				{ assign(*this, c); }

				//! Construct color using the given components
				inline BasicColor(uint16_t r, uint16_t g, uint16_t b)
				: _a(0xFFFF), _r(0), _g(0), _b(0)
				{
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				//! Construct color using the given components
				inline BasicColor(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				: _a(0), _r(0), _g(0), _b(0)
				{
					setAlpha(a);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				//! Convert this color to ARGB components of the master color model
				//! (destinations range from 0 to 0xFFFF)
				inline void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b) const
				{ a = _a; r = _r; g = _g; b = _b; }

				//! Set this color from the given ARGB components of the master color model
				//! (sources range from 0 to 0xFFFF)
				inline void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				{
					setAlpha(a);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				//! Assignment operator from the same color space
				inline BasicColor<ARgb>& operator=(const BasicColor<ARgb>& c)
				{ _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

				//! Assignment operator from different color space
				template <typename SrcColorSpaceT> inline
				BasicColor<ARgb>& operator=(const BasicColor<SrcColorSpaceT>& c)
				{ assign(*this, c); return *this; }

				//! Equality comparison operator
				inline bool eq(const BasicColor<ColorSpaceT>& c) const
				{ return _a==c._a && _r==c._r && _g==c._g && _b==c._b; }

				//! Lessthan comparison operator
				bool lt(const BasicColor<ColorSpaceT>& c) const;

				//! Greaterthan comparison operator
				bool gt(const BasicColor<ColorSpaceT>& c) const;

				//! Return the alpha component of this color
				inline uint16_t alpha() const
				{ return _a; }

				//! Return the red component of this color
				inline uint16_t red() const
				{ return _r; }

				//! Return the green component of this color
				inline uint16_t green() const
				{ return _g; }

				//! Return the blue component of this color
				inline uint16_t blue() const
				{ return _b; }

				//! Set the alpha component of this color
				inline void setAlpha(uint16_t a)
				{ _a = a; }

				//! Set the red component of this color
				inline void setRed(uint16_t r)
				{ _r = r; }

				//! Set the green component of this color
				inline void setGreen(uint16_t g)
				{ _g = g; }

				//! Set the blue component of this color
				inline void setBlue(uint16_t b)
				{ _b = b; }

				//! Get brightness
				inline uint16_t brightness() const
				{ return (_r>=_g && _r>=_b) ? _r : ((_g>=_r && _g>=_b) ? _g : _b); }

                inline BasicColor& operator*=( float factor )
                {
                    _a *= static_cast<Pt::uint16_t>( factor ); 
                    _r *= static_cast<Pt::uint16_t>( factor );
                    _g *= static_cast<Pt::uint16_t>( factor );
                    _b *= static_cast<Pt::uint16_t>( factor );
                    return *this;
                }
                
                inline BasicColor& operator+=( const BasicColor& color)
                {
                    _a += color._a;
                    _r += color._r;
                    _g += color._g;
                    _b += color._b;                    
                    return *this;
                }                
                
				//! Set brightness
				void setBrightness(uint16_t l);

			protected:
				uint16_t _a, _r, _g, _b;
		};


		typedef BasicColor<ARgb> ARgbColor;


		inline void greyscale(ARgbColor& color)
		{
			const Pt::uint16_t _f = static_cast<uint16_t>( color.red()*0.3f + color.green()*0.5f + color.blue()*0.2f );
			color.setAlpha( color.alpha() );
			color.setRed(_f);
			color.setGreen(_f);
			color.setBlue(_f);
		}


		//! \brief Full specialization for Greyscale<T> to be used with ARgb color space
		template<>
		struct Greyscale<ARgb>
		{
			public:
				typedef BasicColor<ARgb> Color;

				inline void operator()(Color& color) const
				{
					this->operator()(color, color);
				}

				inline void operator()(Color& to, const Color& from) const
				{
					const Pt::uint16_t _f = static_cast<uint16_t>(from.red()*0.3f + from.green()*0.5f + from.blue()*0.2f);
					to.setAlpha( from.alpha() );
					to.setRed(_f);
					to.setGreen(_f);
					to.setBlue(_f);
				}
		};

	} // namespace Gfx

} // namespace Pt

#endif

