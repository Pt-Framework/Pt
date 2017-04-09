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

#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>

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
        : _pen   ( Color::fromRgb8(0, 0, 0, 255) )
        , _brush ( Color::fromRgb8(0, 0, 0, 255) )
        {}

        inline SGNode(const Path& path, const Transform& transform)
        : _pen      ( Color::fromRgb8(0, 0, 0, 255) )
        , _brush    ( Color::fromRgb8(0, 0, 0, 255) )
        , _path     ( path )
        , _transform( transform )
        {}

        inline SGNode(const Path& path, const Transform& transform, const Children& children)
        : _pen      ( Color::fromRgb8(0, 0, 0, 255) )
        , _brush    ( Color::fromRgb8(0, 0, 0, 255) )
        , _path     ( path )
        , _transform( transform )
        , _children ( children )
        {}

        inline ~SGNode()
        {}

        inline void clear()
        {
            _pen   = Pen  ( Color::fromRgb8(0, 0, 0, 255) );
            _brush = Brush( Color::fromRgb8(0, 0, 0, 255) );

            _path.clear();
            _transform.identity();
            _children.clear();
        }

        //
        // Drawing pen and path
        //

        inline void setPen(const Pen& pen)
        { _pen = pen; }

        inline const Pen& pen() const
        { return _pen; }

        inline void setPen(const Brush& brush)
        { _brush = brush; }

        inline const Brush& brush() const
        { return _brush; }

        //
        // Path
        //

        inline Path& path()
        { return _path; }

        inline const Path& path() const
        { return _path; }

        //
        // Transform
        //

        inline Transform& transform()
        { return _transform; }

        inline const Transform& transform() const
        { return _transform; }

        //
        // Children access
        //

        inline const Children& children() const
        { return _children; }

        Children::const_iterator begin() const
        { return _children.begin(); }

        Children::const_iterator end() const
        { return _children.end(); }

    private:
        Pen       _pen;
        Brush     _brush;

        Path      _path;
        Transform _transform;

        Children  _children;
};


} // namespace
} // namespace

#endif
