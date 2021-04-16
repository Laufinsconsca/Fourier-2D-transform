/**********************************************************************************************//**
 * @file	bmpTest\complex_amplitude.cpp.
 *
 * Implements the complex amplitude class.
 **************************************************************************************************/

#define _USE_MATH_DEFINES
#include "complex_amplitude.h"
#include "vortex.h"
#include <math.h>
#include <functional>
#include <algorithm>

/** Default constructor. */
complex_amplitude::complex_amplitude() : pixels() {}

/**********************************************************************************************//**
 * Copy constructor.
 *
 * @parameters	obj	The object.
 **************************************************************************************************/

complex_amplitude::complex_amplitude(const complex_amplitude& obj) : size(obj.size), pixels(obj.pixels) {}

/**********************************************************************************************//**
 * Constructor.
 *
 * @exceptions	runtime_error	Raised when a amplitude image size is inconsistent with phase image size
 *
 * @parameters	amplitudeBMP	[in,out] The amplitude bitmap.
 * @parameters	phaseBMP		[in,out] The phase bitmap.
 **************************************************************************************************/

complex_amplitude::complex_amplitude(BMP& amplitudeBMP, BMP& phaseBMP) {
	image_size amplitude_size = amplitudeBMP.get_size();
	image_size phase_size = phaseBMP.get_size();
	if (amplitude_size != phase_size) {
		throw runtime_error("Inconsistent sizes");
	}
	size = amplitude_size;
	double amplitude, phase;
	int i = 0, j = 0, x, y;
	vector<vector<vector<unsigned char>>>::iterator amplitude_row = amplitudeBMP.first_row();
	vector<vector<vector<unsigned char>>>::iterator phase_row = phaseBMP.first_row();
	vector<vector<unsigned char>>::iterator amplitude_column;
	vector<vector<unsigned char>>::iterator phase_column;
	pixels.reserve(size.height);
	vector<complex<double>> row;
	while (!(amplitude_row == amplitudeBMP.last_row())) {
		x = i++ - size.height/2;
		amplitude_column = (*amplitude_row).begin();
		phase_column = (*phase_row).begin();
		while (!(amplitude_column == (*amplitude_row).end())) {
			y = j++ - size.width/2;
			amplitude = amplitudeBMP.apply_scheme(*amplitude_column++);
			phase = 2 * M_PI * double(phaseBMP.apply_scheme(*phase_column++)) / 255;
			//double temp = sqrt(x * x + y * y);
			if (sqrt(x * x + y * y) < size.width / 2) {
				row.push_back(polar(amplitude, phase));
			}
			else {
				row.push_back(polar(0., 0.));
			}
		}
		j = 0;
		pixels.push_back(row);
		row.clear();
		amplitude_row++;
		phase_row++;
	}
}

/**********************************************************************************************//**
 * Constructor.
 *
 * @parameters	vortex	[in,out] The vortex.
 * @parameters	size  	The size.
 **************************************************************************************************/

complex_amplitude::complex_amplitude(vortex& vortex, image_size size) : complex_amplitude(vortex, gauss(0.6, 0), size) {}

/**********************************************************************************************//**
 * Constructor.
 *
 * @parameters	vortex	[in,out] The vortex.
 * @parameters	ref_beam  	The reference beam.
 **************************************************************************************************/

complex_amplitude::complex_amplitude(vortex& vortex, gauss gauss_beam, image_size size) {
	this->size = size;
	this->ref_beam = init_gauss(gauss_beam);
	double hx, hy;
	this->pixels.reserve(size.height);
	hx = 2. / size.width;
	hy = 2. / size.height;
	vector<double> x_r(size.width);
	x_r.at(0) = -1;
	for (int i = 1; i < size.width; i++) {
		x_r.at(i) = x_r.at(i - 1) + hx;
	}
	vector<double> y_r(size.height);
	y_r.at(0) = -1;
	for (int i = 1; i < size.height; i++) {
		y_r.at(i) = y_r.at(i - 1) + hy;
	}
	for (int i = 0; i < size.height; i++) {
		vector<complex<double>> row;
		row.reserve(size.width);
		for (int j = 0; j < size.width; j++) {
			if (pow(x_r.at(j), 2) + pow(y_r.at(i), 2) < 1) {
				double angle = atan2(-y_r.at(i), x_r.at(j));
				double tp = vortex.tp(sqrt(x_r.at(round(j)) * x_r.at(round(j)) + y_r.at(round(i)) * y_r.at(round(i))));
				row.push_back(polar(ref_beam.at(i).at(j), tp * (angle > 0 ? pow(angle, vortex.pow_fi) : pow(angle + 2 * M_PI, vortex.pow_fi))));
			}
			else {
				row.push_back(0);
			}
		}
		this->pixels.push_back(row);
		row.clear();
	}
	create_hole(vortex, 1);
}

