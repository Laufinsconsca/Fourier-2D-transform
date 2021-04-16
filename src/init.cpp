#include <iostream>
#include <functional>
#include "bmp.h"
#include "complex_amplitude.h"
#include "vortex.h";
#include "gauss.h";
#include "multiple_processing.h"
#include "multiple_processing_gauss_shift.h"

using namespace std;
class vortex;
class gauss;
int main() {
	image_size size(256, 256);
	//BMP phase("L:\\backup\\input\\tp=1\\pow_fi=1\\tp=1,pow_fi=1.bmp");
	BMP beam("L:\\distributions\\input_512\\gauss_with_shift\\shift=0\\gauss,shift=0.bmp");
	//complex_amplitude test_vortex(beam, phase);
	vortex vortex([](double r) { return 3; }, 1, 0, 0, 0);
	gauss gauss( 0.6, 0.25);
	complex_amplitude test_vortex(vortex, gauss, size);
	test_vortex.write("vortex.bmp", out_field_type::phase, scheme::gray);
	test_vortex.write("gauss.bmp", out_field_type::amplitude, scheme::gray);
	test_vortex.FFT2D(16);
	BMP oam(scheme::fire);
	double* t = test_vortex.get_oam(oam);
	cout << t[0];
	oam.write("oam.bmp");
	test_vortex.write("intensity.bmp", out_field_type::intensity, scheme::fire);
	test_vortex.write("amplitude.bmp", out_field_type::amplitude, scheme::fire);
	test_vortex.write("phase.bmp", out_field_type::phase, scheme::rainbow);
	system("pause");
}

//int main() {
//	multiple_processing_gauss_shift();
//}