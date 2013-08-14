#include <Pt/Hmi/Desktop/Label.h>
#include <Pt/Hmi/LabelController.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/LabelRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Label::Label()
: _defController( new LabelController())
, _defModel( new LabelModel())
, _defRenderer( new LabelRenderer())
{
	_defController->setModel(_defModel);
	_defController->setRenderer(_defRenderer);	
	_defModel->ForeColor.set(Pt::Gfx::ARgbColor(160,160,160));
	_defModel->BorderWidth.set(1);
	_defModel->BorderStyle.set(BorderStyle::NoBorder);

	setController(*_defController);
}

Label::~Label()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
