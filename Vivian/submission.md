## Project 6: Final Project Gear Up

The project handout can be found [here](https://cs1230.graphics/projects/final/gear-up).

### Test Cases

Insert your test cases here. Be sure to include images/videos as needed!

![orange](student_outputs/bump-mapping/orange.png)
[orange_bump-slider.mov](student_outputs/bump-mapping/orange_bump-slider.mov)

![texture_cone_depth10](student_outputs/bump-mapping/texture_cone_depth10.png)
![texture_cone2_depth10](student_outputs/bump-mapping/texture_cone2_depth10.png)

![texture_cube_depth10](student_outputs/bump-mapping/texture_cube_depth10.png)
![texture_cube2_depth10](student_outputs/bump-mapping/texture_cube2_depth10.png)

![texture_cyl_depth10](student_outputs/bump-mapping/texture_cyl_depth10.png)
![texture_cyl2_depth10](student_outputs/bump-mapping/texture_cyl2_depth10.png)

![texture_sphere_depth10](student_outputs/bump-mapping/texture_sphere_depth10.png)
![texture_sphere2_depth10](student_outputs/bump-mapping/texture_sphere2_depth10.png)

![phong_total_depth0](student_outputs/bump-mapping/phong_total_depth0.png)
![phong_total_depth10](student_outputs/bump-mapping/phong_total_depth10.png)
![phong_total_depth50](student_outputs/bump-mapping/phong_total_depth50.png)
![phong_total_depth-10](student_outputs/bump-mapping/phong_total_depth-10.png)
![phong_total-50](student_outputs/bump-mapping/phong_total-50.png)

[phong-total_bump-slider.mov](student_outputs/bump-mapping/phong-total_bump-slider.mov)

![phong_total_inverse](scenefiles/post-process/phong_total_inverse.png)
![phong_total](scenefiles/post-process/phong_total.png)


### Design Choices

Implemented bumpmapping and setup the post-processing pipeline with the invert filter. (will be implementing color grading for final)

I wanted the bumps to be interactive, added a tolerance slider that switches between outward bumps and inward bumps

both bump-mapping and the inversion filter are controlled by a boolean variable in settings, which detects whether or not that effect is being used.


### Collaboration/References
n/a

### Known Bugs
n/a

### Extra Credit
n/a
