/* Copyright (C) 2017 Marc Boris Duerner
   Copyright (C) 2017 Aloysius Indrayanto

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

#ifndef PT_GFX_PATH_H
#define PT_GFX_PATH_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/FillRule.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Polygon.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/SmartPtr.h>

#include <vector>
#include <iterator>
#include <cstddef>

namespace Pt {

namespace Gfx {

class PathData;
class PathElement;
class PathIterator;

/** @brief Vector path for drawing outlines and filled shapes.
    @ingroup Pt-Gfx-Drawing

    %Path stores a sequence of drawing commands such as moves, lines and curves.
    Painters can stroke or fill the current path directly, while helper
    functions make it easy to append rectangles, ellipses and arc segments.
*/
class PT_GFX_API Path
{
    public:
        /** @brief Identifies the command stored in a path element.
        */
        enum ElementType
        {
            MoveTo,
            LineTo,
            QuadTo,
            CubicTo,
            Close
        };

        typedef PathIterator Iterator;
        typedef PathElement Element;

    public:
        /** @brief Constructs an empty path.
        */
        Path();

        /** @brief Copies another path.
        */
        Path(const Path& other);

        /** @brief Replaces the path contents.
        */
        Path& operator=(const Path& other);

        /** @brief Destroys the path.
        */
        ~Path();

        /** @brief Returns the number of path elements.
        */
        std::size_t size() const;

        /** @brief Returns true if the path has no elements.
        */
        bool isEmpty() const;

        /** @brief Returns an iterator to the first element.
        */
        Iterator begin() const;

        /** @brief Returns an iterator past the last element.
        */
        Iterator end() const;

        /** @brief Removes all path elements.
        */
        void clear();

        /** @brief Returns the bounding rectangle of the path.
        */
        RectF boundingRect() const;

        /** @brief Returns true if the point lies inside the filled area of the path.

            The test uses a horizontal ray cast from the point. The @a rule
            parameter controls how overlapping subpaths are handled.
            A point exactly on an upward-crossing edge is considered outside.
        */
        bool contains(const PointF& point, FillRule rule = FillRule::NonZero) const;

        /** @brief Returns true if the rectangle lies entirely inside the filled area.

            Every point of @a rect must be covered by the filled area of the path.
            Returns false if any path segment crosses a rectangle edge or any
            corner of @a rect is outside the filled area.
        */
        bool contains(const RectF& rect, FillRule rule = FillRule::NonZero) const;

        /** @brief Returns true if the filled area of the path overlaps @a rect.

            Returns true if any part of the filled path area shares at least
            one point with the interior of @a rect.  The test covers three
            cases: a corner of @a rect lies inside the path, a path segment
            crosses a rect edge, or the entire path lies inside @a rect.
        */
        bool intersects(const RectF& rect, FillRule rule = FillRule::NonZero) const;

        /** @brief Returns the current drawing position.
        */
        const PointF& currentPosition() const;

        /** @brief Starts a new subpath at the given point.
        */
        void moveTo(const PointF& p);

        /** @brief Adds a straight line to the given point.
        */
        void lineTo(const PointF& p);

        /** @brief Adds a quadratic Bezier segment.
        */
        void quadTo(const PointF &cp, const PointF& to);

        /** @brief Adds a cubic Bezier segment.
        */
        void cubicTo(const PointF &cp1, const PointF &cp2, const PointF& to);

        /** @brief Adds an arc segment to the current subpath.

            The arc is drawn inside the ellipse bounded by @a topLeft and
            @a size.  Angles are measured clockwise from the positive
            x-axis.  A positive sweep (@a degEnd > @a degBegin) is
            clockwise; a negative sweep is counter-clockwise.  If the
            path is empty a %MoveTo is added, otherwise a %LineTo.
        */
        void arcTo(const PointF& topLeft, const SizeF& size,
                   double degBegin, double degEnd);

        /** @brief Closes the current subpath.
        */
        void close();

        /** @brief Appends all elements of @a p to this path.
        */
        void addPath(const Path& p);

        /** @brief Adds a rectangle as a new subpath.
        */
        void addRect(const RectF& rect);

        /** @brief Adds a rounded rectangle as a new subpath.
        */
        void addRoundedRect(const RectF& rect, double radius);

        /** @brief Adds a rounded rectangle with elliptical corners as a new subpath.

            @a rx is the horizontal corner radius, @a ry the vertical corner radius.
        */
        void addRoundedRect(const RectF& rect, double rx, double ry);

        /** @brief Adds an ellipse as a new subpath.
        */
        void addEllipse(const PointF& topLeft, const SizeF& size);

        /** @brief Adds an arc as a new subpath.

            Starts a new subpath at the arc start point.  Angle and
            sweep-direction semantics are identical to %arcTo().
        */
        void addArc(const PointF& topLeft, const SizeF& size,
                    double degBegin, double degEnd);

        /** @brief Adds a pie segment as a new subpath.
        */
        void addPie(const PointF& topLeft, const SizeF& size,
                    double degBegin, double degEnd);

        /** @brief Adds a chord as a new subpath.
        */
        void addChord(const PointF& topLeft, const SizeF& size,
                      double degBegin, double degEnd);

        /** @brief Adds a polyline as a new subpath.
        */
        void addPolyline(const PointF* points, std::size_t count);

        /** @brief Adds a polygon as a new closed subpath.
        */
        void addPolygon(const PointF* points, std::size_t count);

        /** @brief Applies a transform to all path coordinates.
        */
        void transform(const Transform& transform);

