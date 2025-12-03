## Project 5: Realtime

The project handout can be found [here](https://cs1230.graphics/projects/realtime/1).

### Test Cases



### Design Choices
realtime.cpp is the heart of this code, binding VBO and VAO. In addition to existing lab codes, I implemented a helper function for projection matrixes, and then sent the nesessary data such as lighting and camera positions in paintGL(). paintGL has a loop for each primitives, sending out model matrixes and material datas for each primitves. updateAllShapeTesselations() update tesselations data, and sends them to primitive .cpp / .h classes to create tesselation. mouseMoveEvent and timerEvent handles mouse and keyboard movement. 
In OpenGL, default.frag computes lightdata from data sent from realtime.cpp, returning the final output color. default.vert computes positions and normals of objects.

### Collaboration/References
I used Geminmi heavily for conceptual questions, and I made sure I was on the right track. I sent my codes to ask if they are designed well everytime I made the code more complex. Also, I used it to give debug codes when I thought my lights were wrong- however, the lights were correct and I just needed to reset the light everytime scenes render. I noticed this thanks to an ed post.

### Known Bugs
none

### Extra Credit
I implemented stuff worth 140/120 points!

