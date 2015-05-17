#include <Pt/Gfx/Image.h>

namespace Pt{
namespace Gfx2{


const ImageFormat& ImageFormat::rgb565()
{
	static const Rgb565Format f;
	return f;
}

const ImageFormat& ImageFormat::rgb888()
{
	static const Rgb888Format f;
	return f;

}

const ImageFormat& ImageFormat::argb8888()
{
	static const Argb8888Format f;
	return f;
}



}}