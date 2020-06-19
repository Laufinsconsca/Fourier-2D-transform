#include <iostream>
#include <functional>
#include "bmp.h"
#include "complex_amplitude.h"
#include "vortex.h";
#include "multiple_processing.h"

using namespace std;
class vortex;
int main() {
	image_size size(256, 256);
	vortex vortex([](double r) { return 5; }, 1.5, 0,0,0);
	complex_amplitude test_vortex(vortex, size, scheme::rainbow);
	test_vortex.write("vortex.bmp", out_field_type::argument);
	test_vortex.FFT2D(8);
	BMP oam(scheme::fire);
	cout << test_vortex.get_oam(oam);
	oam.write("oam.bmp");
	test_vortex.write("intensity.bmp", out_field_type::intensity, scheme::fire);
	test_vortex.write("amplitude.bmp", out_field_type::amplitude, scheme::fire);
	test_vortex.write("phase.bmp", out_field_type::argument, scheme::rainbow);
	multiple_processing();
	system("pause>nul");
}