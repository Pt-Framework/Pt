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

#include <Pt/Gfx/Transform.h>


namespace Pt{
namespace Gfx{


class ImagePainter2;


/** @brief The base class for all scene-graph node classes.
  */
class PT_GFX_API SGNode {
    public:
        enum RenderMode {
            RenderInherit,         //! @brief Inherit the mode from this node's parent
            RenderNone,            //! @brief Do not draw this node
            RenderStroke,          //! @brief Draw as a stroked shape
            RenderStrokeAutoClose, //! @brief Draw as a stroked shape with auto-close (if not supported by, it will be drawn using the closest mode)
            RenderFill,            //! @brief Draw as a filled shape (if not supported by, it will be drawn using the closest mode)

            _DoNotOverrideRM       //! @brief For internal use
        };

        typedef double                 ValueT;
        typedef BasicPoint    <ValueT> PointT;
        typedef BasicSize     <ValueT> SizeT;
        typedef BasicRect     <ValueT> RectT;
        typedef BasicTransform<ValueT> TransformT;

        typedef std::vector<SGNode*>             Children;
        typedef Children::const_iterator         ConstIterator;
        typedef Children::const_reverse_iterator ConstReverseIterator;

    private:
        // Node data
        struct NodeData {
            Pen                        pen;       // Pen   (if null, then use the parent's pen  )
            Brush                      brush;     // Brush (if null, then use the parent's brush)
            TransformT                 transform; // Transform
            Children                   children;  // Children

            Color                      trCFil;    // Background fill used when rotating texture
            Brush::TextureRotationMode trMode;    // Texture rotation mode

            inline NodeData()
            {}

            inline NodeData(const TransformT& transform_)
            : transform( transform_ )
            {}

            inline NodeData(const TransformT& transform_, const Children& children_)
            : transform( transform_ )
            , children ( children_ )
            {}
        };

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

    private:
        // Special constructor to create a proxy node
        inline SGNode(RenderMode rm, SmartPtr<NodeData> nodeData)
        : _parent    ( 0 )
        , _rm        ( rm )
        , _nodeData  ( nodeData )
        , _nodeDataRO( true )
        {}

        friend class SGNodeProxy;

    public:
        inline SGNode(RenderMode rm = RenderInherit)
        : _parent    ( 0 )
        , _rm        ( rm )
        , _nodeData  ( new NodeData() )
        , _nodeDataRO( false )
        { setTextureRotationParameters(); }

        inline SGNode(RenderMode rm, const TransformT& transform)
        : _parent    ( 0 )
        , _rm        ( rm )
        , _nodeData  ( new NodeData(transform) )
        , _nodeDataRO( false )
        { setTextureRotationParameters(); }

        inline SGNode(RenderMode rm, const TransformT& transform, const Children& children)
        : _parent    ( 0 )
        , _rm        ( rm )
        , _nodeData  ( new NodeData(transform, children) )
        , _nodeDataRO( false )
        { setTextureRotationParameters(); }

        virtual ~SGNode();

        //
        // Management functions
        //

        virtual void clear();

        template <typename T>
        inline T& addChild(T* child_)
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            SGNode* child = child_;
            child->checkForCircularChain(this);

            _nodeData->children.push_back(child);

            child->_parent = this;

            return *child_;
        }

        inline void removeChild(const SGNode* child)
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            Children::iterator it = std::find(_nodeData->children.begin(), _nodeData->children.end(), child);
            if(it == _nodeData->children.end()) return;

            _nodeData->children.erase(it);
        }

        inline const SGNode* parent() const
        { return _parent; }

        //
        // Render mode
        //

        inline void setRenderMode(RenderMode rm)
        { _rm = rm; }

        inline RenderMode renderMode() const
        { return _rm; }

        //
        // Drawing functions
        //

        inline void setPen(const Pen& pen)
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            _nodeData->pen = pen;
        }

        inline const Pen& effectivePen() const
        {
            if(!_nodeData->pen.isNull()) return _nodeData->pen;

            const SGNode* p = _parent;
            while(p) {
                if(!p->_nodeData->pen.isNull()) return p->_nodeData->pen;
                p = p->_parent;
            }

            return _nodeData->pen;
        }

        inline void setBrush(const Brush& brush)
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            _nodeData->brush = brush;
        }

        inline const Brush& effectiveBrush() const
        {
            if(!_nodeData->brush.isNull()) return _nodeData->brush;

            const SGNode* p = _parent;
            while(p) {
                if(!p->_nodeData->brush.isNull()) return p->_nodeData->brush;
                p = p->_parent;
            }

            return _nodeData->brush;
        }

        inline void setTextureRotationParameters(const Color& colorFill = Color::fromRgb8(0, 0, 0, 255), Brush::TextureRotationMode mode = Brush::BlockFit)
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            _nodeData->trCFil = colorFill;
            _nodeData->trMode = mode;
        }

        void draw(ImagePainter2& painter, const TransformT* transform = 0);

        //
        // Direct access to the pen object
        //

        inline Pen& pen()
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            return _nodeData->pen;
        }

        inline const Pen& pen() const
        { return _nodeData->pen; }

        //
        // Direct access to the brush object
        //

        inline Brush& brush()
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            return _nodeData->brush;
        }

        inline const Brush& brush() const
        { return _nodeData->brush; }

        //
        // Direct access to the transform object
        //

        inline TransformT& transform()
        {
            if(_nodeDataRO) throw std::logic_error("the node data is read-only in this instance");

            return _nodeData->transform;
        }

        inline const TransformT& transform() const
        { return _nodeData->transform; }

        //
        // Direct access to the child nodes
        //

        inline const Children& children() const
        { return _nodeData->children; }

        Children::const_iterator begin() const
        { return _nodeData->children.begin(); }

        Children::const_iterator end() const
        { return _nodeData->children.end(); }

        Children::const_reverse_iterator rbegin() const
        { return _nodeData->children.rbegin(); }

        Children::const_reverse_iterator rend() const
        { return _nodeData->children.rend(); }

    protected:
        inline RenderMode effectiveRenderMode(RenderMode overrideRM) const
        {
            const RenderMode rm = (overrideRM != _DoNotOverrideRM) ? overrideRM : _rm;

            if(rm != RenderInherit) return rm;

            const SGNode* p = _parent;
            while(p) {
                if(p->_rm != RenderInherit) return p->_rm;
                p = p->_parent;
            }

            return rm;
        }

        virtual void checkForCircularChain(const SGNode* parent) const;

        virtual void drawImpl(ImagePainter2& painter, const TransformT& transform, RenderMode overrideRM) const;

    private:
        SGNode*            _parent;     // Parent node
        RenderMode         _rm;         // Render mode

        SmartPtr<NodeData> _nodeData;   // Node data
        bool               _nodeDataRO; // A flag that indicates whether the node data is read-only
};


} // namespace
} // namespace

#endif
