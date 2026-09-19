/* Copyright (C) 2015 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_H
#define PT_FORMS_API_H

#include <Pt/Api.h>

#define PT_FORMS_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_FORMS_VERSION_MINOR PT_VERSION_MINOR
#define PT_FORMS_VERSION_REVISION PT_VERSION_REVISION
#define PT_FORMS_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_FORMS_API_EXPORT)
#    define PT_FORMS_API PT_EXPORT
#  else
#    define PT_FORMS_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Forms

    @brief Graphical user interfaces.

  The %Pt::Forms namespace provides the types for building event-driven
  graphical user interfaces. It contains applications, windows, widgets,
  controls, layouts, and the events that report user input and lifecycle
  changes.
*/
namespace Forms {

class ActivateEvent;
class CloseEvent;
class Control;
class EnableEvent;
class EnterEvent;
class FocusEvent;
class InvalidateEvent;
class KeyEvent;
class LeaveEvent;
class LayoutEvent;
class MeasureEvent;
class MouseEvent;
class MoveEvent;
class PaintEvent;
class RelayoutEvent;
class RescaleEvent;
class ResizeEvent;
class ScrollEvent;
class ShowEvent;
class TouchEvent;
class Widget;
class Window;
class WindowStateEvent;

} // namespace

} // namespace

#endif
