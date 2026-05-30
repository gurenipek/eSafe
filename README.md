A low-level embedded systems project written in C for a microcontroller development board. Implements a fire-aware electronic safe with the following features:

- LCD screen management for status display
- PIN code entry and authentication
- Thermostat / temperature monitoring with fire detection logic
- Automated lock/unlock behaviour based on thermal state
- Direct hardware control via register-level programming, interrupt handling, and timer management

Technical details:
Written in bare-metal C — no abstractions or HAL libraries. Hardware interaction is handled directly through memory-mapped registers, with state machines managing the safe's operational modes and hardware interrupts driving the thermostat and input response.
Context:
Developed as coursework for an Embedded Systems course at Middle East Technical University.
