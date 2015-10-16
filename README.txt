CMPT 361
Assignment 3
Patrick Wang
April 7, 2014

To run the racetracer, execute /make and then:

./raycast [–u | –d] step_max <options>

where,
-d: default scene
-u: user scene
step_max: levels of recurrsion

<options>
+s: inclusion of shadows
+l: inclusion of reflection
+r: inclusion of refraction
+c: inclusion of chess board pattern
+f: enabling diffuse rendering using stochastic ray generations
+p: enabling super-sampling


Sources:

Ray Tracer tutorial: http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-1-writing-a-simple-raytracer/
Uniform Sphere point selection: http://mathworld.wolfram.com/SpherePointPicking.html
