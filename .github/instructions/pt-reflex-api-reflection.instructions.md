---
description: "Reflection and Meta-Objects"
---

- Define reflected types, inheritance, constructors, methods, properties, and object lifetime hooks:
  `include/Pt/Reflex/Type.h`
- Register, unregister, and find reflected types, generic types, and global functions:
  `include/Pt/Reflex/TypeManager.h`
- Define and cache reflected specializations of a generic type:
  `include/Pt/Reflex/GenericType.h`
- Describe a named type and its generic type parameters for type lookup:
  `include/Pt/Reflex/TypeSpecifier.h`
- Pass dynamically typed values and iterate call argument lists:
  `include/Pt/Reflex/Argument.h`
- Convert reflective arguments and return values to C++ parameter types:
  `include/Pt/Reflex/ArgumentTraits.h`
- Describe constructor parameters and invoke a reflected constructor:
  `include/Pt/Reflex/ConstructorInfo.h`
- Wrap construction proxies for reflective constructor invocation:
  `include/Pt/Reflex/ConstructorProxy.h`
- Describe and invoke registered global functions:
  `include/Pt/Reflex/FunctionInfo.h`
- Wrap C++ free functions or custom function implementations for reflection:
  `include/Pt/Reflex/Function.h`
- Describe and invoke reflected object methods:
  `include/Pt/Reflex/MethodInfo.h`
- Wrap C++ member functions for reflective invocation:
  `include/Pt/Reflex/Method.h`
- Wrap free-function proxies that act on reflected objects:
  `include/Pt/Reflex/MethodProxy.h`
- Wrap generic methods that receive reflective arguments directly:
  `include/Pt/Reflex/GenericMethod.h`
- Describe a reflected property's name, type, getter, and setter operations:
  `include/Pt/Reflex/PropertyInfo.h`
- Wrap C++ member getter and setter functions as reflected properties:
  `include/Pt/Reflex/Property.h`
- Wrap free-function getter and setter proxies as reflected properties:
  `include/Pt/Reflex/PropertyProxy.h`