/* Copyright (C) 2017 Marc Boris Duerner 
  
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

#ifndef Pt_Forms_StackLayout_H
#define Pt_Forms_StackLayout_H

#include <Pt/Forms/Layout.h>
#include <Pt/Signal.h>
#include <vector>

namespace Pt {

namespace Forms {

/** @brief Shows one child at a time.

    Added children are hidden until %setCurrent() selects an index. The
    current child fills the padded bounds minus its margin. The preferred
    size is the largest preferred child size. %current() is %NoIndex until
    a child is selected.

    @code
    +------------------+
    | current child    |
    | (others hidden)  |
    +------------------+
    @endcode

    @code
    Pt::Forms::StackLayout stack;
    stack.addItem(pageA);
    stack.addItem(pageB);
    stack.setCurrent(0);
    window.setContent(&stack);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class PT_FORMS_API StackLayout : public Layout
{
    public:
        typedef Layout Base;

        /** @brief Index value that means no child is current.
        */
        static const std::size_t NoIndex = static_cast<const std::size_t>(-1); 

    public:
        /** @brief Creates an empty stack layout.
        */
        StackLayout();

        /** @brief Destroys the layout. Attached controls are not destroyed.
        */
        virtual ~StackLayout();

        /** @brief Attaches @a control as a hidden stack page.

            The layout does not take ownership. Call %setCurrent() to show a
            page.
        */
        void addItem(Control& control);

        /** @brief Detaches @a control without destroying it.
        */
        void removeItem(Control& control);

        /** @brief Returns true when the stack has no children.
        */
        bool empty() const;

        /** @brief Returns the number of stacked children.
        */
        std::size_t size() const;

        /** @brief Returns the child at index @a n, or 0 when out of range.
        */
        Control* controlAt(std::size_t n) const;

        /** @brief Returns the index of @a control, or %NoIndex when absent.
        */
        std::size_t indexOf(Control& control) const;

        /** @brief Returns the current child index, or %NoIndex when none.
        */
        std::size_t current() const;

        /** @brief Shows the child at index @a n and hides the previous child.

            Has no effect when @a n is out of range.
        */
        void setCurrent(std::size_t n);

        /** @brief Returns the signal emitted when a control is removed.

            The argument is the former index of the removed control.
        */
        Pt::Signal<std::size_t>& controlRemoved()
        { return _controlRemoved; }

        /** @brief Returns the signal emitted when the current index changes.

            The argument is the new current index.
        */
        Pt::Signal<std::size_t>& currentChanged()
        { return _currentChanged; }

    protected:
        /** @brief Updates the stack after @a control is detached.
        */
        virtual void onRemoveControl(Control& control);

        /** @brief Measures the largest preferred child size.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Assigns the padded bounds to the current child.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    private:
        Pt::Signal<std::size_t> _controlRemoved;
        Pt::Signal<std::size_t> _currentChanged;
        std::vector<Control*>    _controls;
        std::size_t             _current;
};

} // namespace

} // namespace

#endif
