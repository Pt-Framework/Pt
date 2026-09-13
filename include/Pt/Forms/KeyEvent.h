/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/
#ifndef Pt_Forms_KeyEvent_h
#define Pt_Forms_KeyEvent_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Key.h>
#include <Pt/Event.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

/** @brief Key press or release.

    The event carries a %Key, optional Unicode text, and the action. The
    widget pointer and widget ID identify the target.

    @ingroup Pt-Forms-Input
*/
class PT_FORMS_API KeyEvent : public Pt::BasicEvent<KeyEvent>
{
    public:
        /** @brief Defines the key action.
        */
        enum Action
        {
            Press,
            Release
        };

    public:
        /** @brief Creates a key-release event without a target.
        */
        KeyEvent()
        : _widgetId_(0)
        , _widget(0)
        , _action(Release)
          {
          }

        /** @brief Creates a key-release event targeted at @a widget.
        */
        explicit KeyEvent(Widget& widget)
        : _widgetId_( widget.id() )
        , _widget(&widget)
        , _action(Release)
	      {
	      }

        /** @brief Returns the target widget ID, or 0 when no target is set.
        */
        Pt::uint64_t widgetId() const
        {
            return _widgetId_;
        }

        /** @brief Returns the target widget, or 0 when no target is set.
        */
        Widget* widget() const
        {
            return _widget;
        }

        /** @brief Sets the target widget and its ID.
        */
        void setWidget(Widget* widget)
        {
            _widget = widget;
            _widgetId_ = widget ? widget->id() : 0;
        }

        /** @brief Returns the key and modifiers.
        */
        const Key& key() const
        {
            return _key;
        }

        /** @brief Returns the optional Unicode text.
        */
        const Pt::Char& unicode() const
        {
            return _unicode;
        }

        /** @brief Returns true when the event is a key press.
        */
        bool isPress() const
        {
            return _action == Press;
        }

        /** @brief Changes the event to a press of @a key with text @a ch.
        */
        void setPress(const Key& key, const Pt::Char& ch)
        {
            _action = Press;
            _key = key;
            _unicode = ch;
        }

        /** @brief Returns true when the event is a key release.
        */
        bool isRelease() const
        {
            return _action == Release;
        }

        /** @brief Changes the event to a release of @a key with text @a ch.
        */
        void setRelease(const Key& key, const Pt::Char& ch)
        {
            _action = Release;
            _key = key;
            _unicode = ch;
        }

    private:
        Pt::uint64_t _widgetId_;
        Widget*      _widget;
        Action       _action;
        Key          _key;
        Pt::Char     _unicode;
};

} // namespace

} // namespace

#endif
