#include <iostream>
#include "bmp.h"
#include "complex_amplitude.h"

using namespace std;

int main() {
	string input_amplitude_name("gauss.bmp");
	string input_phase_name("phase.bmp");
	BMP amplitude(input_amplitude_name);
	BMP phase(input_phase_name);
	complex_amplitude test_image(amplitude, phase);
	test_image.FFT2D(4); // 4 - FFT expansion, this parameter allows you to zoom in on the field
	test_image.write("fourier.bmp", field_type::argument, scheme::red);
}