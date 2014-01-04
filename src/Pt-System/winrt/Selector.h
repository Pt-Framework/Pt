/*
 * Copyright (C) 2006-2012 Marc Boris Duerner
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

#ifndef PT_SYSTEM_SELECTOR_H
#define PT_SYSTEM_SELECTOR_H

#include "../SelectableList.h"
#include "Pt/System/Api.h"
#include "Pt/System/Mutex.h"
#include "Pt/System/EventLoop.h"
#include <vector>
#include <windows.h>

namespace Pt {

namespace System {

class Selectable;

class PT_SYSTEM_API Selector
{
    public:
        Selector();
        
        ~Selector();

        void wake();

        void attach(Selectable& s);

        void detach(Selectable& s);

        bool waitForWake(size_t msecs);

    protected:
        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);

    private:
        HANDLE _wakeEvent;
        std::vector<HANDLE> _handles;
        SelectableList _selectables;
};

} // namespace System

} // namespace Pt

#endif