        /** @brief Returns a copy of the path with the transform applied.
        */
        Path toTransformed(const Transform& transform) const;

        /** @brief Flattens one subpath into @a polygon starting at @a it.

            Skips a leading MoveTo, then flattens elements into @a polygon
            until a Close command, the next MoveTo, or end() is reached.
            No closing point is appended. The caller must clear @a polygon
            before each call.

            @returns iterator to the start of the next subpath, or end().
        */
        Iterator getPolygon(Iterator it, Polygon& polygon, float tolerance = 0.25f) const;

    private:
        void detach();

    private:
        SmartPtr<PathData> _pathData;
};

/* @internal @brief Path entry.
*/
class PathEntry
{
    public:
        typedef Path::ElementType Type;

    public:
        PathEntry(Type type, std::size_t n)
        : _type(type)
        , _size(n)
        {
        }

        Type type() const
        {
            return _type;
        }

        std::size_t size() const
        {
            return _size;
        }

    private:
        Type         _type;
        std::size_t  _size;
};

/** @brief Read-only view of a path element.
    @ingroup Pt-Gfx-Drawing
*/
class PathElement 
{
    friend class PathIterator;

    protected:
        PathElement(const PathEntry* entry, const PointF* points)
        : _entry(entry)
        , _pos(0.0, 0.0)
        , _points(points)
        {
        }
        
        void setPosition(const PointF& pos)
        {
            _pos = pos;
        }

        void setEntry(const PathEntry* entry, const PointF* points)
        {
            _entry = entry;
            _points =  points;
        }
    
    public:
        /** @brief Returns the element type.
        */
        Path::ElementType type() const
        {
            return _entry->type();
        }
        
        /** @brief Returns the number of points stored in the element.
        */
        std::size_t size() const
        {
            return _entry->size();
        }

        /** @brief Returns the current path position before the element.
        */
        const PointF& position() const
        {
            return _pos;
        }

        /** @brief Returns one point of the element.
        */
        const PointF& point(std::size_t n) const
        {
            return _points[n];
        }

        /** @brief Flattens the element into polygon points.
        */
        void flatten(Polygon& points, double tolerance = 0.25) const;

    private:
        const PathEntry*  _entry;
        PointF            _pos;
        const PointF*     _points;
};

/** @brief Forward iterator over path elements.
    @ingroup Pt-Gfx-Drawing
*/
class PathIterator
{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Path::Element;
        using difference_type = std::ptrdiff_t;
        using pointer = const Path::Element*;
        using reference = const Path::Element&;

        /** @brief Constructs an end iterator.
        */
        PathIterator()
        : _entry(0)
        , _points(0)
        , _element(0, 0)
        {
        }

        /** @brief Constructs an iterator for the given storage pointers.
        */
        PathIterator(const PathEntry* entry, const PointF* points)
        : _entry(entry)
        , _points(points)
        , _element(_entry, _points)
        {
        }

        /** @brief Returns the current path element.
        */
        const Path::Element& operator*() const 
        {
            return _element;
        }

        /** @brief Returns a pointer to the current path element.
        */
        const Path::Element* operator->() const 
        {
            return &_element;
        }

        /** @brief Advances to the next path element.
        */
        PathIterator& operator++() 
        {
            if( _entry->size() > 0)
            {
                const PointF& pos = _points[ _entry->size() - 1 ];
                _element.setPosition(pos);
            }

            _points += _entry->size();
            ++_entry;
            
            _element.setEntry(_entry, _points);
            return *this;
        }

        /** @brief Advances to the next path element and returns the previous iterator.
        */
        PathIterator operator++(int) 
        {
            PathIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /** @brief Returns true if both iterators refer to the same element.
        */
        bool operator == (const PathIterator& other) const 
        { 
            return _entry == other._entry; 
        }
        
        /** @brief Returns true if both iterators refer to different elements.
        */
        bool operator != (const PathIterator& other) const 
        { 
            return _entry != other._entry; 
        }

        /** @brief Returns true if this iterator precedes the other iterator.
        */
        bool operator < (const PathIterator & other) const
        {
          return _entry < other._entry;
        }

    private:
        const PathEntry*  _entry;
        const PointF*     _points;
        Path::Element     _element;
};

/* @internal Path data.
*/
class PathData
{
    public:
        PathData()
        { }

        ~PathData()
        { }

        const PointF& currentPosition() const
        {
            return _position;
        }

        void setCurrentPosition(const PointF& p)
        {
            _position = p;
        }

        Path::Iterator begin() const 
        {
            return Path::Iterator( _entries.data(), _points.data() );
        }

        Path::Iterator end() const 
        {
            return Path::Iterator(_entries.data() + _entries.size(), _points.data() + _points.size());
        }

        std::size_t size() const
        {
            return _entries.size();
        }

        bool isEmpty() const
        {
            return _entries.empty();
        }

        void clear()
        {
            _entries.clear();
            _points.clear();
            _start = PointF();
            _position = PointF();
        }

        void append(const PathData& path);

        void moveTo(const PointF& pos);

        void lineTo(const PointF& pos);

        void quadTo(const PointF& cp, const PointF& to);

        void cubicTo(const PointF& cp1, const PointF& cp2, const PointF& to);

        void close();

        void transform(const Transform& tform);

    private:
        std::vector<PathEntry> _entries;
        std::vector<PointF>    _points;
        PointF                 _start;
        PointF                 _position;
};

} // namespace

} // namespace

#endif
