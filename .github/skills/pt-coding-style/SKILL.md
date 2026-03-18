---
name: pt-coding-style
description: Coding style and conventions for the Pt project. Use this when writing or reviewing C++ code in the Pt project.
---

# Coding Style

## Comments

- Comments in english
- Keep comments short and precise

## Indentation

- Use 4 spaces for indentation, no tabs
- Keep existing indentation style when modifying existing code

## Language Features

- Use C++14 features where appropriate

## Namespaces

- All production code lives in namespace `Pt` with a nested module namespace, e.g. `Pt::Gfx`
- Namespace closing braces are commented: `} // namespace Gfx`
