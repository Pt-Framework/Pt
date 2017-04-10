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
            RenderInherit,         //! @brief Inherit from parent
            RenderNone,            //! @brief Do not draw this node
            RenderStroke,          //! @brief Draw this node as an stroked shape
            RenderStrokeAutoClose, //! @brief Draw this node as an stroked shape with auto-close (only meaningful for some node types)
            RenderFill             //! @brief Draw this node as a filled shape                   (only meaningful for some node types)
        };

        typedef double ValueT;

        typedef BasicTransform<ValueT> TransformT;

        typedef std::vector<SGNode*>             Children;
        typedef Children::const_iterator         ConstIterator;
        typedef Children::const_reverse_iterator ConstReverseIterator;

    private:
        // A stack-element used for processing (traversing) the nodes
        struct TraversalStack {
            const SGNode*    node;      // Scene-graph node to be rendered
            const Pen        pen;       // For saving the pen
            const Brush      brush;     // For saving the brush
            const TransformT transform; // For saving the transformation
            bool             after;     // Flag that indicates the active processing phase

            inline TraversalStack(const SGNode* node_, const Pen& pen_, const Brush& brush_, const SGNode::TransformT& transform_)
            : node(node_), pen(pen_), brush(brush_), transform(transform_), after(false)
            {}
        };

    public:
        inline SGNode(RenderMode rm)
        : _parent(0)
        , _rm    (rm)
        {}

        inline SGNode(RenderMode rm, const TransformT& transform)
        : _parent   (0)
        , _rm       (rm)
        , _transform( transform )
        {}

        inline SGNode(RenderMode rm, const TransformT& transform, const Children& children)
        : _parent   (0)
        , _rm       (rm)
        , _transform( transform )
        , _children ( children )
        {}

        virtual ~SGNode();

        //
        // Management functions
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

        //
        // Drawing functions
        //

        inline void setRenderMode(RenderMode rm)
        { _rm = rm; }

        inline RenderMode renderMode() const
        { return _rm; }

        inline RenderMode effectiveRenderMode() const
        {
            if(_rm != RenderInherit) return _rm;

            const SGNode* p = _parent;
            while(p) {
                if(p->_rm != RenderInherit) return p->_rm;
                p = p->_parent;
            }

            return _rm;
        }

        inline void setPen(const Pen& pen)
        { _pen = pen; }

        inline void setBrush(const Brush& brush)
        { _brush = brush; }

        void draw(ImagePainter2& painter, const TransformT* transform = 0);

        //
        // Direct access to the pen object
        //

        inline Pen& pen()
        { return _pen; }

        inline const Pen& pen() const
        { return _pen; }

        //
        // Direct access to the brush object
        //

        inline Brush& brush()
        { return _brush; }

        inline const Brush& brush() const
        { return _brush; }

        //
        // Direct access to the transform object
        //

        inline TransformT& transform()
        { return _transform; }

        inline const TransformT& transform() const
        { return _transform; }

        //
        // Direct access to the child nodes
        //

        inline const Children& children() const
        { return _children; }

        Children::const_iterator begin() const
        { return _children.begin(); }

        Children::const_iterator end() const
        { return _children.end(); }

        Children::const_reverse_iterator rbegin() const
        { return _children.rbegin(); }

        Children::const_reverse_iterator rend() const
        { return _children.rend(); }

    protected:
        virtual void drawImpl(ImagePainter2& painter, const TransformT& transform) const = 0;

    protected:
        SGNode*    _parent;
        RenderMode _rm;
        Pen        _pen;
        Brush      _brush;
        TransformT _transform;
        Children   _children;
};


/** @brief A scene-graph node class that specifies a path.
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

        inline SGNodePath(RenderMode rm, const Path& path, const TransformT& transform)
        : SGNode     ( rm, transform )
        , _path      ( path )
        , _smoothness( 1.0f )
        {}

        inline SGNodePath(RenderMode rm, const Path& path, const TransformT& transform, const Children& children)
        : SGNode     ( rm, transform, children )
        , _path      ( path )
        , _smoothness( 1.0f )
        {}

        virtual ~SGNodePath();

        //
        // Management functions
        //

        virtual void clear();

        //
        // Drawing functions
        //

        inline void setSmoothness(float smoothness = 1.0f)
        { _smoothness = smoothness; }

        inline float smoothness() const
        { return _smoothness; }

        //
        // Direct access to the path object
        //

        inline Path& path()
        { return _path; }

        inline const Path& path() const
        { return _path; }

    protected:
        virtual void drawImpl(ImagePainter2& painter, const TransformT& transform) const;

    protected:
        Path  _path;
        float _smoothness;
};


/** @brief A scene-graph node class that specifies a line.
  */
class PT_GFX_API SGNodeLine : public SGNode {
    public:
        inline SGNodeLine(RenderMode rm = RenderInherit)
        : SGNode( rm )
        {}

        inline SGNodeLine(RenderMode rm, const PointF& from, const PointF& to)
        : SGNode( rm )
        , _from ( from )
        , _to   ( to )
        {}

        inline SGNodeLine(RenderMode rm, const PointF& from, const PointF& to, const TransformT& transform)
        : SGNode( rm, transform )
        , _from ( from )
        , _to   ( to )
        {}

        inline SGNodeLine(RenderMode rm, const PointF& from, const PointF& to, const TransformT& transform, const Children& children)
        : SGNode( rm, transform, children )
        , _from ( from )
        , _to   ( to )
        {}

        virtual ~SGNodeLine();

        //
        // Management functions
        //

        virtual void clear();

        //
        // Direct access to the line object
        //

        inline void set(const PointF& from, const PointF& to)
        {
            _from = from;
            _to   = to;
        }

        inline const PointF& from() const
        { return _from; }

        inline const PointF& to() const
        { return _to; }

    protected:
        virtual void drawImpl(ImagePainter2& painter, const TransformT& transform) const;

    protected:
        PointF _from;
        PointF _to;
};


// ### TODO: !!! MORE NODE TYPES !!! ###


} // namespace
} // namespace

#endif