/**********************************************************************************************//**
 * Creates a hole.
 *
 * @parameters	vortex	[in,out] The vortex.
 * @parameters	r	  	The relative width of an image (by default r = 5).
 **************************************************************************************************/

void complex_amplitude::create_hole(vortex& vortex, double r) {
	double fi = vortex.fi*2*M_PI/360 + M_PI_2;
	double n = size.width;
	double t_cos = (n + cos(fi) * vortex.r_d * n / r) / 2;
	double t_sin = (n + sin(fi) * vortex.r_d * n / r) / 2;
	for (int i = round(t_cos - vortex.r_hole * n / r); i < round(t_cos + vortex.r_hole * n / r); i++) {
		for (int j = round(t_sin - vortex.r_hole * n / r); j < round(t_sin + vortex.r_hole * n / r); j++) {
			if (pow(i - t_cos, 2) + pow(j - t_sin, 2) < pow(vortex.r_hole * n / r, 2)) {
				pixels.at(i).at(j) = 0;
			}
		}
	}
}

/**********************************************************************************************//**
 * Assignment operator.
 *
 * @parameters	obj	The object.
 *
 * @returns	A shallow copy of this object.
 **************************************************************************************************/

complex_amplitude& complex_amplitude::operator=(const complex_amplitude& obj){
	if (this == &obj) return *this;
	size = obj.size;
	vector<vector<complex<double>>> pixels(obj.pixels);
	return *this;
}

/**********************************************************************************************//**
 * Function call operator.
 *
 * @parameters	row   	The row.
 * @parameters	column	The column.
 *
 * @returns	The pixel at given location
 **************************************************************************************************/

complex<double>& complex_amplitude::operator()(int row, int column){
	return this->pixels.at(row).at(column);
}

/**********************************************************************************************//**
 * Function call operator.
 *
 * @parameters	number   	[in,out] Number of row/column).
 * @parameters	direction	The direction (row or column).
 *
 * @returns	The vector of the pixel as given row/columm
 **************************************************************************************************/

vector<complex<double>>& complex_amplitude::operator()(int& number, direction direction) {
	static vector<complex<double>> temp_vector;
	switch (direction) {
		case row: {
			for (int i = 0; i < size.width; i++) {
				temp_vector.push_back(pixels.at(number).at(i));
			}
			return temp_vector;
		}
		case column: {
			for (int i = 0; i < size.height; i++) {
				temp_vector.push_back(pixels.at(i).at(number));
			}
			return temp_vector;
		}
		default: return temp_vector;
	}
}

/**********************************************************************************************//**
 * Replaces.
 *
 * @exceptions	runtime_error	Raised when given pixels vector is inconsitent with the pixels vector at given location.
 *
 * @parameters	vector   	[in,out] The replacing vector.
 * @parameters	number   	Number of the row/column where the replaced vector is located.
 * @parameters	direction	The direction (row or column).
 **************************************************************************************************/

void complex_amplitude::replace(vector<complex<double>>& vector, int number, direction direction) {
	switch (direction) {
	case row: {
		if (vector.size() == size.width) {
			pixels.at(number) = vector;
		}
		else {
			throw runtime_error("Inconsistest dimensions");
		}
		break;
	}
	case column: {
		if (vector.size() == size.height) {
			for (int i = 0; i < size.height; i++) {
				pixels.at(i).at(number) = vector.at(i);
			}
		} else {
			throw runtime_error("Inconsistest dimensions");
		}
		break;
	}
	}
}

