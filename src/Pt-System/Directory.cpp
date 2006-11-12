#include "Pt/System/Directory.h"

#include "DirectoryImpl.h"


namespace Pt {

namespace System {

DirectoryIterator::DirectoryIterator()
{
	_impl = new DirectoryIteratorImpl();
}


DirectoryIterator::DirectoryIterator(const char* path)
{
	_impl = new DirectoryIteratorImpl(path);
}


DirectoryIterator::DirectoryIterator(const DirectoryIterator& it)
{
	_impl = it._impl;
	_impl->ref();
}


DirectoryIterator::~DirectoryIterator()
{
	if( 0 == _impl->deref() ) {
		delete _impl;
	}
}


DirectoryIterator& DirectoryIterator::operator++()
{
	_impl->advance();
	return *this;
}


DirectoryIterator& DirectoryIterator::operator=(const DirectoryIterator& it)
{
	if (*this == it )
		return *this;

	if( 0 == _impl->deref() )
	{
		delete _impl;
	}

	_impl = it._impl;
	_impl->ref();

	return *this;
}

bool DirectoryIterator::operator==(const DirectoryIterator& it) const
{
	return *_impl == *(it._impl);
}


bool DirectoryIterator::operator!=(const DirectoryIterator& it) const
{
	return !( *_impl == *(it._impl) );
}


FileSystemNode& DirectoryIterator::operator*() const
{
	return _impl->node();
}

Directory::Directory(const std::string& path, mode m)
: _path(path.c_str())
{
	switch(m) {
		case UseExisting:
			if (!DirectoryImpl::exists(path.c_str()))
				throw IllegalArgument("Directory " + path + " does not exist.", PT_SOURCEINFO);
			break;
		case Create:
			if (!DirectoryImpl::exists(path.c_str()))
				DirectoryImpl::create(path.c_str());
			break;
		default: IllegalArgument("wrong mode for constructor", PT_SOURCEINFO);
			 break;
	}
}

bool Directory::exists() const
{
	return DirectoryImpl::exists(_path);
}

char Directory::separator()
{
	return DirectoryImpl::separator();
}

void Directory::remove()
{
	DirectoryImpl::remove(_path);
}

void Directory::move(const std::string& newname)
{
	DirectoryImpl::move(_path, newname);
	_path = newname;
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


} // namespace System

} // namespace Pt
