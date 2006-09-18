#include "Pt/IO/IOManager.h"
using namespace Pt::IO;

#include <iostream>
using namespace std;


IOManager::IOManager()
{
	// open plugins?
}


IOManager::~IOManager()
{
	std::map<std::string, IOHandler*>::iterator it;
	for( it = _handlerMap.begin(); it != _handlerMap.end(); ++it ) {
		delete it->second;
	}

	_handlerMap.clear();
}


void IOManager::registerHandler(const std::string& protocol, IOHandler* handler)
{
	_handlerMap.insert( make_pair(protocol, handler) );
}


GetTask* IOManager::get(const Url& url) throw (IOError)
{
	IOHandler* handler = this->findHandler( url.protocol() );
	if( !handler ) {
		return 0;
	}

	return handler->get(url);
}


PutTask* IOManager::put(const Url& url) throw (IOError)
{
	IOHandler* handler = this->findHandler( url.protocol() );
	if( !handler ) {
		return 0;
	}

	return handler->put(url);
}


IOHandler* IOManager::findHandler(const std::string& protocol)
{
	std::map<std::string, IOHandler*>::iterator it = _handlerMap.find( protocol );
	if( it == _handlerMap.end() )
		return 0;

	return it->second;
}






