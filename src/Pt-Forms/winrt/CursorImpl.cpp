/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "CursorImpl.h"
#include <Pt/Forms/Api.h>
#include <Windows.h>
#include <Pt/Forms/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Forms/Window.h>

namespace Pt{
namespace Forms{

CursorImpl::CursorImpl()
{	
}

CursorImpl::~CursorImpl()
{
}

void CursorImpl::setCursor(Cursors::Type c, Window* parent)
{
	_type = c;
	/*
	HINSTANCE hin = NULL; //System Instance

	_cursor = LoadCursor(hin, IDC_ARROW);

	switch(_type)
	{
		default:			
		break;
		case Cursors::Arrow:		
			_cursor = LoadCursor(hin, IDC_ARROW);
		break;
				
		case Cursors::Cross:
			_cursor = LoadCursor(hin, IDC_CROSS);
		break;

		case Cursors::Hand:
			_cursor = LoadCursor(hin, IDC_HAND);
		break;
		
		case Cursors::IBeam:
			_cursor = LoadCursor(hin, IDC_IBEAM);
		break;

		case Cursors::SizeNS:
			_cursor = LoadCursor(hin, IDC_SIZENS);
		break;		
		
		case Cursors::SizeWE:
			_cursor = LoadCursor(hin, IDC_SIZEWE);
		break;
	}

	SetCursor(_cursor);*/
}

Cursors::Type CursorImpl::getCursor() const
{
	return _type;
}

}}
