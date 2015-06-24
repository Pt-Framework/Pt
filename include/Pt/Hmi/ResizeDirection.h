/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * License along with this library; if not, write to the Free Software*/
#ifndef Pt_Hmi_ResizeDirection_H
#define Pt_Hmi_ResizeDirection_H

#include <string>
#include <stdexcept> 

namespace Pt{
namespace Hmi{

class ResizeDirection
{
  public:
	  enum Type
	  {
		  No,
		  North,
		  NorthEast,
		  East,
		  SouthEast,
		  South,
		  SouthWest,
		  West,
		  NorthWest
	  };		

    static std::string toString( ResizeDirection::Type t )
    {
      switch(t)
      {
      	case No:
          return "No";
		    
        case North:
          return "North";

		    case NorthEast:
          return "NorthEast";
		  
        case East:
          return "East";

		    case SouthEast:
          return "SouthEast";
		    
        case South:
          return "South";
		  
        case SouthWest:
          return "SouthWest";
		  
        case West:
          return "West";
		  
        case NorthWest:
          return "NorthWest";
      }

      throw std::logic_error("unknown resize direction");
      return "No";
    }

    static ResizeDirection::Type fromString( const std::string& t )
    {
      if( t == "No")
        return No;
		    
      if( t == "North")
        return North;

      if( t == "NorthEast")
        return NorthEast;

      if( t == "East")
        return East;

      if( t == "SouthEast")
        return SouthEast;

      if( t == "South")
        return South;

      if( t == "SouthWest")
        return SouthWest;

      if( t == "West")
        return West;

      if( t == "NorthWest")
        return NorthWest;
      
      throw std::logic_error("unknown resize direction");
      return No;
    }
};


}} //namespace
#endif
