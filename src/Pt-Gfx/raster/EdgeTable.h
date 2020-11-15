/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_GFX_EDGETABLE_H
#define PT_GFX_EDGETABLE_H
#include <algorithm>
#include "Edge.h"
#include <set>
#include <iostream>

namespace Pt{
namespace Gfx{

typedef std::multiset<Edge> EdgeSet;

class ActiveEdgeTable : public std::vector<Edge>
{
    public:
        ActiveEdgeTable()
        { }

        void addEdge(const Edge& edge)
        {  
            // remove the edge which connects to the added edge, so we do
            // not fill a scanline twice and make it easier to keep the
            // edge table sorted correctly

            std::vector<Edge>::iterator it;
            for(it = begin(); it != end(); ++it)
            {
                // an edge connects to another edge if it has the same
                // current X coordinate and ymin of the added edge equals
                // ymax of a current edge (top-down rasterization)
                if(it->x == edge.x && it->ymax == edge.ymin)
                {
                    erase(it);
                    break;
                }
            }

            push_back(edge); 
        }

        void update(int scanLine)
        {
            for( size_t i = 0; i < size(); i++ )
            {
               Edge& edge = (*this)[i];

                if(edge.ymax <= scanLine )
                {
                    erase(begin() + i);
                    --i;
                }
                else
                {
                  if (edge.m1 > 0) 
                  {
                      if (edge.d > 0) 
                      {
                          edge.x += edge.m1;
                          edge.d += edge.incr1;
                      }
                      else {
                          edge.x += edge.m;
                          edge.d += edge.incr2;
                      }
                  } 
                  else 
                  {
                      if (edge.d >= 0) 
                      {
                          edge.x += edge.m1;
                          edge.d += edge.incr1;
                      }
                      else 
                      {
                          edge.x += edge.m;
                          edge.d += edge.incr2;
                      }                 
                  }
               }
            }
        }

        inline void update()
        {
          // recalc new x value for the scanline

            for( size_t i = 0; i < size(); i++ )
            {
               Edge& edge = (*this)[i];
               
                if (edge.m1 > 0) 
                {
                    if (edge.d > 0) 
                    {
                        edge.x += edge.m1;
                        edge.d += edge.incr1;
                    }
                    else {
                        edge.x += edge.m;
                        edge.d += edge.incr2;
                    }
                } 
                else 
                {
                    if (edge.d >= 0) 
                    {
                        edge.x += edge.m1;
                        edge.d += edge.incr1;
                    }
                    else 
                    {
                        edge.x += edge.m;
                        edge.d += edge.incr2;
                    }
                }
            }
        }
        
        inline void sort()
        {           
          std::sort( begin(), end(), _lessXValue ); 
        }
        
    private:
        struct LessXValue
        {
            inline bool operator()(const Edge& e1, const Edge& e2) const
            {                 
                return e1.x < e2.x;
            }
        };        

        LessXValue _lessXValue;
};

}}//namespace

#endif
