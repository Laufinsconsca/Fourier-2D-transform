#pragma once
#include <complex>
#include "bmp.h";
#include "field_type.h";
#include "direction.h";

class complex_amplitude {
	image_size size;
	scheme color;
	int count_RGB_channels;
private:
	vector<vector<complex<double>>> pixels;
	void _FFT2D(int dir, int expansion);
	vector<complex<double>>& FFT1D(int dir, int m, vector<complex<double>>& transforming_vector);
	void add_channels(vector<unsigned char>& pixel, int count_RGB_channels);
	vector<unsigned char> form_pixel(unsigned char value, scheme color);
	vector<unsigned char> form_pixel(unsigned char value);
public:
	complex_amplitude();
	complex_amplitude(const complex_amplitude& obj);
	complex_amplitude(BMP& amplitudeBMP, BMP& phaseBMP);
	complex_amplitude& operator=(const complex_amplitude& obj);
	complex<double>& operator()(int row, int column);
	vector<complex<double>>& operator()(int& number, direction direction, vector<complex<double>>& temp_vector);
	scheme set_color(BMP& amplitudeBMP, BMP& phaseBMP, field_type output_color_of);
	void replace(vector<complex<double>>& vector, int number, direction direction);
	double get_max(field_type type);
	void norm(field_type type);

	void write(string filename, field_type type, scheme color);
	void write(string filename, field_type type);

	void FFT2D(int expansion);
	void IFFT2D(int expansion);
	static boolean is_power_of_2(image_size size) {
		int greater_value = size.height > size.width ? size.height : size.width;
		int less_value = greater_value == size.width ? size.height : size.width;
		int m = 2;
		boolean appropriate_less_value = false;
		boolean appropriate_greater_value = false;
		do {
			m *= 2;
			if (greater_value == m) {
				appropriate_greater_value = true;
			}
			if (less_value == m) {
				appropriate_less_value = true;
			}
		} while (2 * m <= greater_value);
		return appropriate_less_value && appropriate_greater_value;
	};
	//vector<vector<double>>& split_real_and_imag(vector<complex<double>>& complex_vector, vector<vector<double>>& splitted_vector);
	//vector<complex<double>>& unite_real_and_imag(vector<vector<double>>& splitted_vector, vector<complex<double>>& united_vector);
	void circshift(int xshift, int yshift);
	void fftshift();
	void ifftshift();
};

