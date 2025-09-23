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

#ifndef PT_GFX_DASHER_H
#define PT_GFX_DASHER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Point.h>

#include <vector>
#include <cmath>
#include <algorithm>

namespace Pt {

namespace Gfx {

class Dasher 
{
    public:
        typedef std::vector<PointF> Dash;

    public:
        Dasher(const std::vector<double>& pattern)
            : _dashPattern(pattern)
            , _patternIdx(0)
            , _patternRemaining(0)
            , _isDash(true)
            , _isValid(false)
        {
            if (pattern.size() >= 2)
            {
                auto leqZero = [](double val) { return val <= 0; };
                auto it = std::find_if(pattern.begin(), pattern.end(), leqZero);
                _isValid = it == pattern.end();
            }

            if (_isValid) 
            {
                _patternRemaining = pattern[0];
            } 
        }

        void push(const PointF* points, std::size_t pointsSize) 
        {
            if (pointsSize < 2 || ! _isValid) 
                return;

            for (size_t segmentIdx = 0; segmentIdx < pointsSize - 1; ++segmentIdx) 
            {
                double segmentConsumed = 0;
                PointF p1 = points[segmentIdx];
                PointF p2 = points[segmentIdx + 1];
                double segLength = std::hypot(p2.x() - p1.x(), p2.y() - p1.y());
                double segRemaining = segLength;

                while (segRemaining > 0) 
                {
                    double lengthToUse = std::min(segRemaining, _patternRemaining);
                
                    if (_isDash) 
                    {
                        if (_currentDash.empty())
                        {
                            double t = segmentConsumed / segLength;
                            _currentDash.push_back(interpolate(p1, p2, t));
                        }

                        double t = (segmentConsumed + lengthToUse) / segLength;
                        PointF newPoint = interpolate(p1, p2, t);
                        _currentDash.push_back(newPoint);
                    
                        if (_patternRemaining <= segRemaining) 
                        {
                            _dashes.push_back(_currentDash);
                            _currentDash.clear();
                        }
                    }

                    _patternRemaining -= lengthToUse;
                    segmentConsumed += lengthToUse;
                    segRemaining -= lengthToUse;

                    if (_patternRemaining <= 0) 
                    {
                        _patternIdx = (_patternIdx + 1) % _dashPattern.size();
                        _patternRemaining = _dashPattern[_patternIdx];
                        _isDash = ! _isDash;
                    }
                }

                if (_isDash) 
                {
                    _currentDash.push_back(p2);
                }
            }
        }

        void finish() 
        {
            if (! _currentDash.empty()) 
            {
                _dashes.push_back(_currentDash);
                _currentDash.clear();
            }

            _patternIdx = 0;
            _patternRemaining = _isValid ? _dashPattern[0] : 0;
            _isDash = true;
        }

        void pop() 
        {
            _dashes.clear();
        }

        const std::vector<Dash>& getDashes() const 
        {
            return _dashes;
        }

    private:
        PointF interpolate(const PointF& p1, const PointF& p2, double t) const 
        {
            return PointF(p1.x() + t * (p2.x() - p1.x()), p1.y() + t * (p2.y() - p1.y()));
        }

    private:
        std::vector<double>   _dashPattern;
        size_t                _patternIdx; 
        double                _patternRemaining;
        bool                  _isDash;
        bool                  _isValid;
        Dash                  _currentDash;
        std::vector<Dash>     _dashes;
};

} // namespace

} // namespace

#endif

