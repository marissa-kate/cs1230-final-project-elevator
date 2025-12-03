
https://github.com/user-attachments/assets/4696289e-e06b-4946-8ea5-ce3187fced06
## Project 6: Final Project Gear up

The project handout can be found [here](https://cs1230.graphics/projects/realtime/1).

### Test Cases
particle_velocity.json (Tests velocity of particle movement working. Tests edge case of speed=0)


https://github.com/user-attachments/assets/49887b2b-6c73-4e42-97d6-1395e9482753



particle_lifetime.json (Tests lifetime of particles. Tests lifetime=0 particles, which don't show up but prove to be not crash causing)


https://github.com/user-attachments/assets/a843f9d8-389e-4aad-9616-3c79153ec199




particle_scale.json (Tests scale of particles, scale=0 is tested as well, which doesn't show up on screens but proves robostness of code)


https://github.com/user-attachments/assets/a029463a-4b7a-439c-a756-084e7fe20d16




particle_stress.json (MaxParticles set to 1,000,000 to show robustness)


https://github.com/user-attachments/assets/8f55de04-38e4-4f20-963b-d82b44c76e93



particle_edge.json (Max particles on one, minimal implementation)


https://github.com/user-attachments/assets/a1aa0a0a-9634-429c-a1a2-0f15bfdf6213




particle_multi.json (Different system settings showing up)


https://github.com/user-attachments/assets/b1ff1201-6d26-4c7e-a690-f0a0a4093dca




particle_audio.json (Sound reaction on and off tested)


https://github.com/user-attachments/assets/4ea6c774-a48b-4abe-965a-4b513155c2ed




lsystem_angle.json (testing many angles of lsystem)



https://github.com/user-attachments/assets/705e4828-efb9-4578-a81a-b4f60ff932e0




lsystem_forest.json (larger trees drew by different amount of particles)




https://github.com/user-attachments/assets/946cfd08-6788-4f6b-be0b-adecb7368eed




lsystem_iter.json (tests 0 iteration as well, resulting in a normal particle system)



https://github.com/user-attachments/assets/35942fa9-7b3f-4ae6-9e47-dab11c4dd438




lsystem_axiom.json (generating different shapes, having multiple bases)



https://github.com/user-attachments/assets/6bbe3a09-e558-4b4a-812c-3ea1bd193875





lsystem_rule.json (plant like system or fractale like system)



https://github.com/user-attachments/assets/f258f8ea-c1ff-4ff5-8ef4-56448951a18b





lsystem_weird.json (Can it ignore undefined letters?)



https://github.com/user-attachments/assets/8b229db2-1bf4-4d70-b940-272429a3362b


campath_loop.json (Looped continuously, animation being at the same place at start at back, working on all angles)



https://github.com/user-attachments/assets/5179ac12-f7ca-48e6-b063-34eeef95f8ac


campath_speed.json (Non constant time intervals)


https://github.com/user-attachments/assets/697588ef-d30c-467f-9315-42e9a44a9fb6



campath_orientation.json (Can the up vector and look vector change drastically)


https://github.com/user-attachments/assets/7cfdee61-26b9-42bd-8cc9-d7927c60f87f



campath_curve.json (Non-linear movement)


https://github.com/user-attachments/assets/13fed351-3adc-4504-9799-ea4ae673cb51




campath_stay.json (Stopping, edge case)



https://github.com/user-attachments/assets/fa5eaa8c-a607-41d2-8310-0785df212976



### Design Choices
Realtime.cpp acts as the central manager for the new features. I implemented particles, lsystems, and camera paths. Also for particles, I made them audio reactive using Qt multimedia. In sceneChanged(), it initializes a list of ParticleSystem objects and a CameraPath instance using data parsed from the JSON file by ScenefileReader. In timerEvent(), it retrieves real-time audio data (volume and frequency) from the AudioCapture class, updates the camera's position and orientation via CameraPath, and advances the simulation for all particle systems. paintGL() then issues the draw calls for these systems.
The ParticleSystem class handles the CPU-side simulation. It manages particle lifecycles, updates positions based on velocity and audio reactivity (using volume to drive speed and spread), and prepares the VBOs for Instanced Rendering. If L-System parameters are present, it utilizes the LSystem helper class (lsystem.cpp) to generate a recursive branching structure (skeleton) via string rewriting and turtle graphics, using the resulting segments as spawn points for the particles. This way, lsystem mechanics are integrated into particles.
The CameraPath class handles the mathematics of smooth camera movement. It implements Cubic Bézier interpolation for position (automatically calculating control points based on velocity to ensure smoothness across non-constant time intervals) and uses Quaternion Slerp for rotation to prevent gimbal lock, based on the paper.
In OpenGL, particle.vert handles the Billboarding logic (keeping quads facing the camera) and per-instance scaling. particle.frag renders the particles as soft, glowing circles and implements the "sparkle" effect by modulating transparency with a time-based sine wave.

### Collaboration/References
I used Geminmi heavily for conceptual questions, especially understanding the gaven guides and finding a way of implementing it, through discussion. I also asked to give debug codes when some codes didn't work. Also, I made json files using Gemini, as they were not that much of a creative work. 
Also for audio reactive particles, I generated codes related to Qt MultiMedia using Gemini, because this is not a scope for this class. 

### Known Bugs
Campath inconsistent when loaded?

### Extra Credit
I implemented stuff worth 140(120) points!