//double complex_amplitude::get_max(out_field_type type) {
//	double t, max = -1;
//	auto f = [=](complex<double> a) {
//		switch (type) {
//		case out_field_type::amplitude: {
//			return std::abs(a);
//		}
//		case out_field_type::intensity: {
//			return std::norm(a);
//		}
//		}
//	};
//	auto predicate = [=](complex<double> a, complex<double> b) {
//		switch (type) {
//			case out_field_type::amplitude: {
//				return abs(a) < abs(b);
//			}
//			case out_field_type::intensity: {
//				return std::norm(a) < std::norm(b);
//			}
//		}
//	};
//	for (vector<complex<double>> row : pixels) {
//		double t = f(*max_element(row.begin(), row.end(), predicate));
//		if (max < t) max = t;
//	}
//	return max;
//}

/**********************************************************************************************//**
 * Finds the max of the whole complex amplitude normalized to given field type.
 *
 * @parameters	type	[out] The out field type.
 *
 * @returns	The calculated maximum.
 **************************************************************************************************/

double complex_amplitude::get_max(out_field_type type) {
	double max = -1;
	for (vector<complex<double>> row : pixels) {
		for (complex<double> pixel : row) {
			switch (type) {
			case out_field_type::amplitude: {
				if (max < abs(pixel)) {
					max = abs(pixel);
				}
				break;
			}
			case out_field_type::intensity: {
				if (max < abs(pixel) * abs(pixel)) {
					max = abs(pixel) * abs(pixel);
				}
				break;
			}
			case out_field_type::phase: {
				return 2*M_PI;
			}
			}
		}
	}
	return max;
}

/**********************************************************************************************//**
 * Finds the min of the whole complex amplitude normalized to given field type.
 *
 * @parameters	type	[out] The out field type.
 *
 * @returns	The calculated minimum.
 **************************************************************************************************/

double complex_amplitude::get_min(out_field_type type) {
	double min = 1e308;
	for (vector<complex<double>> row : pixels) {
		for (complex<double> pixel : row) {
			switch (type) {
			case out_field_type::amplitude: {
				if (min > abs(pixel)) {
					min = abs(pixel);
				}
				break;
			}
			case out_field_type::intensity: {
				if (min > abs(pixel) * abs(pixel)) {
					min = abs(pixel) * abs(pixel);
				}
				break;
			}
			case out_field_type::phase: {
				return 0;
			}
			}
		}
	}
	return min;
}


/**********************************************************************************************//**
 * Normalizes the whole complex amplitude in relation to the given type.
 *
 * @parameters	type	[out] The out field type.
 **************************************************************************************************/

void complex_amplitude::norm(out_field_type type) {
	double max = get_max(type);
	for (vector<complex<double>> row : pixels) {
		for (complex<double> pixel : row) {
			pixel /= max;
		}
	}
}

/***************************************************************************************************
 * Get gradient of this for given variable.
 * 
 * @parameters	var 	The variable symbol that indicates direction ('x' or 'y').
 * @parameters  grad    A reference to a vector that will contain the gradient in the chosen direction.
 * 				
 * @exception	runtime_error	Raised when variable is undefined (isn't 'x' or 'y').
 *
 * @returns	A reference to a vector that contains the gradient in the chosen direction.
 **************************************************************************************************/

vector<vector<complex<double>>>& complex_amplitude::gradient(char var, vector<vector<complex<double>>>& grad) {
	grad.clear();
	switch (var) {
	case 'x': {
		grad.reserve(size.height);
		for (int i = 0; i < size.height; i++) {
			vector<complex<double>> row;
			row.reserve(size.width);
			for (int j = 0; j < size.width; j++) {
				if (j != size.width - 1) {
					row.push_back(pixels.at(i).at(j) - pixels.at(i).at(j + 1));
				}
				else {
					row.push_back(0);
				}
			}
			grad.push_back(row);
			row.clear();
		}
		break;
	}
	case 'y': {
		grad.reserve(size.height);
		for (int i = 0; i < size.height; i++) {
			vector<complex<double>> row;
			row.reserve(size.width);
			for (int j = 0; j < size.width; j++) {
				if (i != size.height - 1) {
					row.push_back(pixels.at(i).at(j) - pixels.at(i + 1).at(j));
				}
				else {
					row.push_back(0);
				}
			}
			grad.push_back(row);
			row.clear();
		}
		break;
	}
	default: throw runtime_error("Undefined variable");
	}
	return grad;
}

