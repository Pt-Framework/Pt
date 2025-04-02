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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */
#ifndef Pt_Forms_Dialog2_h
#define Pt_Forms_Dialog2_h

#include <Pt/Forms/Window.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/Panel.h>

namespace Pt{
namespace Forms{
namespace Demo{

class Dialog2 : public Pt::Forms::Window
{
	public:
		Dialog2();
		virtual ~Dialog2();

	protected:
		virtual void init();

	private:
		void onClosedByButton();
    
	private:
		Pt::Forms::PushButton _okButton;
		Pt::Forms::PushButton _cancelButton;	
		Pt::Forms::Label  _label;
		Pt::Forms::Panel  _okCancelPanel;
		Pt::Forms::Panel  _mainPanel;
};

}}}

#endif