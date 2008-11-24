/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
 *                                                                         *
 *   This code is based on code written by Christian Prochnow              *
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
#include "Pt/System/IOError.h"
#include <string>

namespace Pt {

namespace System {

IOError::IOError(const std::string& what, const SourceInfo& si)
: std::ios::failure(what + si)
{
}


IOError::IOError(const char* msg)
: std::ios::failure(msg)
{
}


AccessFailed::AccessFailed(const std::string& what, const SourceInfo& si)
: IOError(what , si)
{
}


AccessFailed::AccessFailed(const char* msg)
: IOError(msg)
{
}


PermissionDenied::PermissionDenied(const std::string& what, const SourceInfo& si)
: AccessFailed(what , si)
{
}


FileNotFound::FileNotFound(const std::string& path, const SourceInfo& si)
: AccessFailed("File not found", si)
, _path(path)
{}


DirectoryNotFound::DirectoryNotFound(const std::string& path, const SourceInfo& si)
: AccessFailed("Directory not found", si)
, _path(path)
{ }


IOPending::IOPending(const std::string& what, const SourceInfo& si)
: IOError(what, si)
{
}


IOPending::IOPending(const char* msg)
: IOError(msg)
{
}

} // namespace System

} // namespace Pt
