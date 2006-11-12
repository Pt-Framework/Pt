/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_WIDGETPAINTER_H
#define PTV_WIDGETPAINTER_H

#include "PainterImpl.h"

#include <ptv/Api.h>
#include <ptv/gfx/gfx.h>


namespace ptv {

namespace gui {

	class WidgetImpl;

	class PTV_EXPORT WidgetPainter : public PainterImpl {
		public:
			WidgetPainter(WidgetImpl& widgetImpl);

			virtual ~WidgetPainter();

			virtual void begin();

			virtual void end();

		private:
			WidgetImpl& _widgetImpl;
	};

} // namespace gui

} // namespace ptv

#endif
