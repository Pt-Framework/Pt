/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_XRGB8888IMAGE_H
#define PT_GFX_XRGB8888IMAGE_H

#include <Pt/Gfx/XRgb8888Color.h>
#include <Pt/Gfx/SubImage.h>


namespace Pt {

	namespace Gfx {

		//!
		//! \brief XRGB8888 image class
		//!
		typedef BasicImage<XRgb8888> XRgb8888Image;

		//!
		//! \brief XRGB8888 sub image class
		//!
		typedef SubImage<XRgb8888> XRgb8888SubImage;
	}
	
}

#endif

