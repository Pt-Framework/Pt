/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_PRUSH_H
#define PT_GFX_PRUSH_H

#include <Pt/Gfx/Api.h>
#include <Pt/SmartPtr.h>
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/ARgbImage.h>


namespace Pt {

namespace Gfx {

	class BrushData;

	class PT_GFX_API Brush
	{
		public:
			enum FillStyle {
				SolidFill, TextureFill
			};

		public:
			Brush(const ARgbColor& color = ARgbColor(0,0,0));

			Brush(const ARgbImage* texture);

			FillStyle fillStyle() const;

			const ARgbColor& color() const;

			const ARgbImage& texture() const;

			bool isSame(const Brush& b) const
			{ return _brushData == b._brushData; }

		private:
			SmartPtr<BrushData> _brushData;
	};


	class PT_API BrushData
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


} // namespace Gfx

} // namespace Pt

#endif
