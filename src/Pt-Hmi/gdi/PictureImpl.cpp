#include "PictureImpl.h"

namespace Pt{
namespace Hmi{

PictureImpl::PictureImpl(const Gfx::Image& image)
: _width( image.width() )
, _height( image.height() )
{


	for( size_t y = 0; y < image.height(); ++y )
	{
		for( size_t x = 0; x < image.width(); ++x )
		{
			const Gfx::Color& color = image.color( x, y );

			if( color.alpha() == 0 )
			{//Transparent
				_andMask.push_back( 0xff );
				_andMask.push_back( 0xff );
				_andMask.push_back( 0xff );				
				_andMask.push_back( 0xff );	

				_xorMask.push_back( 0 );
				_xorMask.push_back( 0 );
				_xorMask.push_back( 0 );
				_xorMask.push_back( 0 );
			}
			else
			{
				_andMask.push_back( 0 );
				_andMask.push_back( 0 );
				_andMask.push_back( 0 );
				_andMask.push_back( 0 );
				
                	
				_xorMask.push_back( (Pt::uint8_t) (color.blue() * 255.0));
                _xorMask.push_back( (Pt::uint8_t) (color.green()* 255.0) );
                _xorMask.push_back( (Pt::uint8_t) (color.red() * 255.0) );								
				_xorMask.push_back( 0xff);
			}
		}
	}
}


PictureImpl::~PictureImpl()
{

}

}}
