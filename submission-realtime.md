## Project 6: Final Project Gear up

The project handout can be found [here](https://cs1230.graphics/projects/realtime/1).

### Test Cases
particle_velocity.json (Tests velocity of particle movement working. Tests edge case of speed=0)

particle_lifetime.json (Tests lifetime of particles. Tests lifetime=0 particles, which don't show up but prove to be not crash causing)

particle_scale.json (Tests scale of particles, scale=0 is tested as well, which doesn't show up on screens but proves robostness of code)

particle_stress.json (MaxParticles set to 1,000,000 to show robustness)

particle_edge.json (Max particles on one, minimal implementation)

particle_multi.json (Different system settings showing up)

particle_audio.json (Sound reaction on and off tested)

lsystem_angle.json (testing many angles of lsystem)

lsystem_forest.json (larger trees drew by different amount of particles)

lsystem_iter.json (tests 0 iteration as well, resulting in a normal particle system)

lsystem_axiom.json (generating different shapes, having multiple bases)

lsystem_rule.json (plant like system or fractale like system)

lsystem_weird.json (Can it ignore undefined letters?)

campath_speed.json 


### Design Choices
Realtime.cpp acts as the central manager for the new features. I implemented particles, lsystems, and camera paths. Also for particles, I made them audio reactive using Qt multimedia. In sceneChanged(), it initializes a list of ParticleSystem objects and a CameraPath instance using data parsed from the JSON file by ScenefileReader. In timerEvent(), it retrieves real-time audio data (volume and frequency) from the AudioCapture class, updates the camera's position and orientation via CameraPath, and advances the simulation for all particle systems. paintGL() then issues the draw calls for these systems.
The ParticleSystem class handles the CPU-side simulation. It manages particle lifecycles, updates positions based on velocity and audio reactivity (using volume to drive speed and spread), and prepares the VBOs for Instanced Rendering. If L-System parameters are present, it utilizes the LSystem helper class (lsystem.cpp) to generate a recursive branching structure (skeleton) via string rewriting and turtle graphics, using the resulting segments as spawn points for the particles. This way, lsystem mechanics are integrated into particles.
The CameraPath class handles the mathematics of smooth camera movement. It implements Cubic Bézier interpolation for position (automatically calculating control points based on velocity to ensure smoothness across non-constant time intervals) and uses Quaternion Slerp for rotation to prevent gimbal lock, based on the paper.
In OpenGL, particle.vert handles the Billboarding logic (keeping quads facing the camera) and per-instance scaling. particle.frag renders the particles as soft, glowing circles and implements the "sparkle" effect by modulating transparency with a time-based sine wave.

### Collaboration/References
I used Geminmi heavily for conceptual questions, especially understanding the gaven guides and finding a way of implementing it, through discussion. I also asked to give debug codes when some codes didn't work. Also, I made json files using Gemini, as they were not that much of a creative work. 
Also for audio reactive particles, I generated codes related to Qt MultiMedia using Gemini, because this is not a scope for this class. 

### Known Bugs
none

### Extra Credit
I implemented stuff worth 140(120) points!

