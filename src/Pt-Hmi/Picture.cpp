#include <Pt/Hmi/Picture.h>
#include "PictureImpl.h"

namespace Pt{
namespace Hmi{


Picture::Picture()
: _impl( new PictureImpl())
{

}


Picture::Picture(const Gfx::Image& image)
: _impl( new PictureImpl() )
{
  set(image);
}


Picture::~Picture()
{
    delete _impl;
}


 bool Picture::empty() const
{
  return _impl->empty();
}

void Picture::set(const Gfx::Image& image)
{
    _impl->set( image);
}

}}
