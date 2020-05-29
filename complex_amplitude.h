#pragma once
#include <complex>
#include "bmp.h";
#include "out_field_type.h";
#include "direction.h";

class vortex;
class complex_amplitude {
	image_size size;
	scheme color;
	int count_RGB_channels;
	vector<vector<double>> gauss;
private:
	vector<vector<complex<double>>> pixels;
	void _FFT2D(int dir, int expansion);
	vector<complex<double>>& FFT1D(int dir, int m, vector<complex<double>>& transforming_vector);
	void add_channels(vector<unsigned char>& pixel, int count_RGB_channels);
public:
	complex_amplitude();
	complex_amplitude(const complex_amplitude& obj);
	complex_amplitude(BMP& amplitudeBMP, BMP& phaseBMP);
	complex_amplitude(vortex& vortex, image_size size, scheme color);
	void create_hole(vortex& vortex, double r);
	complex_amplitude& operator=(const complex_amplitude& obj);
	complex<double>& operator()(int row, int column);
	vector<complex<double>>& operator()(int& number, direction direction, vector<complex<double>>& temp_vector);
	scheme set_color(BMP& amplitudeBMP, BMP& phaseBMP, out_field_type output_color_of);
	void replace(vector<complex<double>>& vector, int number, direction direction);
	double get_max(out_field_type type);
	void norm(out_field_type type);
	vector<vector<complex<double>>>& gradient(vector<vector<complex<double>>>& grad, char var);
	double get_oam(BMP& oam, scheme color_scheme);
	double get_oam(BMP& oam);

	void write(string filename, out_field_type type, scheme color);
	void write(string filename, out_field_type type);

	void init_gauss(double r, double sigma);
	void FFT2D(int expansion);
	void IFFT2D(int expansion);
	void FresnelT(double rx, double ry, double distance, double wavelength, int expansion, int direction);
	//void FresnelT(double rx, double ry, double distance, double wavelength, int expansion);
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

