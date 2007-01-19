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
        , dx(0)
        , dy(0)
        , xaccu(0)
        {}
                

        Edge( size_t  ymin_, size_t ymax_, size_t x_, size_t dx_, size_t dy_ )
        : ymin(ymin_)
        , ymax(ymax_)
        , x(x_)
        , dx(dx_)
        , dy(dy_)
        {}
                
        bool operator<(const Edge& edge) const
        {
            if( ymin == edge.ymin )
                return x < edge.x;
                
            return ymin < edge.ymin;              
        }
        
        Pt::ssize_t ymin;
        Pt::ssize_t ymax;
        Pt::ssize_t x;       
        Pt::ssize_t dx;
        Pt::ssize_t dy;
        Pt::ssize_t xaccu; 
};

}
}

#endif
