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
        , x(0.0)
        , rslope(0.0)
        {}
                

        Edge( size_t  ymin_, size_t ymax_, size_t x_, double rslope_ )
        : ymin(ymin_)
        , ymax(ymax_)
        , x(x_)
        , rslope(rslope_)
        {}
        
        
        bool operator<(const Edge& edge) const
        {
            if( ymin == edge.ymin )
                return x < edge.x;
                
            return ymin < edge.ymin;              
        }
        
        size_t ymin;
        size_t ymax;
        double x;       
        
        /**@brief Reciprocal slope 1/m */
        double  rslope;
};

}
}

#endif
