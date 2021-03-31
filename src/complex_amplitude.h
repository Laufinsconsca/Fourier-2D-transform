/**************************************************************************************************
 * \file	bmpTest\complex_amplitude.h.
 *
 * \brief	Declares the complex amplitude class
 **************************************************************************************************/

#pragma once
#include <complex>
#include "bmp.h";
#include "out_field_type.h";
#include "direction.h";

class vortex;

class complex_amplitude {
	/** The size. */
	image_size size;
	/** The reference beam. */
	vector<vector<double>> ref_beam;
private:
	/** The pixels, that contain a complex amplitude as itself */
	vector<vector<complex<double>>> pixels;
	void _FFT2D(int dir, int expansion);
	vector<complex<double>>& FFT1D(int dir, int m, vector<complex<double>>& transforming_vector);
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
public:

	// Constructors
	complex_amplitude();
	complex_amplitude(const complex_amplitude& obj);
	complex_amplitude(BMP& amplitudeBMP, BMP& phaseBMP);
	complex_amplitude(vortex& vortex, image_size size);
	//The overridden operators.
	complex_amplitude& operator=(const complex_amplitude& obj);
	complex<double>& operator()(int row, int column);
	vector<complex<double>>& operator()(int& number, direction direction);
	//Write complex amplitude to a bitmap file.
	void write(string filename, out_field_type type, scheme color);
	void replace(vector<complex<double>>& vector, int number, direction direction);
	double get_max(out_field_type type);
	double get_min(out_field_type type);
	void norm(out_field_type type);

	vector<vector<complex<double>>>& gradient(char var, vector<vector<complex<double>>>& grad);
	double* get_oam(BMP& oam);
	void create_hole(vortex& vortex, double r);
	void init_gauss(double r, double sigma);

	void FFT2D(int expansion);
	void IFFT2D(int expansion);
	void FresnelT(double rx, double ry, double distance, double wavelength, int expansion, int direction);

	void circshift(int xshift, int yshift);
	void fftshift();
	void ifftshift();
};

