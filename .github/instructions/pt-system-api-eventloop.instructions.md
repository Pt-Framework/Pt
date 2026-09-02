---
description: "Event loops, timers and console applications"
---

- Console application, event loop, arguments, environment, cwd and system signals:
  `include/Pt/System/Application.h`
- Event loop, queue events, I/O multiplexing, run and exit:
  `include/Pt/System/EventLoop.h`
- Main-thread event loop:
  `include/Pt/System/MainLoop.h`
- Receive events from other threads:
  `include/Pt/System/EventSink.h`
- Send events to sinks in other threads:
  `include/Pt/System/EventSource.h`
- Periodic timeout notifications:
  `include/Pt/System/Timer.h`
- Attach operations to an event loop:
  `include/Pt/System/Selectable.h`
