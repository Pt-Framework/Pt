#include <Pt/Hmi/PanelModel.h>

namespace Pt{
namespace Hmi{

PanelModel::PanelModel()
: DefineProperty(BorderStyle,BorderStyleType::Single)
, DefineProperty(BorderWidth,3)
, DefineProperty(BorderRoundEdge,false)
{
	AcceptFocus.set(false);
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
