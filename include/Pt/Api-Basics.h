/*
  Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_API_BASICS_H
#define PT_API_BASICS_H

/** @addtogroup Pt-Basics

    @brief Program options, application settings, events and a few
    general-purpose helper types.

    Basic application support is provided by the Pt::Arg class and the
    Pt::Settings class. The first one is a convenient way to parse and
    process program options and the latter one allows to load and store
    application settings in files or other places.

    Pt::Event is the base class for type-safe event objects dispatched by an
    event loop.

    Pt::Any holds a value of any default- and copy-constructible type, and
    Pt::Void is a marker type used where @c void cannot be used as a
    template argument. Pt::NonCopyable is a mixin base class that disables
    copy construction and assignment, and Pt::Singleton implements the
    singleton pattern for a type.
*/

#endif
