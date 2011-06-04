// cpptest.cpp: Hauptprojektdatei.

#include "stdafx.h"
#include <vector>
#include <iostream>
#include "Any.h"

int main(array<System::String ^> ^args)
{
	try
	{
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);

		std::cout << "vector[1]: " << vec[1] << std::endl;

		Pt::Any any;

		any = 1;
		std::cout << "any: " << Pt::any_cast<int>(any) << std::endl;
	} 
	catch(const std::exception& ex)
	{
		std::cerr << "error: " << ex.what() << std::endl;
	}
	
	return 0;
}
