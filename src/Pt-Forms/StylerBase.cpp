/*
  Copyright (C) 2015 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#include <Pt/Forms/StylerBase.h>

namespace Pt {

namespace Forms {

StylerBase::StylerBase()
: _styleGeneration(StyleOptions::InvalidGeneration)
, _optionsGeneration(StyleOptions::InvalidGeneration)
, _localOptionsGeneration(StyleOptions::InvalidGeneration)
, _isRenderer(false)
, _isOverride(false)
{
}


StylerBase::~StylerBase()
{
}


void StylerBase::init(Renderer* renderer)
{
    _isRenderer = renderer != 0;
    _renderer.reset(renderer);
}


Renderer* StylerBase::bind(const Style& style, const StyleOptions& styleOptions)
{
    StyleOptions& localOptions = onBindOptions(styleOptions);

    if( isStyleChanged(style, localOptions) )
    {
        Renderer* renderer = 0;

        if( _isRenderer )
        {
            // use specific renderer
            renderer = _renderer.get();
        }
        else if( localOptions.hasOptions() )
        {
            // cloned style renderer
            renderer = onCreateRenderer(style);
        }
        else
        {
            // default shared style renderer
            renderer = onStyleRenderer(style);
        }

        _renderer.reset(renderer);
        _styleGeneration = style.generation();
        _isOverride = localOptions.hasOptions();
        _optionsGeneration = StyleOptions::InvalidGeneration;
        _localOptionsGeneration = StyleOptions::InvalidGeneration;
    }

    if( _renderer && isOptionsChanged(styleOptions, localOptions) )
    {
        if( _isRenderer || _isOverride )
        {
            _renderer->prepare(localOptions);
        }

        _optionsGeneration = styleOptions.generation();
        _localOptionsGeneration = localOptions.generation();
    }

    return _renderer.get();
}


bool StylerBase::isBound() const
{
    return _renderer != 0;
}


bool StylerBase::isStyleChanged(const Style& style,
                                const StyleOptions& localOptions) const
{
    if( ! _renderer )
        return true;

    if( _styleGeneration != style.generation() )
        return true;

    if( _isOverride != localOptions.hasOptions() )
        return true;

    return false;
}


bool StylerBase::isOptionsChanged(const StyleOptions& options,
                                  const StyleOptions& localOptions) const
{
    if( _optionsGeneration != options.generation() )
        return true;

    if( _localOptionsGeneration != localOptions.generation() )
        return true;

    return false;
}

} // namespace

} // namespace
