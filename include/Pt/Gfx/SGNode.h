/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#ifndef PT_GFX_SGNODE_H
#define PT_GFX_SGNODE_H

#include <vector>

#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Transform.h>


namespace Pt{
namespace Gfx{


/** @brief A scene-graph node class.
  */
class PT_GFX_API SGNode {
    public:
        typedef std::vector<SGNode> Children;

    public:
        inline SGNode()
        {}

        inline ~SGNode()
        {}

        inline void clear()
        {
            _path.clear();
            _transform.identity();
            _children.clear();
        }

        inline Path& path()
        { return _path; }

        inline const Path& path() const
        { return _path; }

        inline Transform& transform()
        { return _transform; }

        inline const Transform& transform() const
        { return _transform; }

        inline const Children& children() const
        { return _children; }

    private:
        Path      _path;
        Transform _transform;

        Children  _children;
};


} // namespace
} // namespace

#endif
