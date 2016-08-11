#include <Pt/Hmi/Picture.h>
#include "PictureImpl.h"

namespace Pt{
namespace Hmi{


Picture::Picture()
: _impl( new PictureImpl())
{

}


Picture::Picture(const Gfx::Image& image, Gfx::RenderFlags::Type flags)
: _impl( new PictureImpl() )
{
  set(image, flags);
}


Picture::~Picture()
{
    delete _impl;
}


 bool Picture::empty() const
{
  return _impl->empty();
}

void Picture::set(const Gfx::Image& image,  Gfx::RenderFlags::Type flags)
{
    _impl->set( image, flags);
}

size_t Picture::width() const
{
  return _impl->width();
}

size_t  Picture::height() const
{
  return _impl->height();
}


}}
