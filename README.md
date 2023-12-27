#
# Computer Graphics, Autumn 2022
#

This is the complete code for the 2nd assignment of the Computer Graphics course, Autumn 2022, at University of Edinburgh. 
It dictates the class structure to be followed for the implementation of the actual raytracer in this assignment.

Example code for the provided libraries can be found in the examples/ folder, along with an example input file example.json.


To compile the code:

cd path/to/RayTracer
mkdir build
cd build
cmake ..
make


This compiles both the raytracer and the example programs, executables of which can be found in build/ folder.

To run the examples, do:

1. for the json parsing example:

./jsonexample ../examples/example.json

2. for the vector library example:

./vectorexample

3. for the raytracer provided:

./raytracer ../examples/example.json <path_to_output_img>/testout.ppm

The raytracer provided outputs a rendered image file. 
