/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
	02110-1301  USA
*/

#ifndef Pt_Hmi_Layout_H
#define Pt_Hmi_Layout_H

#include <Pt/Hmi/Spacing.h>

namespace Pt {
namespace Hmi {

class Layout
{
	public:
		enum Type
		{
			Fixed,
			TopToButton,
			ButtomToTop,
			LeftToRight,
			RightToLeft,
		};
  
		enum Docking
		{
			None,
			Left,
			Top,
			Right,
			Bottom,
			Fill
		};

	public:
		Layout()
		: _type( Fixed )		
		, _dock( None )
		{
		}

		Type type() const 
		{
			return _type; 
		}

		void setType( Type& t )
		{
			_type = t;
		}

		const Spacing&  margin() const
		{
			return _margin;
		}

		Spacing&  margin()
		{
			return _margin;
		}

		void setMargin( const Spacing& m )
		{
			_margin = m;
		}

		const Spacing&  padding() const
		{
			return _padding;
		}

		Spacing&  padding()
		{
			return _padding;
		}

		void setPadding( const Spacing& p )
		{
			_padding = p;
		}

		Docking docking() const
		{
			return _dock;
		}

		void setDocking( Docking d )
		{
			_dock = d;
		}


	private:
		Type		_type;	
		Docking _dock;
		Spacing _margin;
		Spacing _padding;
};

}}

#endif
