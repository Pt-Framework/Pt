#include <Pt/Hmi/PanelModel.h>

namespace Pt{
namespace Hmi{

PanelModel::PanelModel()
: BorderStyle(BorderStyle::Sizebale)
, BorderWidth(3)
, Invert3DEffect(false)
{
}

PanelModel::~PanelModel()
{
}

Pt::Gfx::SizeF PanelModel::clientSize() const
{
	Pt::Gfx::SizeF cs;

	cs.setWidth(Size.get().width() - BorderWidth.get());
	cs.setHeight(Size.get().height() - BorderWidth.get());
	return cs;
}

}}