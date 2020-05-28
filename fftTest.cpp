#include <iostream>
#include "bmp.h"
#include "complex_amplitude.h"
#include "vortex.h";

using namespace std;
class vortex;
int main() {
	image_size size(512, 512);
	vortex vortex(5, 1, 3., 0.15, 30.);
	complex_amplitude test_vortex(vortex, size, scheme::rainbow);
	test_vortex.write("vortex.bmp", out_field_type::argument);
	test_vortex.FFT2D(8);
	test_vortex.write("intensity.bmp", out_field_type::intensity, scheme::fire);
	test_vortex.write("amplitude.bmp", out_field_type::amplitude, scheme::fire);
	test_vortex.write("phase.bmp", out_field_type::argument, scheme::rainbow);
}