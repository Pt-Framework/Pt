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

/* @brief Graphics path.
*/
class PT_GFX_API Path
{
    public:
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
        Path();

        ~Path();

        std::size_t size() const;

        bool isEmpty() const;

        Iterator begin() const;

        Iterator end() const;

        void clear();

        RectF boundingRect() const;

        const PointF& currentPosition() const;

        void moveTo(const PointF& p);

        void lineTo(const PointF& p);

        void curveTo(const PointF &cp, const PointF& to);

        void curveTo(const PointF &cp1, const PointF &cp2, const PointF& to);

        void quadTo(const PointF &cp, const PointF& to);

        void cubicTo(const PointF &cp1, const PointF &cp2, const PointF& to);

        /** @internal @brief Not implemented.
        */
        void bezierTo(const PointF* cps, size_t cn, const PointF& to);

        void arcTo(const PointF& p, double r);

        /** @brief closes the current subpath.
        */
        void close();

        /** @brief Adds a path as a new subpath.
        */
        void addPath(const Path& p);

        /** @brief Appends a path into the current subpath.
        */
        void appendPath(const Path& p);

        void addRect(const SizeF& size);

        void addRoundedRect(const SizeF& size, float radius);

        void addEllipse(const SizeF& size);

        void addPie(const SizeF& size, float degBegin, float degEnd);

        void addChord(const SizeF& size,  float degBegin, float degEnd);

        void transform(const Transform& transform);

        void toPolygons(std::vector<Polygon>& polygons, float smoothness = 1) const;

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

/* @brief Path element.
*/
class PathElement 
{
    friend class PathIterator;

    protected:
        PathElement(const PathEntry* entry, const PointF* points)
        : _entry(entry)
        , _points(points)
        {
        }
        
        void set(const PathEntry* entry, const PointF* points)
        {
            _entry = entry;
            _points =  points;
        }
    
    public:
        Path::ElementType type() const
        {
            return _entry->type();
        }
        
        std::size_t size() const
        {
            return _entry->size();
        }

        const PointF& point(std::size_t n) const
        {
            return _points[n];
        }

    private:
        const PathEntry*  _entry;
        const PointF*     _points;
};

/* @brief Iterator for path elements.
*/
class PathIterator
{
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Path::Element;
        using difference_type = std::ptrdiff_t;
        using pointer = const Path::Element*;
        using reference = const Path::Element&;

        PathIterator()
        : _entry(0)
        , _points(0)
        , _element(0, 0)
        {
        }

        PathIterator(const PathEntry* entry, const PointF* points)
        : _entry(entry)
        , _points(points)
        , _element(_entry, _points)
        {
        }

        const Path::Element& operator*() const 
        {
            return _element;
        }

        const Path::Element* operator->() const 
        {
            return &_element;
        }

        PathIterator& operator++() 
        {
            _points += _entry->size();
            ++_entry;
            
            _element.set(_entry, _points);
            return *this;
        }

        PathIterator operator++(int) 
        {
            PathIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator == (const PathIterator& other) const 
        { 
            return _entry == other._entry; 
        }
        
        bool operator != (const PathIterator& other) const 
        { 
            return _entry != other._entry; 
        }

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
        }

        void append(const PathData& path);

        void moveTo(const PointF& pos);

        void lineTo(const PointF& pos);

        void quadTo(const PointF& cp, const PointF& to);

        void cubicTo(const PointF& cp1, const PointF& cp2, const PointF& to);

        void bezierTo(const PointF* cps, size_t cn, const PointF& to);

        void close();

        void transform(const Transform& tform);

    private:
        std::vector<PathEntry> _entries;
        std::vector<PointF>    _points;
        PointF                 _position;
};

} // namespace

} // namespace

#endif
