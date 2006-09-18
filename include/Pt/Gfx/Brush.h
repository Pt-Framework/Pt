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