/**************************************************************************************************
 * Gets the OAM.
 * 
 * @parameters	oam	[in,out] A reference to the bitmap that will contain the OAM density
 * distribution of this complex amplitude.
 *
 * @param [in,out]	oam	The oam.
 *
 * @returns	The array, that contain total OAM, minimum of the OAM density image and maximum of the OAM density image (for taking possibility of negativity of the pixels of the OAM density into account).
 **************************************************************************************************/

double* complex_amplitude::get_oam(BMP& oam)  {
	vector<vector<double>> pixels;
	pixels.reserve(size.height);
	vector<vector<complex<double>>> gx, gy;
	gx = gradient('x', gx);
	gy = gradient('y', gy);
	for (int i = 0; i < size.height; i++) {
		vector<double> row;
		row.reserve(size.width);
		for (int j = 0; j < size.width; j++) {
			double width_2 = size.width / 2.;
			double height_2 = size.height / 2.;
			row.push_back(imag(conj(this->pixels.at(i).at(j)) * (gy.at(i).at(j) * (j - width_2) - gx.at(i).at(j) * (i - height_2))));
		}
		pixels.push_back(row);
		row.clear();
	}
	double oam_numerator = 0, oam_denominator = 0;
	for (int i = 0; i < size.height; i++) {
		for (int j = 0; j < size.width; j++) {
			oam_numerator += pixels.at(i).at(j);
			oam_denominator += std::norm(this->pixels.at(i).at(j));
		}
	}
	double max = 0;
	for (vector<double> row : pixels) {
		for (double pixel : row) {
			if (pixel > max) {
				max = pixel;
			}
		}
	}
	for (int i = 0; i < size.height; i++) {
		for (int j = 0; j < size.width; j++) {
			pixels.at(i).at(j) *= 255/max;
		}
	}
	double min = 1e308;
	max = 0;
	int i = 0;
	int j = 0;
	int i1 = 0;
	int j1 = 0;
	for (vector<double> source_row : pixels) {
		i++;
		for (double pixel : source_row) {
			j++;
			if (min > pixel) {
				min = pixel;
				i1 = i;
				j1 = j;
			}
		}
		j = 0;
	}
	for (int i = 0; i < pixels.size(); i++) {
		for (int j = 0; j < pixels.at(0).size(); j++) {
			pixels.at(i).at(j) -= min;
		}
	}
	for (vector<double> source_row : pixels) {
		for (double pixel : source_row) {
			if (max < pixel) {
				max = pixel;
			}
		}
	}
	for (int i = 0; i < pixels.size(); i++) {
		for (int j = 0; j < pixels.at(0).size(); j++) {
			pixels.at(i).at(j) *= 255 / max;
		}
	}
	BMP oam_(pixels, oam.get_color());
	oam = oam_;
	return new double[3] {oam_numerator / oam_denominator, min, max + min};
}

/**********************************************************************************************//**
 * Writes this object to a given location on the computer.
 *
 * @parameters	filename	Filename of the file.
 * @parameters	type		[out] The out field type.
 * @parameters	color   	the color scheme that the generated bitmap will have.
 **************************************************************************************************/

