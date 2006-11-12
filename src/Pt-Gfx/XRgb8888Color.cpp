/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include "Pt/Exception.h"
#include "Pt/Gfx/XRgb8888Color.h"

#include <iomanip>
#include <sstream>
using namespace std;


namespace Pt {

namespace Gfx {


Pt::uint8_t BasicColor<XRgb8888>::brightness() const
{
	Pt::uint8_t r = red();
	Pt::uint8_t g = green();
	Pt::uint8_t b = blue();
	return((r>=g && r>=b) ? r : ((g>=r && g>=b) ? g : b));
}


void BasicColor<XRgb8888>::setBrightness(Pt::uint8_t l)
{
	if(l == 0) {
		_val = 0;
		return;
	}

	Pt::uint8_t r = red();
	Pt::uint8_t g = green();
	Pt::uint8_t b = blue();

	if(r>=g && r>=b) {
		float o = r;
		setRed  (l);
		setGreen(Pt::uint8_t(g * l / o));
		setBlue (Pt::uint8_t(b * l / o));
	}
	else if (g>=r && g>=b) {
		float o = g;
		setRed  (Pt::uint8_t(r * l / o));
		setGreen(l);
		setBlue (Pt::uint8_t(b * l / o));
	}
	else {
		float o = b;
		setRed  (Pt::uint8_t(r * l / o));
		setGreen(Pt::uint8_t(g * l / o));
		setBlue (l);
	}
}

}

}
