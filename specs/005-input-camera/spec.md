# Feature Specification: Input-Controlled Camera Component

**Feature Branch**: `005-input-camera`  
**Created**: March 4, 2026  
**Status**: Draft  
**Input**: User description: "Implement a basic camera component. This component should read from keyboard input and mouse input. It should handle WASD to move around and rotation through holding the mouse right click button. The camera should be implemented as a component extending the existing camera implementation. Input handling should be implemented as a subsystem that abstracts away movement and rotation through events. That is when I press "W" on the keyboard, the input subsystem should broadcast a "Move" event. Similarly it should broadcast a rotate event on mouse right click."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Basic Camera Movement (Priority: P1)

A user can control camera position using WASD keys to move through a 3D scene for basic navigation.

**Why this priority**: Core navigation is the foundation requirement - without it, no other camera functionality matters.

**Independent Test**: Can be fully tested by launching the application, pressing WASD keys, and observing smooth camera movement in 3D space.

**Acceptance Scenarios**:

1. **Given** the application is running with a 3D scene, **When** user presses W key, **Then** camera moves forward in the direction it's facing
2. **Given** the application is running, **When** user presses S key, **Then** camera moves backward relative to its facing direction
3. **Given** the application is running, **When** user presses A key, **Then** camera moves left (strafe) perpendicular to facing direction
4. **Given** the application is running, **When** user presses D key, **Then** camera moves right (strafe) perpendicular to facing direction
5. **Given** user is not pressing any movement keys, **When** camera is idle, **Then** camera position remains stationary

---

### User Story 2 - Camera Rotation Control (Priority: P1)

A user can control camera viewing direction using mouse right-click and drag to look around the 3D environment.

**Why this priority**: Rotation control is essential for effective navigation - movement without rotation creates a severely limited experience.

**Independent Test**: Can be fully tested by holding right mouse button, moving mouse, and verifying camera orientation changes smoothly.

**Acceptance Scenarios**:

1. **Given** the application is running, **When** user holds right mouse button and moves mouse horizontally, **Then** camera rotates left/right (yaw) following mouse movement
2. **Given** the application is running, **When** user holds right mouse button and moves mouse vertically, **Then** camera rotates up/down (pitch) following mouse movement  
3. **Given** user is holding right mouse button, **When** user releases right mouse button, **Then** camera rotation stops and cursor returns to normal behavior
4. **Given** user is not holding right mouse button, **When** user moves mouse, **Then** camera rotation does not occur

---

### User Story 3 - Combined Movement and Rotation (Priority: P2)

A user can simultaneously move and rotate the camera for fluid navigation through complex 3D environments.

**Why this priority**: Real-world usage requires combined movement and rotation - this enables natural first-person navigation patterns.

**Independent Test**: Can be tested by holding right mouse button while pressing WASD keys and verifying both movement and rotation work together smoothly.

**Acceptance Scenarios**:

1. **Given** user is holding right mouse button and moving mouse, **When** user presses WASD keys, **Then** both camera movement and rotation occur simultaneously
2. **Given** user is pressing movement keys, **When** user holds right mouse button and rotates view, **Then** movement direction updates relative to new camera orientation

---

### Edge Cases

