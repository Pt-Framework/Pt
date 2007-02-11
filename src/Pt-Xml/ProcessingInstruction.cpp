#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/String.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

ProcessingInstruction::ProcessingInstruction()
: Node(Node::ProcessingInstruction)
{
}


ProcessingInstruction::~ProcessingInstruction()
{
}


const String& ProcessingInstruction::target() const
{
    return _target;
}


void ProcessingInstruction::setTarget(const String& target)
{
    _target = target;
}


const String& ProcessingInstruction::data() const
{
    return _data;
}


void ProcessingInstruction::setData(const String& data)
{
    _data = data;
}

} // namespace Xml

} // namespace Pt

