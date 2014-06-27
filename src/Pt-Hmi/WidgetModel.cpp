#include <Pt/Hmi/WidgetModel.h>

namespace Pt{
namespace Hmi{

WidgetModel::WidgetModel()
: DefinePropertyInitMacro(Caption,"")
, DefinePropertyInitMacro(UseMnemonic,true)
{
	registerProperty(Caption);
	registerProperty(UseMnemonic);
}

WidgetModel::~WidgetModel()
{
}

std::string WidgetModel::getMnemonicKey() const
{
	std::string mnemonic = "";

	int index = Caption.get().find('&');
	
	if( index < 0 || ((index + 1)> Caption.get().size()))
		return mnemonic;


	mnemonic = "A//";	
	mnemonic+= std::tolower(Caption.get()[index + 1]);
	return mnemonic;	
}

}}