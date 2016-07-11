#include <Pt/Hmi/Picture.h>
#include "PictureImpl.h"

namespace Pt{
namespace Hmi{


Picture::Picture()
: _impl(0)
{

}


Picture::Picture(const Gfx::Image& image)
: _impl( new PictureImpl(image) )
{

}


Picture::~Picture()
{
    delete _impl;
}          


void Picture::set(const Gfx::Image& image)
{
 if( _impl ) 
    delete _impl;

    _impl = new PictureImpl( image);
}

}}
