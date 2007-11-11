/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2006 Sebastian Pieck                                    *
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

#include "ApplicationImpl.h"

#include <iostream>
#include <cerrno>
#include <stdexcept>
#include <cstdlib>
#include <cerrno>
#include <sys/neutrino.h>
#include <sys/select.h>
#include <Pt.h>

namespace Pt {

namespace Gui {

int eventCallback(PtWidget_t* widget, void* data, PtCallbackInfo_t* info)
{
	reinterpret_cast<ApplicationImpl*>(data)->wake();
	return 0;
}


ApplicationImpl::ApplicationImpl(Application& app)
{
	if( PtInit(NULL) == -1 )
		PtExit(EXIT_FAILURE);
}


ApplicationImpl::ApplicationImpl()
{
	if( PtInit(NULL) == -1 )
		PtExit(EXIT_FAILURE);
}


ApplicationImpl::~ApplicationImpl()
{

}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{

}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{

}


void ApplicationImpl::processEvents()
{

}


int ApplicationImpl::run()
{  
	PhEventArm();

    //connect(GfEventLoop::instance().event, app.event);

	//mainWidget = PtCreateWidget(PtWindow, Pt_NO_PARENT, 0, NULL);
	//PtAddEventHandler(mainWidget, Ph_EV_BUT_PRESS | Ph_EV_PTR_MOTION, eventCallback, this);
	//PtRealizeWidget(mainWidget);
	
	PtMainLoop();
	return true;
}


void ApplicationImpl::wake()
{

}


void ApplicationImpl::exit()
{
	PtExit(EXIT_SUCCESS);
}

} // namespace Gui

} // namespace Pt

