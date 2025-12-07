# Project 6: Final Project Gear Up

The project handout can be found [here](https://cs1230.graphics/projects/final/gear-up/).



We are essentially doing texture mapping but instead of adding it to the phong equation, you use the 0-1 value of the texture to change the direction
of the normal. 


relevant information for combining code: 

1. primitives with udpated vbos/vaos
2. in realtime, makeShape() binds shape vbo/vaos with updated values
2. in MainWindow, onUploadBump(), I convert the image to a one-channel grayscale image
3. realtime, bindTexture called whenever a new texture image is loaded
4. in settings, variable hasTexture if there is a texture active, and 
texture QImage object as well
5. **ignore paintTexture/paintBackground for now, just me trying to do color-grading
6. shaders, default frag/vert are the relevant bump mapping ones
7. in MainWindow, i added a slider for bumpDepth/tolerance, bumpDepth value in settings as well



to test, try putting a filepath directly into the bindTexture, and in the shader comment out the bumpmapping logic
just to see the texture flat on the surface of the primitives.  
