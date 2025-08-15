Smart Traffic Light Surveillance and Execution Framework
---------------------------------------------------
1. OVERVIEW
---------------------------------------------------
This project simulates a smart traffic light system at a four-way intersection.
The simulation supports both normal traffic light operation and a pedestrian
mode that temporarily prioritizes pedestrians at a specific crossing.

The system operates in a **clockwise traffic cycle**:
    North -> East -> South -> West -> repeat

Only one traffic light is green at a time.

Pedestrians can press a button to override normal traffic operation,
turning their direction's light DARK GREEN for 5 seconds while all other
directions stay RED.

The program renders a graphical intersection using Raylib, including:
    - Traffic lights (red, yellow, green)
    - Pedestrian button controls
    - Countdown timers for each light
    - Background image (if available)

---------------------------------------------------
2. FEATURES
---------------------------------------------------
- **Four independent traffic lights**: North, East, South, West
- **Clockwise light cycle** with adjustable green/yellow/red durations
- **Pedestrian Mode**:
    - Activated by clicking the "PED" button below any traffic light
    - Light changes to DARK GREEN for pedestrians for 5 seconds
    - All other lights remain RED during pedestrian mode
    - Automatically resumes normal cycle after pedestrian phase
- **Custom background support**:
    - Background PNG can be loaded and resized to fit window
    - If missing, a default greenish background is shown
- **Real-time countdown timers** displayed on each light
- **Responsive mouse interaction** for pedestrian button clicks
- **Safe interruption**:
    - Cycle state is saved when pedestrian mode activates
    - Restored after pedestrian phase ends

---------------------------------------------------
3. FILES
---------------------------------------------------
- main.cpp            → Source code for simulation
- background.png      → Optional background image (your intersection map)
- [Recorded demo.mov] → Example screen recording of program in action

---------------------------------------------------
4. REQUIREMENTS
---------------------------------------------------
- C++17 compatible compiler (e.g., g++, clang++)
- Raylib library installed (version 4.x recommended)
- A PNG background image (optional, 1000x700 recommended for no scaling)

---------------------------------------------------
5. BUILD & RUN
---------------------------------------------------
1. Install Raylib (see: https://www.raylib.com/)
2. Compile with:
       g++ main.cpp -o traffic_sim -lraylib -std=c++17
3. Run:
       ./traffic_sim
4. If you want a custom background:
       - Place your PNG in the specified path inside `bgPath`
       - Match resolution to avoid scaling artifacts

---------------------------------------------------
6. CONTROLS
---------------------------------------------------
- **Mouse Click on PED Button**:
    Activates pedestrian mode for the chosen direction for 5 seconds.

- **Close Window**:
    Ends the simulation.

---------------------------------------------------
7. CODE STRUCTURE
---------------------------------------------------
- **TrafficLight class**:
    Represents a single light pole with independent timers and states.

- **IntersectionController class**:
    Handles the overall state machine:
        - Clockwise light cycling
        - Switching between green, yellow, red
        - Pedestrian mode activation & restoration
    Holds an array of 4 TrafficLight objects.

- **Main Loop**:
    1. Handles mouse input for pedestrian buttons
    2. Updates traffic light timers and states
    3. Draws background, lights, buttons, and timers

---------------------------------------------------
8. EXTENSIBILITY
---------------------------------------------------
This project can be expanded with:
- Vehicle detection sensors
- Variable pedestrian crossing times
- Sound alerts for visually impaired pedestrians
- Integration with real-time traffic data
- Multiple intersections with network control

---------------------------------------------------
9. NOTES
---------------------------------------------------
- If the background image is not found, a default green background is shown.
- All timing values (green, yellow, red, pedestrian) are defined in milliseconds
  and can be adjusted in the `TrafficLight` constructor or `pressPedestrian()` method.
- Traffic lights use DARK GREEN for pedestrian mode to distinguish from normal green.

---------------------------------------------------
10. DEMO
---------------------------------------------------
In the provided screen recording, you can see:
- Traffic lights changing in a clockwise sequence
- Clicking a pedestrian button instantly activates pedestrian mode
- Countdown timers updating in real time
- Background image overlayed under all objects

---------------------------------------------------
END OF README
===================================================
