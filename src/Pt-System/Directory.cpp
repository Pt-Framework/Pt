#include "Pt/System/Directory.h"

#include "DirectoryImpl.h"


namespace Pt {

namespace System {

Directory::Iterator::Iterator()
{
	_impl = new DirectoryIteratorImpl();
}


Directory::Iterator::Iterator(const char* path)
{
	_impl = new DirectoryIteratorImpl(path);
}


Directory::Iterator::Iterator(const Directory::Iterator& it)
{
	_impl = it._impl;
	_impl->ref();
}


Directory::Iterator::~Iterator()
{
	if( 0 == _impl->deref() ) {
		delete _impl;
	}
}


Directory::Iterator& Directory::Iterator::operator++()
{
	_impl->advance();
	return *this;
}


Directory::Iterator& Directory::Iterator::operator=(const Directory::Iterator& it)
{
	if( 0 == _impl->deref() ) {
		delete _impl;
	}

	_impl = it._impl;
	_impl->ref();

	return *this;
}


bool Directory::Iterator::operator==(const Directory::Iterator& it) const
{ 
	return *_impl == *(it._impl); 
}


bool Directory::Iterator::operator!=(const Directory::Iterator& it) const
{ 
	return !( *_impl == *(it._impl) );
}


/*
FileSystemNode& Directory::Iterator::node()
{
	return _impl->node();
}
*/


FileSystemNode& Directory::Iterator::operator*() const
{ 
	return _impl->node(); 
}


Directory Directory::create(const char* dirpath)
{
	DirectoryImpl::create(dirpath);
	return Directory(dirpath);
}


void Directory::remove(const char* dirpath)
{
	DirectoryImpl::remove(dirpath);
}


Directory Directory::current()
{
	std::string path = DirectoryImpl::current();
	return Directory( path.c_str() );
}


Directory Directory::system()
{
	std::string path = DirectoryImpl::system();
	return Directory( path.c_str() );
}


void Directory::changeCurrent(const char* dirpath)
{
	DirectoryImpl::changeCurrent(dirpath);
}

} // namespace System

} // namespace Pt
