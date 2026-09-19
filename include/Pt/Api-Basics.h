/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
