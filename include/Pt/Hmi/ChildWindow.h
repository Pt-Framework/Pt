/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
#ifndef Pt_Hmi_ChildWindow_H
#define Pt_Hmi_ChildWindow_H

#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Window.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API ChildWindow  : public Hmi::Window
{
	public:
		ChildWindow();	
		virtual ~ChildWindow();	

	public:  
		Property<double>							BorderWidth;	  
		Property<Pt::Gfx::ARgbColor>	BorderColor;
		Property<Pt::Gfx::ARgbColor>	FocusedColor;        
    
    const Panel* titleBar() const
    {
      if( ShowTitle.get() )
        return &_titelBar;
      else
        return 0;
    }

    Panel* titleBar()
    {
      if( ShowTitle.get() )
        return &_titelBar;
      else
        return 0;
    }

    const Pt::Gfx::SizeF&	 winSize() const
    {
        return _winSize;    
    }

	protected:
	  virtual void onRender();
    virtual void onInvalidate();		
    virtual void onSizeChanged(const Property<Gfx::SizeF>& prop);

  private:    
    void onFocusChanged(const Property<bool>& prop);        
    void onShowTitleBar(const Property<bool>& prop); 

  private:
    Panel _titelBar;
    Pt::Gfx::SizeF	 _winSize;    

};

}}

#endif