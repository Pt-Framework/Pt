/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_FORMS_API_WIDGETS_H
#define PT_FORMS_API_WIDGETS_H

/** @addtogroup Pt-Forms-Widgets

    @brief Widget and view object model.

  Every visual Forms object is a %Widget. A widget has a non-owning parent
  relationship and becomes connected when its parent hierarchy reaches a
  %Screen. Connection is independent of visibility. It supplies logical
  coordinates, scaling, repaint and state-change requests, and input
  delivery. Attaching a control does not transfer ownership: the caller
  keeps every attached object alive until it is detached.

  %Screen and %WindowManager are widgets but not views. %Form and %Control
  are the usual %View implementations. A view is the host and paint-surface
  boundary for controls. When a control joins a form that is shown on a
  screen, the screen, scaling, coordinates, and paint surface become
  available for the cycle in @ref Pt-Forms-Page-Updating. Removing it
  reverses that relationship.

  Derive custom visual content from %Control. Derive from %View only when a
  custom content host needs different paint-surface or coordinate behavior.
  %WindowManager and %WindowFrame support platform and embedded-window
  implementations; applications normally use the window manager provided
  by a %Screen or %Workspace.
*/

#endif
