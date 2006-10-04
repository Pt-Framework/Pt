/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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
#ifndef PT_GFX_PRUSH_H
#define PT_GFX_BRUSH_H

#include <Pt/Api.h>
#include <Pt/SmartPtr.h>
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/ARgbImage.h>


namespace Pt {

	namespace Gfx {

		class BrushData;


		class PT_EXPORT Brush
		{
			public:
				enum FillStyle {
					SolidFill, TextureFill
				};

			public:
				Brush(FillStyle fillStyle = SolidFill, const ARgbColor& color = ARgbColor(), const ARgbImage* texture = 0);

				FillStyle fillStyle() const;

				const ARgbColor& color() const;

				const ARgbImage& texture() const;

				bool isSame(const Brush& b)
				{
					return _brushData == b._brushData;
				}

			private:
				SmartPtr<BrushData> _brushData;
		};


		class PT_EXPORT BrushData
		{
			public:
				BrushData(Brush::FillStyle fillStyle, const ARgbColor& color, const ARgbImage* texture);

				~BrushData();

				Brush::FillStyle fillStyle() const;

				const ARgbColor& color() const;

				const ARgbImage& texture() const;

				void setFillStyle(Brush::FillStyle fillStyle);

				void setColor(const ARgbColor& color);

				void setTexture(const ARgbImage& texture);

			private:
				Brush::FillStyle _fillStyle;
				ARgbColor        _color;
				ARgbImage*       _texture;
		};


	} // namespace gfx

} // namespace ptv

#endif