void complex_amplitude::write(string filename, out_field_type type, scheme color) {
	double max = 0;
	double output_value;
	if (type == out_field_type::amplitude || type == out_field_type::intensity) {
		max = get_max(type);
	}
	vector<vector<double>> output_pixels;
	output_pixels.reserve(size.height);
	for (vector<complex<double>> row : pixels) {
		vector<double> output_row;
		output_row.reserve(size.width);
		for (complex<double> pixel : row) {
			double output_value;
			switch (type) {
			case amplitude: output_value = 255 * abs(pixel) / max; break;
			case phase: output_value = 255 * (pixel.imag() < 0 ? arg(pixel) + 2 * M_PI : arg(pixel)) * M_1_PI / 2; break;
			case intensity: output_value = 255 * std::norm(pixel) / max; break;
			}
			//add_channels(output_pixel, count_RGB_channels);
			output_row.push_back(output_value);
		}
		output_pixels.push_back(output_row);
		output_row.clear();
	}
	BMP output(output_pixels, color);
	output.write(filename);
}

/**********************************************************************************************//**
 * 2-dimension fast Fourier transform.
 *
 * @exceptions	runtime_error	Raised when the size of this object is not power of 2.
 *
 * @parameters	dir		 	The direction of the transform (1 for direct, -1 for inverse).
 * @parameters	expansion	The expansion (should be power of 2 as well).
 **************************************************************************************************/

void complex_amplitude::_FFT2D(int dir, int expansion) {
	if (is_power_of_2(size)) {
		int i, j, t;
		t = 0;
		do { t++; } while ((1 << t) < size.height);
		t = 1 << t;
		int line_size = t * expansion;
		int st = (line_size - size.width) / 2;
		int end = st + size.width;
		vector<complex<double>> line(line_size);

		for (i = 0; i < size.height; i++) {
			for (j = 0; j < line_size; j++) {
				if ((j >= st) && (j < end))
					line.at(j) = pixels.at(i).at(j - st);
				else line.at(j) = 0;
			}
			line = FFT1D(dir, line_size, line);
			for (j = 0; j < size.width; j++) {
				pixels.at(i).at(j) = line.at(j + st);
			}
		}
		line.clear();

		t = 0;
		do { t++; } while ((1 << t) < size.width);
		t = 1 << t;
		line_size = t * expansion;
		st = (line_size - size.height) / 2;
		end = st + size.height;
		line = vector<complex<double>>(line_size);

		for (i = 0; i < size.width; i++)
		{
			for (j = 0; j < line_size; j++)
			{
				if ((j >= st) && (j < end))
					line.at(j) = pixels.at(j - st).at(i);
				else line.at(j) = 0;
			}
			line = FFT1D(dir, line_size, line);
			for (j = 0; j < size.height; j++) {
				pixels.at(j).at(i) = line.at(j + st);
			}
		}
		line.clear();
	}
	else {
		throw runtime_error("Invalid size");
	}
}

/**********************************************************************************************//**
 * 2-dimension direct fast Fourier transform.
 *
 * @parameters	expansion	The expansion (should be power of 2, allows you to increase an output Fresnel transform image).
 **************************************************************************************************/

void complex_amplitude::FFT2D(int expansion) {
	_FFT2D(1, expansion);
}

/**********************************************************************************************//**
 * 2-dimension inverve fast Fourier transform.
 *
 * @parameters	expansion	The expansion (should be power of 2, allows you to increase an output Fourier transform image).
 **************************************************************************************************/

void complex_amplitude::IFFT2D(int expansion) {
	_FFT2D(-1, expansion);
}

/**********************************************************************************************//**
 * Fresnel tranform.
 *
 * @parameters	rx		  	The rx.
 * @parameters	ry		  	The ry.
 * @parameters	distance  	The distance.
 * @parameters	wavelength	The wavelength.
 * @parameters	expansion 	The expansion (should be power of 2, allows you to increase an output Fourier transform image).
 * @parameters	direction 	The direction of the transform (1 for direct, -1 for inverse).
 **************************************************************************************************/

void complex_amplitude::FresnelT(double rx, double ry, double distance, double wavelength, int expansion, int direction) {
	int i, j;
	double x, y, hx, hy, t;
	complex<double> ex, ey;
	hx = rx / (size.width - 1);
	hy = ry / (size.width - 1);
	t = wavelength / (2 * distance) * direction;
	for (i = 0; i < size.height; i++) {
		ex = polar(1., pow(-ry / 2 + i * hy, 2) * t);
		for (j = 0; j < size.width; j++) {
			pixels.at(i).at(j) *= ex * polar(1., pow(-rx / 2 + j * hx, 2) * t);
		}
	}
	_FFT2D(direction, expansion);
}

