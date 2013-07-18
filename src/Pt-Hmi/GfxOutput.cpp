#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Gfx/ARgbImage.h>
#include "GfxOutputImpl.h"


namespace Pt{
namespace Hmi{

GfxOutput::GfxOutput()
: _impl(new GfxOutputImpl())
{
}

GfxOutput::~GfxOutput()
{
}

Pt::Gfx::Painter* GfxOutput::nativePainter()
{
	return _impl->nativePainter();
}

void GfxOutput::output(Pt::Hmi::Model* model)
{	
	_impl->output(model);
}

}}
