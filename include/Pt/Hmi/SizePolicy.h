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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_HMI_SIZEPOLICY_H
#define PT_HMI_SIZEPOLICY_H

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Hmi {

class SizePolicy
{
    public:
        enum Mode
        {
            Any = 0,

            Preferred = 1,

            // Is Maximum required?
            Maximum = 2,
            
            Fixed = 3,
        };

    public:
        SizePolicy()
        : _horizontalMode(Any)
        , _verticalMode(Any)
        { }

        SizePolicy(Mode horizontal, Mode vertical)
        : _horizontalMode(horizontal)
        , _verticalMode(vertical)
        { }

        Mode horizontal() const
        {
            return _horizontalMode;
        }

        void setHorizontal(Mode m)
        {
            _horizontalMode = m;
        }
        
        Mode vertical() const
        {
            return _verticalMode;
        }
        
        void setVertical(Mode m)
        {
            _verticalMode = m;
        }

        void setMode(Mode horizontal, Mode vertical)
        {
            _horizontalMode = horizontal;
            _verticalMode = vertical;
        }
        
        const Gfx::SizeF& size() const
        {
            return _sizeHint;
        }

        void setSize(const Gfx::SizeF& hint)
        {
            _sizeHint = hint;
        }

        void setSize(double w, double h)
        {
            _sizeHint.set(w, h);
        }

        double width() const
        { 
            return _sizeHint.width(); 
        }

        void setWidth(double w)
        {
            _sizeHint.setWidth(w);
        }

        double height() const
        { 
            return _sizeHint.height(); 
        }

        void setHeight(double h)
        {
            _sizeHint.setHeight(h);
        }

        bool operator== (const SizePolicy& s) const
        {
            return _horizontalMode == s._horizontalMode && 
                   _verticalMode == s._verticalMode && 
                   _sizeHint == s._sizeHint;
        }
        
        bool operator!= (const SizePolicy& s) const
        {
            return _horizontalMode != s._horizontalMode || 
                   _verticalMode != s._verticalMode || 
                   _sizeHint != s._sizeHint;
        }

    private:
        Mode       _horizontalMode;
        Mode       _verticalMode;
        Gfx::SizeF _sizeHint;
};

} // namespace

} // namespace

#endif // include guard
