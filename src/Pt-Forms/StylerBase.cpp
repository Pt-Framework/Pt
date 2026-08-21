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

const std::size_t StylerBase::InvalidGeneration = std::size_t(-1);


StylerBase::StylerBase()
: _styleGeneration(InvalidGeneration)
, _optionsGeneration(InvalidGeneration)
, _localOptionsGeneration(InvalidGeneration)
, _hasOverrides(false)
, _customChanged(false)
{
}


Renderer* StylerBase::bind(const Style& style, const StyleOptions& options)
{
    const StyleOptionsBase& localOptions = onLocalOptions();

    if( isStyleChanged(style, localOptions) )
    {
        Renderer* renderer = 0;

        if( _custom )
        {
            renderer = _custom.get();
        }
        else
        {
            renderer = onStyleRenderer(style);

            if( renderer && localOptions.hasOverrides() )
                renderer = onCreateRenderer(style);
        }

        _renderer.reset(renderer);

        _customChanged = false;
        _styleGeneration = style.generation();
        _hasOverrides = localOptions.hasOverrides();
        _optionsGeneration = InvalidGeneration;
        _localOptionsGeneration = InvalidGeneration;
    }

    if( _renderer && isOptionsChanged(options, localOptions) )
    {
        if( _custom || _hasOverrides )
            onBindOptions(options);

        _optionsGeneration = options.generation();
        _localOptionsGeneration = localOptions.generation();
    }

    return _renderer.get();
}


bool StylerBase::isBound() const
{
    return _renderer != 0;
}


void StylerBase::apply(Renderer* renderer)
{
    _custom.reset(renderer);
    _customChanged = true;
}


bool StylerBase::isStyleChanged(const Style& style,
                                const StyleOptionsBase& localOptions) const
{
    if( ! _renderer )
        return true;

    if( _styleGeneration != style.generation() )
        return true;

    if( localOptions.hasOverrides() != _hasOverrides )
        return true;

    return _customChanged;
}


bool StylerBase::isOptionsChanged(const StyleOptions& options,
                                  const StyleOptionsBase& localOptions) const
{
    if( _optionsGeneration != options.generation() )
        return true;

    if( _localOptionsGeneration != localOptions.generation() )
        return true;

    return false;
}

} // namespace

} // namespace
