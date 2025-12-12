# cs1230-final-project-elevator
elevator graphics


# Project Overview: 
In this project, multiple parameters are controlled by realtime keyboard input, making rendered scenes based on user interaction. Sound is produced from an external software also reacting to the same keyboard inputs, causing the scenefiles to be a visual representation of the auditory state. 
The four of us each are assigned to certain key inputs, and based on inputs from each other, we are improvising both sound and graphic.

# Design Choices: 
Pipeline Overview:

- **Initialize textures in three FBOs**: m_fbo, ppingpong fbos, and composite fbos.

- m_fbo creates 7 textures: store colors, bright colors, object normals, object position, material information (cDfifuse, cAmbient, cSpecular, shininess), and second depth (as color attachment). Also includes an 8th texture for depth as depth attachment.
  
- pinpong-fbos each creates 1 texture: stores pingpong color texture (blur)
  
- m_composite_fbo creates 2 textures: compopsite_color (combined final color and blur), and final_color texture (combined composite_color and color grading)

  
- **Physics & Textures are applied within the shapes at rendering time** 

(bind geometry + lighting fbo)

- **Geometry Pass** (geometry shader) - stores information into position, normal, and material-dependent textures. Samples information from the depth attachment stored during the depth test; stores the depth value into the second_depth texture (as a color attachment). (Do this to save the depth calculated during object-shape binding and not accidentally sample depth calculated while bound to fullscreen vao/vbo from depth test in future passes). 
- **Phong Lighting Pass** (phong shader) - samples information from textures stored from geometry pass to calculate lighting; stores information into color and bright textures
- **Particle Pass** (particle shader) 

(bind pingpong-fbos)

- **Screen Space Bloom Pass Looped** (blur shader) - samples information from bright color texture stored during the phong pass and its own pingpong textures; stores information iteratively into a ping-pong color texture for bloom

(bind composite_fbo)

- **Composite Pass** (composite shader) - samples information from pingppong texture stored during the screen space bloom pass and the information from the original color texture stored during Phong lighting pass; stores information into the composite/colorgrading texture
- **Post Processing Pass** (color grading shader) - samples information from the composite texture stored during the composite pass to apply color grading; stores information into the final color texture


(bind default fbo) (our default was set to 5)

- **Depth of Field Pass** (DOF shader) - generates mipmaps of info stored in the final color texture during composite pass, and samples information from the second depth texture stored during the first geometry pass to calculate the circle of confusion to get the corresponding mipmapped color from the information stored in the final color texture. Outputs final color to screen!

We required this many FBOs because the order in which we perform our passes was less flexible. We must first perform lighting to apply bloom, and then we must have the lighting calculated after bloom stored in a texture for depth of field to generate mipmaps. Any new information we wrote to an FBO after previously binding to the pingpong fbos would result in black; we resolved this issue by writing to a new fbo after the pingpong fbos.

## Ethan
- Depth-Buffers
- G-Buffers
- Depth of Field - the red laser observed in the video is the calculated focal plane; the red line appears by adding red to the final color if the object distance subtracted from the focal plane is less than epsilon (this is ideally where the focal plane meets the object distance, and where our "intended" focus is).
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



