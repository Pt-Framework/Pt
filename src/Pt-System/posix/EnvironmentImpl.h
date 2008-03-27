/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 by PTV AG                                          *
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

#ifndef PT_EnvironmentImpl_H
#define PT_EnvironmentImpl_H

#include <string>

namespace Pt {

namespace System {

class EnvironmentImpl
{
public:
    EnvironmentImpl();
    
    ~EnvironmentImpl();

    static const std::string& sharedLibraryExtension();

    static const std::string& sharedLibraryPrefix();

    static const std::string& systemDirectory();
    
    static const std::string currentDirectory();

    static bool changeDirectory( const std::string& path);

    static const std::string tempDirectory();

    static char pathSeparator()
    {
        return '/';
    }
    
    static unsigned long getTotalMemory();
    
    static unsigned long getFreeMemory();
    
    static unsigned long getProcessMemoryUsage();

};

} // namespace ptv
} // namespace system

#endif //PT_EnvironmentImpl_H
