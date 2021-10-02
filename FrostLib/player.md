# Player code

### Game loop
TODO:
floor float position to ints (subpixel to pixel)


#### Normal
*When Sonic is not airborne or rolling.*
1. ~~Check for special animations that prevent control (such as balancing).~~
2. ~~Check for starting a spindash.~~
3. Adjust Ground Speed based on current Ground Angle (Slope Factor).
4. Check for starting a jump.
5. Update Ground Speed based on directional input and apply friction/deceleration.
6. Check for starting ducking, balancing on ledges, etc.
7. Wall sensor collision occurs
    - Which sensors are used varies based on the the sensor activation.
    - Note: This occurs before Sonic's position physically moves, meaning he might not actually be touching the wall yet, the game accounts for this by adding Sonic's X Speed and Y Speed to the sensor's position.
8. Check for starting a roll
9. Handle camera boundaries (keep Sonic inside the view and kill sonic if he touches the kill plane)
10. Move Sonic
    - Update X Position and Y Position based on X Speed and Y Speed.
11. Floor sensor collision occurs.
    - Update Sonic's Ground Angle & position.
    - Adhere to level of terrain or become airborne if none found/too low.
12. Check for falling when Ground Speed is too low on walls/ceilings.