//void complex_amplitude::FresnelT(double rx, double ry, double distance, double wavelength, int expansion) {
//	//wavelength /= 1e9;
//	int size = sqrt(this->size.width * this->size.height);
//	if (distance > 50 * size) {
//		FresnelT(rx, ry, distance, wavelength, 1, expansion);
//	}
//	else {
//		FresnelT(rx, ry, size * 100, wavelength, 1, expansion);
//		FresnelT(rx, ry, size * 100 - distance, wavelength, -1, expansion);
//	}
//}

/**********************************************************************************************//**
 * 1-dimension fast Fourier transform.
 *
 * @exceptions	runtime_error	Raised when the size of this object is not power of 2.
 *
 * @parameters	dir				   	The direction of the transform (1 for direct, -1 for inverse).
 * @parameters	size			   	The size.
 * @parameters	transforming_vector	[in,out] The transforming vector.
 *
 * @returns	A reference to a transforming vector.
 **************************************************************************************************/

vector<complex<double>>& complex_amplitude::FFT1D(int dir, int size, vector<complex<double>>& transforming_vector) {
	int i, k, m = 0;
	for (i = 1; i < size; i <<= 1, m++);
	if (i != size)
		throw runtime_error("Invalid size");
	float q = 1.;
	for (i = 0; i < size; i++, q = -q)
		transforming_vector.at(i) *= q;
	int n_2 = size / 2, n_1 = size - 1, j = 0;
	for (i = 0; i < n_1; i++) {
		if (i < j) {
			complex<double> t = transforming_vector.at(i);
			transforming_vector.at(i) = transforming_vector.at(j);
			transforming_vector.at(j) = t;
		}
		k = n_2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	int le = 1;
	float ed = 1;
	float c_M_PI = dir * M_PI;
	for (int l = 0; l < m; l++) {
		int le1 = le;
		le *= 2;
		complex<double> u = 1.;
		complex<double> w = polar(1., (double)(-c_M_PI / le1));
		for (j = 0; j < le1; j++, u *= w) {
			for (i = j; i < size; i += le) {
				int ip = i + le1;
				complex<double> t = transforming_vector.at(ip) * u;
				transforming_vector.at(ip) = transforming_vector.at(i) - t;
				transforming_vector.at(i) += t;
			}
		}
	}
	for (i = 0, q = 1. / (dir == 1 ? size : 1); i < size; i++, q = -q) {
		transforming_vector.at(i) *= q;
	}
	return transforming_vector;
}

/**********************************************************************************************//**
 * Circular shifts (isn't used, but used in the another Fourier transform algorithm that isn't presented in this version).
 *
 * @parameters	xshift	The xshift.
 * @parameters	yshift	The yshift.
 **************************************************************************************************/

void complex_amplitude::circshift(int xshift, int yshift) {
	vector<vector<complex<double>>> circshifted_pixel;
	for (int i = 0; i < size.width; i++) {
		circshifted_pixel.reserve(size.height);
		vector<complex<double>> temp;
		temp.reserve(size.width);
		for (int j = 0; j < size.height; j++) {
			temp.push_back(complex<double>(0, 0));
		}
		circshifted_pixel.push_back(temp);
	}
	for (int i = 0; i < size.width; i++) {
		int ii = (i + xshift) % size.width;
		for (int j = 0; j < size.height; j++) {
			int jj = (j + yshift) % size.height;
			circshifted_pixel.at(ii).at(jj) = pixels.at(i).at(j);
		}
	}
	pixels = circshifted_pixel;
}

/** Shifts for direct Fourier transform (isn't used, but used in the another Fourier transform algorithm that isn't presented in this version). */
void complex_amplitude::fftshift() {
	circshift(size.width/2, size.height/2);
}

/** Shifts for inverse Fourier transform (isn't used, but used in the another Fourier transform algorithm that isn't presented in this version). */
void complex_amplitude::ifftshift() {
	circshift((size.width + 1) / 2, (size.height + 1) / 2);
}
