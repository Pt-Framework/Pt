#ifndef PT_GFX_EDGETABLE_H
#define PT_GFX_EDGETABLE_H
#include <algorithm>
#include <Edge.h>
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

        inline void addEdge( const Edge& edge )
        {  push_back( edge ); }


        inline void update( ssize_t ypos )
        {
            for( size_t i = 0; i < size(); i++ )
            {
                if( ypos >= (*this)[i].ymax )
                {
                    //
                    // remove finished edge
                    //
                    erase( begin() + i );
                    --i;
                }
                else
                {
                    //
                    // recalc new x value for the scanline
                    //
                    // NOTE: Yes, Laurentiu, it can really be that simple... ;)
                    //
                    Edge& edge = (*this)[i];

                    if (edge.m1 > 0) {
                        if (edge.d > 0) {
                            edge.x += edge.m1;
                            edge.d += edge.incr1;
                        }
                        else {
                            edge.x += edge.m;
                            edge.d += edge.incr2;
                        }
                    } else {
                        if (edge.d >= 0) {
                            edge.x += edge.m1;
                            edge.d += edge.incr1;
                        }
                        else {
                            edge.x += edge.m;
                            edge.d += edge.incr2;
                        }
                    }
               }
            }
        }

        inline void sort()
        { std::sort( begin(), end(), _lessXValue ); }

    private:
        struct LessXValue
        {
            inline bool operator()(const Edge& e1, const Edge& e2) const
            { return e1.x < e2.x; }
        };

        LessXValue _lessXValue;
};

}

}

#endif
