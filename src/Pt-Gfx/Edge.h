#ifndef PT_GFX_EDGE_H
#define PT_GFX_EDGE_H

namespace Pt{
namespace Gfx{

class Edge
{
    public:
        Edge()
        : ymin(0)
        , ymax(0)
        , x(0)
        , m(0)
        , m1(0)
        , incr1(0)
        , incr2(0)
        , d(0)
        {}

        bool operator<(const Edge& edge) const
        {
            if( ymin == edge.ymin )
                return x < edge.x;

            return ymin < edge.ymin;
        }

        Pt::ssize_t ymin;  // minimum y of the edge
        Pt::ssize_t ymax;  // maxinum y of the edge
        int x;             // current x value
        int m, m1;         // slope and slope + 1
        int incr1, incr2;  // error increments
        int d;             // decision variable
};

}
}

#endif
