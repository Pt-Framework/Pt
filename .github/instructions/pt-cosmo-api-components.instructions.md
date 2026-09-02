---
description: "Cosmo Components, Activation, and Startup"
---

- Define a component base class, initialize component metadata, query component interfaces:
  `include/Pt/Cosmo/Component.h`
- Implement component interface discovery, feature IDs, class IDs, and type queries:
  `include/Pt/Cosmo/IComponentType.h`
- Define the common base class for component interfaces:
  `include/Pt/Cosmo/IUnknown.h`
- Create component plugins, register interfaces, dependencies, feature IDs, and factories:
  `include/Pt/Cosmo/ComponentPlugin.h`
- Implement the component plugin contract, create and destroy component instances:
  `include/Pt/Cosmo/IComponentPlugin.h`
- Set component operation modes, including automatic startup:
  `include/Pt/Cosmo/ComponentMode.h`
- Manage component registration, manifests, loading, unloading, and enumeration:
  `include/Pt/Cosmo/ComponentManager.h`
- Implement the component manager lifecycle and component enumeration contract:
  `include/Pt/Cosmo/IComponentManager.h`
- Access registered component plugin, declaration, status, feature, and dependency metadata:
  `include/Pt/Cosmo/ComponentInfo.h`
- Describe component instances declared in an application manifest:
  `include/Pt/Cosmo/IComponentDeclaration.h`
- Provide runtime component status and dependency information:
  `include/Pt/Cosmo/IComponentStatus.h`
- Activate and deactivate a component after its required dependencies resolve:
  `include/Pt/Cosmo/IActivate.h`
- Resolve mandatory or optional component dependencies by feature ID and interface:
  `include/Pt/Cosmo/IActivator.h`
- Inspect the feature ID, target instance, and resolution state of a dependency:
  `include/Pt/Cosmo/IDependencyInfo.h`
- React to optional components becoming available or unavailable at runtime:
  `include/Pt/Cosmo/IConnect.h`
- Implement application launch and exit behavior for the main component:
  `include/Pt/Cosmo/ILauncher.h`
- Access command-line arguments supplied to a Cosmo application:
  `include/Pt/Cosmo/IMain.h`