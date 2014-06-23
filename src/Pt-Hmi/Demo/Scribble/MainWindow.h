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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */
#ifndef Pt_Hmi_Demo_MainWindow_h
#define Pt_Hmi_Demo_MainWindow_h

#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/Desktop/Button.h>
#include <Pt/Hmi/Desktop/Panel.h>
#include <Pt/Hmi/Desktop/Label.h>
#include <Pt/Gfx/ARgbColor.h>

namespace Pt{
namespace Hmi{

class PaintSurface;

namespace Demo{

class MainWindow : public Pt::Hmi::Desktop::Window
{
public:
	MainWindow();
	virtual ~MainWindow();

protected:
	virtual void init();

private:
	void onClosed();
	void onClosedByWindow(Pt::Hmi::Controller* ctrl);

private:
	void onRed();
	void onGreen();
	void onBlue();
	void onPointerChanged(const void*, const PropertyBase& prop);
	void onRender(GfxController& controller, PaintSurface& surface);

private:
	Pt::Hmi::Desktop::Button _closeButton;
	Pt::Hmi::Desktop::Button _redButton;
	Pt::Hmi::Desktop::Button _greenButton;
	Pt::Hmi::Desktop::Button _blueButton;
	Pt::Gfx::ARgbColor _color;
	bool _painting;
	std::vector<Pt::Gfx::PointF> _points;
}; 

}}}

#endif