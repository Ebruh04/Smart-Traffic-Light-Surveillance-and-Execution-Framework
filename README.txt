Smart Traffic Light Simulation (Raylib C++)

Introduction
------------
This project is a traffic light simulation built using Raylib in C++.
It demonstrates a realistic four-way intersection where:
- Each direction (North, East, South, West) has its own traffic light.
- Only one light is active at a time in a clockwise cycle.
- Pedestrian buttons allow safe walk signals.
- A faulty mode is implemented where lights can fail and the cycle adapts by skipping or speeding up.

Logic and Working
-----------------
1. Traffic Light Cycle
   - Only one traffic light turns GREEN at a time.
   - After the green phase ends, it switches to YELLOW briefly.
   - The next direction becomes GREEN in a clockwise sequence:
     North -> East -> South -> West -> repeat.
   - If a light is faulty, it is skipped in the cycle.
   - This ensures no two lights are green at the same time.

2. Pedestrian Mode
   - Each direction has its own pedestrian button.
   - When pressed, the system:
     a) Switches the current direction’s light into pedestrian mode (dark green).
     b) All other lights are forced RED.
     c) Pedestrian mode lasts for 5 seconds, after which normal cycling resumes.
   - If multiple pedestrian buttons are pressed, each is handled in the order of the cycle.

3. Faulty Mode
   - A direction can be marked as faulty.
   - When faulty, that light is skipped in the normal cycle.
   - This simulates a real-world scenario where a light is out of order.
   - The system automatically speeds up the cycle to avoid delays.

4. User Controls
   - Number keys (1, 2, 3, 4) act as pedestrian buttons for North, East, South, and West.
   - Pressing a key switches that direction into pedestrian mode.
   - Faulty mode is simulated in the code for testing and can be toggled in logic.

Code Explanation
----------------
- The program uses Raylib for rendering graphics.
- Each light is represented by a structure containing its state (RED, YELLOW, GREEN, PEDESTRIAN).
- A central controller updates the states based on a timer.
- The update logic checks:
  a) If a pedestrian button was pressed, override with pedestrian mode.
  b) If a light is faulty, skip it.
  c) Otherwise, continue the clockwise cycle.
- Drawing functions handle rendering each light’s color and pedestrian indicators.

Output
------
- A 2D simulation window shows four traffic lights arranged in a crossroad pattern.
- Lights change one by one in a clockwise manner.
- When a pedestrian button is pressed, the corresponding light switches to pedestrian mode.
- Faulty mode makes the system skip that light and move to the next.
- The simulation gives a clear, step-by-step representation of how modern traffic systems handle normal, pedestrian, and faulty conditions.

Conclusion
----------
This simulation provides an educational demonstration of traffic light systems with added realism:
- Clockwise cycling.
- Pedestrian crossing handling.
- Faulty mode management.

It can be expanded further by adding:
- Vehicle spawn and movement simulation.
- Emergency vehicle priority.
- Advanced pedestrian safety timers.
