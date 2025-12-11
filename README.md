# cs1230-final-project-elevator
elevator graphics


# Project Overview: 
In this project, multiple parameters are controlled by realtime keyboard input, making rendered scenes based on user interaction. Sound is produced from an external software also reacting to the same keyboard inputs, causing the scenefiles to be a visual representation of the auditory state. 
The four of us each are assigned to certain key inputs, and based on inputs from each other, we are improvising both sound and graphic.

# Design Choices: 
Pipeline Overview:
Physics & Textures are applied within the shapes at rendering time 
Geometry Pass (geometry shader)
Phong Lighting Pass (phong shader)
Particle Pass (particle shader)
Screen Space Bloom Pass Looped (blur shader)
Post Processing Pass (color grading shader)
Depth of Field Pass (DOF shader)

## Ethan
- Depth-Buffers
- G-Buffers
- Depth of Field
- Deferred Lighting
Ethan also managed our pipeline, determining the total number of FBOs and textures needed to complete our passes. 

## Yohta
- Audio Reactive class
Using Qt MultiMedia, the microphone signals are integrated into the code. The code in Audioreact.cpp then calculated the volume using FFT methods, then that value is used for particle velocity inside of particle.cpp
- Particles
Using particlesystem.cpp and particle.vert and frag, particles function independently and are merged simply.
- Camera paths
- L systems
  Same as proj 6


## Vivian
- Bump-mapping
Updated primitives to return VBOs with UV values and UV derivatives. Within the shader, the UV values and derivatives were taken in, along with a 0-1 color value of the single-channel image, to modify the direction and length of the surface normal, which in turn modified the way lighting affected the surface. 

- Color grading
Color grading shader takes in a shader2d object of the current image, as well as the shader3d LUT cube. Each color on the screen is mapped to a location in the LUT cube. 

A collection of 70 LUTs were uploaded to the resources folder. We randomly iterate through the folder of “Look LUTs” for this project. 
- Custom scenefile creation (visible in the resources folder, there are four, even though we were only able to showcase two in our demo video due to time constraints.

## Marissa
- Physics 
Rigid Body Translation & Rigid Body Rotation
Implemented with RK4 method and quaternions for rotation. Additionally, I added Inertia Tensors to each primitive which is dependent on their scale and shape(larger and less round objects have more inertia/resistance to change)

- Screen Space Bloom & Integration of the Pipeline
Screen space bloom alters the phong shader to output two texture color attachments and employs ping-pong buffers to blur the bright sections of the image, and then overlay it in the composite pass.

# Collaboration/References: 
- Depth Buffers - Daniel Ritchie and TAs!!
- Deferred-Lighting: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
- G Buffers: https://learnopengl.com/Advanced-Lighting/Deferred-Shading  and  lab 11!
- Depth of Field: https://developer.nvidia.com/gpugems/gpugems/part-iv-image-processing/chapter-23-depth-field-survey-techniques
- Bump-mapping 
https://history.siggraph.org/learning/efficient-bump-mapping-hardware-by-peercy-airey-and-cabral/ 
- Color grading
https://developer.nvidia.com/gpugems/gpugems2/part-iii-high-quality-rendering/chapter-24-using-lookup-tables-accelerate-color
Free LUT package from Color Grading Central
- For Audio reaction, we used Qt Multimedia. I used Gemini for assistance in this topic, asking logistics and debugging certain Qt based issues.
- Physics: https://www.cs.cmu.edu/~baraff/sigcourse/notesd1.pdf 
Inertia tensor https://ocw.mit.edu/courses/16-07-dynamics-fall-2009/dd277ec654440f4c2b5b07d6c286c3fd_MIT16_07F09_Lec26.pdf 
- Screen Space Bloom
https://learnopengl.com/Advanced-Lighting/Bloom

# Known Bugs: 
particles and primitives don't quite work together yet, so we rendered them on different screens to all integrate via the audio system.



