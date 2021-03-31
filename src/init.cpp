#include <iostream>
#include <functional>
#include "bmp.h"
#include "complex_amplitude.h"
#include "vortex.h";
#include "multiple_processing.h"
//#include "multiple_oam_calculation.h"

using namespace std;
class vortex;
int main() {
	image_size size(256, 256);
	//BMP phase("L:\\backup\\input\\tp=1\\pow_fi=1\\tp=1,pow_fi=1.bmp");
	//BMP beam("L:\\backup\\input\\gauss\\gauss.bmp");
	//complex_amplitude test_vortex(beam, phase);
	vortex vortex([](double r) { return 1; }, 1, 0, 0, 0);
	complex_amplitude test_vortex(vortex, size);
	test_vortex.write("vortex.bmp", out_field_type::argument, scheme::gray);
	test_vortex.FFT2D(16);
	BMP oam(scheme::fire);
	cout << test_vortex.get_oam(oam)[0];
	oam.write("oam.bmp");
	test_vortex.write("intensity.bmp", out_field_type::intensity, scheme::fire);
	test_vortex.write("amplitude.bmp", out_field_type::amplitude, scheme::fire);
	test_vortex.write("phase.bmp", out_field_type::argument, scheme::rainbow);
	system("pause>nul");
}

//int main() {
//	multiple_processing();
//}