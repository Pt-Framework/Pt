#include <iostream>

int main( int argc, char* argv[])
{
	if( argc >= 2)
	{
		std::cout << argv[1];
		return 0;
	}
	
	return 1;
}