- What happens when user presses multiple movement keys simultaneously (e.g., W+A for diagonal movement)?
- How does system handle rapid mouse movements during rotation to prevent jarring camera snaps?
- What occurs when camera rotation reaches maximum pitch angles (looking straight up/down)?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST extend existing camera implementation to create new input-controlled camera component
- **FR-002**: System MUST implement input subsystem that abstracts keyboard and mouse input into movement and rotation events, designed for future extensibility to support additional input methods (e.g., game controllers) without requiring camera component changes
- **FR-003**: Input subsystem MUST broadcast "Move" events when WASD keys are pressed with directional information
- **FR-004**: Input subsystem MUST broadcast "Rotate" events when right mouse button is held and mouse moves
- **FR-005**: Camera component MUST subscribe to and respond to Move events to translate camera position
- **FR-006**: Camera component MUST subscribe to and respond to Rotate events to adjust camera orientation
- **FR-007**: System MUST maintain smooth movement at consistent speed regardless of frame rate
- **FR-008**: System MUST provide smooth rotation that follows configurable mouse sensitivity (default: 0.1 degrees per pixel)
- **FR-009**: Camera movement MUST be relative to current camera orientation (forward/backward along camera's facing direction)
- **FR-010**: System MUST provide configurable movement speed with default of 5 units per second
- **FR-011**: System MUST handle multiple simultaneous inputs (movement + rotation) without conflicts
- **FR-012**: Input subsystem MUST only process input events when registered observers exist; unobserved input events MUST be silently ignored
- **FR-013**: System MUST combine simultaneous WASD key presses into normalized diagonal movement vectors to maintain consistent movement speed
- **FR-014**: Camera pitch rotation MUST be limited to ±89 degrees to prevent gimbal lock and maintain stable rotation behavior
- **FR-015**: Camera component MUST register event handlers with input subsystem during initialization to receive Move and Rotate events
- **FR-016**: Input subsystem MUST provide registration interface allowing components to subscribe to specific input event types
- **FR-017**: Input subsystem registration MUST complete during engine initialization, before game logic begins execution
- **FR-018**: Input abstraction MUST hide implementation details from game components (components receive semantic Move/Rotate events without knowledge of source devices)

### Key Entities *(include if feature involves data)*

- **InputEvent**: Base event class containing timestamp and input source information
- **MoveEvent**: Event containing movement direction vector and magnitude derived from WASD input
- **RotateEvent**: Event containing rotation delta values (pitch/yaw) derived from mouse movement
- **InputCameraComponent**: Extended camera component that processes input events and updates camera transform
- **InputSubsystem**: Core system that captures raw input and translates to semantic movement/rotation events

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: User can navigate a 3D scene using WASD keys with movement response time under 16ms (60fps responsiveness)
- **SC-002**: Camera rotation follows mouse movement with 1:1 sensitivity and no perceptible lag
- **SC-003**: Camera movement speed remains consistent regardless of frame rate variations (frame-rate independent movement)
- **SC-004**: 100% of WASD key presses result in corresponding camera movement in correct direction
- **SC-005**: Camera rotation works smoothly without jitter during continuous mouse movement
- **SC-006**: Combined movement and rotation operations work simultaneously without interference

## Assumptions

- Existing camera implementation provides basic transformation capabilities (position, rotation)
- Application runs in windowed or fullscreen mode where mouse input can be captured
- WASD keys are available and not reserved for other application functions
- Right mouse button is available for rotation control (not conflicting with context menus)
- Application has access to standard keyboard and mouse input APIs
- Frame rate is sufficient for responsive input handling (target 60fps minimum)
- Current iteration focuses only on keyboard/mouse input; game controller support is planned for future iterations
- Engine initialization phase provides opportunity for input subsystem setup before game logic starts

## Clarifications

### Session 2026-03-04

- Q: Should the input subsystem be designed for future extensibility to support different input methods? → A: Yes, input subsystem should be swappable to support future game controller input, but current iteration only implements keyboard/mouse
- Q: How should the system handle problematic input scenarios? → A: If an input has no observers then we ignore it - input reactions need to be observed by an actor
- Q: How should movement speed and mouse sensitivity be determined? → A: Configurable movement speed with reasonable defaults - Movement speed of 5 units/second, mouse sensitivity 0.1 degrees/pixel
- Q: What happens when user presses multiple movement keys simultaneously (e.g., W+A for diagonal movement)? → A: Combine movement vectors and normalize to maintain speed (W+A = forward-left diagonal)
- Q: What occurs when camera rotation reaches maximum pitch angles (looking straight up/down)? → A: Limit pitch to ±89 degrees to prevent gimbal lock (standard FPS camera behavior)
- Q: What should be the scope and mechanism of the event broadcasting system? → A: Direct subscription - the camera component at initialization should request the input subsystem and register event handlers for movement and rotation
- Q: When should input registration occur and how should implementation details be abstracted? → A: Input registration must occur before the game runs, and the engine should hide input implementation details (e.g., that input comes from keyboard) from game code
