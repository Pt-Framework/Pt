#include <Pt/Hmi/LabelController.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/LabelRenderer.h>

namespace Pt{
namespace Hmi{

LabelController::LabelController(LabelModel& model, LabelRenderer& renderer)
: PanelController(model, renderer)
{
}

LabelController::~LabelController()
{
}

}}