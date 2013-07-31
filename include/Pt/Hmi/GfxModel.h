/* Copyright (C) 2013 Marc Boris Duerner 
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Hmi_Model_Gfx_h
#define Pt_Hmi_Model_Gfx_h

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/Event2D.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Painter.h>

namespace Pt{
namespace Hmi{

namespace ImageLayout
{
	enum Layout
	{
		None,
		Tile,
		Center,
		Strech,
		Zoom
	};
}

class PT_HMI_API GfxModel : public Model
{
protected:
	GfxModel();

public:
	Property<bool>					Visible;
	Property<Pt::Gfx::Font>			Font;
	Property<Pt::Gfx::PointF>		Position;
	Property<Pt::Gfx::SizeF>		Size;
	Property<Pt::Gfx::ARgbColor>	BackColor;
	Property<Pt::Gfx::ARgbColor>	ForeColor;
	Property<Pt::Gfx::ARgbImage>	BackgroundImage;
	Property<ImageLayout::Layout>	BackgroundImageLayout;
	Property<int>					Opacity;
	Property<Pt::Gfx::ARgbColor>	TransparancyKey;
	Pt::Gfx::ARgbImage				PaintBuffer;
	Property<Event2D>				Pointer2DStatus;	
	Property<Cursor>	            CursorStatus; 

public:
	virtual ~GfxModel();

	void move(const Pt::Gfx::SizeF& size);	
    bool contains(const Pt::Gfx::PointF& p);

	//Unit handling
	double toUnit(int v);
	Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
	Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);
	int fromUnit(double v);
	Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
	Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);
	double unitSizeInch() const;
	double unitSizeMm() const;
};

}}

#endif