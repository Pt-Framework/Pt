#include "Pt/AnyTraits.h"
using namespace Pt;

#include <iostream>
using namespace std;


void Pt::AnyTraits<bool>::output(std::ostream& os, const bool& value)
{
	os << std::boolalpha << value;
}


void Pt::AnyTraits<bool>::input(std::istream& is, bool& value)
{
	is >> std::boolalpha >> value;
}




void Pt::AnyTraits<int>::output(std::ostream& os, const int& value)
{
	os  << value;
}


void Pt::AnyTraits<int>::input(std::istream& is, int& value)
{
	is >> value;
}




void Pt::AnyTraits<float>::output(std::ostream& os, const float& value)
{
	os << value;
}


void Pt::AnyTraits<float>::input(std::istream& is, float& value)
{
	is >> value;
}



void Pt::AnyTraits<double>::output(std::ostream& os, const double& value)
{
	os << value;
}


void Pt::AnyTraits<double>::input(std::istream& is, double& value)
{
	is >> value;
}


