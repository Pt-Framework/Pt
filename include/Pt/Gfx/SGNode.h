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


class ImagePainter2;


/** @brief A scene-graph node class.
  */
class PT_GFX_API SGNode {
    public:
        enum RenderMode {
            RenderInherit,
            RenderNone,
            RenderFill,
            RenderStroke,
            RenderStrokeAutoClose
        };

        typedef std::vector<SGNode*> Children;

    public:
        inline SGNode(RenderMode rm)
        : _parent(0)
        , _rm    (rm)
        {}

        inline SGNode(RenderMode rm, const Transform& transform)
        : _parent   (0)
        , _rm       (rm)
        , _transform( transform )
        {}

        inline SGNode(RenderMode rm, const Transform& transform, const Children& children)
        : _parent   (0)
        , _rm       (rm)
        , _transform( transform )
        , _children ( children )
        {}

        virtual ~SGNode() = 0;

        //
        // Management
        //

        virtual void clear();

        template <typename T>
        inline T& addChild(T* child_)
        {
            SGNode* child = child_;
            _children.push_back(child);

            child->_parent = this;

            return *child_;
        }

        inline RenderMode renderMode() const
        { return _rm; }

        //
        // Drawing
        //

        inline void setPen(const Pen& pen)
        { _pen = pen; }

        inline const Pen& pen() const
        { return _pen; }

        inline void setBrush(const Brush& brush)
        { _brush = brush; }

        inline const Brush& brush() const
        { return _brush; }

        virtual void draw(ImagePainter2& painter, const Transform* transform = 0) = 0;

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

    protected:
        SGNode*    _parent;

        RenderMode _rm;
        Pen        _pen;
        Brush      _brush;

        Transform  _transform;

        Children   _children;
};


/** @brief A scene-graph node class with an embedded Path.
  */
class PT_GFX_API SGNodePath : public SGNode {
    public:
        inline SGNodePath(RenderMode rm = RenderInherit)
        : SGNode     ( rm )
        , _smoothness( 1.0f )
        {}

        inline SGNodePath(RenderMode rm, const Path& path)
        : SGNode     ( rm )
        , _path      ( path )
        , _smoothness( 1.0f )
        {}

        inline SGNodePath(RenderMode rm, const Path& path, const Transform& transform)
        : SGNode     ( rm, transform )
        , _path      ( path )
        , _smoothness( 1.0f )
        {}

        inline SGNodePath(RenderMode rm, const Path& path, const Transform& transform, const Children& children)
        : SGNode     ( rm, transform, children )
        , _path      ( path )
        , _smoothness( 1.0f )
        {}

        virtual ~SGNodePath();

        //
        // Management
        //

        virtual void clear();

        //
        // Drawing
        //

        inline void setSmoothness(float smoothness = 1.0f)
        { _smoothness = smoothness; }

        virtual void draw(ImagePainter2& painter, const Transform* transform = 0);

        //
        // Path
        //

        inline Path& path()
        { return _path; }

        inline const Path& path() const
        { return _path; }

    public:
        friend class SGNode;

    protected:
        Path  _path;
        float _smoothness;
};


} // namespace
} // namespace

#endif
