/*
 * Copyright (C) 2006 Marc Boris Duerner
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

#ifndef Pt_Hmi_ApplicationImpl_h
#define Pt_Hmi_ApplicationImpl_h

#include <Pt/System/MainLoop.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl : public Pt::System::MainLoop
{
    public:
        ApplicationImpl();

        ~ApplicationImpl();

	void showConsole(bool t)
	{
	}

		Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value)
		{
			return Pt::Gfx::PointF(value.x(), value.y());
		}

		Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value)
		{
			return Pt::Gfx::SizeF(value.width(), value.height());
		}

		double toUnit(int value)
		{
			return value;
		}

		Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value)
		{
			return Pt::Gfx::Point(value.x(), value.y());
		}

		Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value)
		{
			return Pt::Gfx::Size(value.width(), value.height());
		}

		Pt::Gfx::Rect fromUnit(const Pt::Gfx::RectF& value)
		{
			return Pt::Gfx::Rect(Gfx::Point( value.x(), value.y()) , Gfx::Size(value.width(), value.height()));
		}

		int fromUnit(double value)
		{
			return (int) value;
		}

		double unitSizeInch() const
		{
			return 0;
		}
			
		double unitSizeMm() const
		{
			return 0;
		}

		void setResolution(double dpi)
		{
		}

		double resolutionDPI() const
		{
			return 0;
		}

		inline Pt::Signal<const Pt::Event&>& systemEvent()
		{
			return _systemEvent;
		}

		void nextEvent();

	protected:
		Pt::Signal<const Pt::Event&> _systemEvent;
};

} // namespace

} // namespace

#endif
