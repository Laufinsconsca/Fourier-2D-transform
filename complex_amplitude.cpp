#define _USE_MATH_DEFINES
#include "complex_amplitude.h"
#include "vortex.h"
#include <math.h>

string _to_string(scheme color);

complex_amplitude::complex_amplitude() : color(scheme::gray), pixels(), count_RGB_channels(1) {}

complex_amplitude::complex_amplitude(const complex_amplitude& obj) : size(obj.size), color(obj.color), pixels(obj.pixels), count_RGB_channels(obj.count_RGB_channels) {}

complex_amplitude::complex_amplitude(BMP& amplitudeBMP, BMP& phaseBMP) : count_RGB_channels(phaseBMP.get_count_RGB_channels()) {
	image_size amplitude_size = amplitudeBMP.get_size();
	image_size phase_size = phaseBMP.get_size();
	if (amplitude_size != phase_size) {
		throw runtime_error("Inconsistent sizes");
	}
	size = amplitude_size;
	color = set_color(amplitudeBMP, phaseBMP, out_field_type::amplitude);
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
			double temp = sqrt(x * x + y * y);
			if (temp < size.width / 2) {
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

complex_amplitude::complex_amplitude(vortex& vortex, image_size size, scheme color) {
	this->color = color;
	this->count_RGB_channels = 4;
	this->size = size;
	double r = 5;
	init_gauss(r, 3);
	double hx, hy;
	pixels.reserve(size.height);
	hx = 2 * r / size.width;
	hy = 2 * r / size.height;
	pixels.reserve(size.height);
	double tp_n;
	bool tp_is_const = vortex.tp_is_const;
	if (tp_is_const) {
		tp_n = vortex.tp(0.);
	}
	vector<double> x(size.width);
	x.at(0) = -r;
	for (int i = 1; i < size.width; i++) {
		x.at(i) = x.at(i - 1) + hx;
	}
	vector<double> y(size.width);
	y.at(0) = -r;
	for (int i = 1; i < size.height; i++) {
		y.at(i) = y.at(i - 1) + hy;
	}
	for (int i = 0; i < size.height; i++) {
		pixels.push_back(vector<complex<double>>());
		pixels.at(i).reserve(size.width);
		for (int j = 0; j < size.width; j++) {
			double angle = atan2(-y.at(i), x.at(j));
			double t = (tp_is_const ? tp_n : vortex.tp(10 * exp(-x.at(round(j)) * x.at(round(j)) - y.at(round(i)) * y.at(round(i)))));
			pixels.at(i).push_back(polar(gauss.at(i).at(j), t * pow(angle, vortex.pow_fi)));
		}
	}
}

void complex_amplitude::init_gauss(double r, double sigma) {
	gauss.clear();
	double hx, hy, x, y;
	hx = 2 * r / size.width;
	hy = 2 * r / size.height;
	gauss.reserve(size.height);
	for (int i = 0; i < size.height; i++) {
		y = -r + i * hy;
		gauss.push_back(vector<double>());
		gauss.at(i).reserve(size.width);
		for (int j = 0; j < size.width; j++) {
			x = -r + j * hx;
			gauss.at(i).push_back((exp(-(x * x + y * y) / (2 * sigma * sigma))));
		}
	}
}

complex_amplitude& complex_amplitude::operator=(const complex_amplitude& obj){
	if (this == &obj) return *this;
	size = obj.size;
	color = obj.color;
	count_RGB_channels = obj.count_RGB_channels;
	vector<vector<complex<double>>> pixels(obj.pixels);
	return *this;
}

complex<double>& complex_amplitude::operator()(int row, int column){
	return this->pixels.at(row).at(column);
}

vector<complex<double>>& complex_amplitude::operator()(int& number, direction direction, vector<complex<double>>& temp_vector) {
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

scheme complex_amplitude::set_color(BMP& amplitudeBMP, BMP& phaseBMP, out_field_type output_color_of) {
	BMP& etalon = output_color_of == out_field_type::amplitude ? amplitudeBMP : phaseBMP;
	if (etalon.get_color() != scheme::color) {
		return etalon.get_color();
	}
	else {
		return scheme::gray;
	}
}

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

double complex_amplitude::get_max(out_field_type type) {
	double max = -1.7976931348623157e+308;
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
			}
		}
	}
	return max;
}

void complex_amplitude::norm(out_field_type type) {
	double max = get_max(type);
	for (vector<complex<double>> row : pixels) {
		for (complex<double> pixel : row) {
			pixel /= max;
		}
	}
}

void complex_amplitude::write(string filename, out_field_type type, scheme color) {
	boolean scheme_is_trivial = color == scheme::gray || color == scheme::only_blue_channel || color == scheme::only_green_channel || color == scheme::only_red_channel;
	if (!scheme_is_trivial) {
		cmap = get_cmap(color, cmap);
	}
	double max = 0;
	double output_value;
	this->color = color;
	if (type == out_field_type::amplitude || type == out_field_type::intensity) {
		max = get_max(type);
	}
	else if (type == out_field_type::argument) {
		max = 2 * M_PI;
	}
	else {
		throw runtime_error("Incorrect type");
	}
	vector<vector<vector<unsigned char>>> output_pixels;
	output_pixels.reserve(size.height);
	for (vector<complex<double>> row : pixels) {
		vector<vector<unsigned char>> output_row;
		output_row.reserve(size.width);
		for (complex<double> pixel : row) {
			unsigned char output_value;
			switch (type) {
			case amplitude: output_value = static_cast<unsigned char>(255 * abs(pixel) / max); break;
			case argument: output_value = static_cast<unsigned char>(255 * (pixel.imag() < 0 ? arg(pixel) + 2 * M_PI : arg(pixel)) * M_1_PI / 2); break;
			case intensity: output_value = static_cast<unsigned char>(255 * abs(pixel) * abs(pixel) / max); break;
			}
			//add_channels(output_pixel, count_RGB_channels);
			output_row.push_back(form_pixel(output_value));
		}
		output_pixels.push_back(output_row);
		output_row.clear();
	}
	BMP output(output_pixels, color);
	output.write(filename);
}

void complex_amplitude::write(string filename, out_field_type type) {
	write(filename, type, this->color);
}

vector<unsigned char> complex_amplitude::form_pixel(unsigned char value, scheme color) {
	vector<unsigned char> pixel;
	switch (color) {
		case scheme::zero: {
			throw runtime_error("Reserved color name");
		}
		case scheme::gray: {
			pixel.push_back(value);
			break;
		}
		case scheme::only_blue_channel: {
			pixel.push_back(value);
			pixel.push_back(0);
			pixel.push_back(0);
			break;
		}
		case scheme::only_green_channel: {
			pixel.push_back(0);
			pixel.push_back(value);
			pixel.push_back(0);
			break;
		}
		case scheme::only_red_channel: {
			pixel.push_back(0);
			pixel.push_back(0);
			pixel.push_back(value);
			break;
		}
		case scheme::richred: {
			pixel.push_back(value * value * value / (255 * 255));
			pixel.push_back(value * value * value / (255 * 255));
			pixel.push_back(value);
			break;
		}
		case scheme::blueviolet: {
			pixel.push_back(255 * exp(value/255 - value * value / (255 * 255)));
			pixel.push_back(value * value * value * value / (255 * 255 * 255));
			pixel.push_back(value);
			break;
		}
		default: {
			unsigned char char_value = round(255 - value);
			pixel.push_back(cmap.at(char_value).at(0));
			pixel.push_back(cmap.at(char_value).at(1));
			pixel.push_back(cmap.at(char_value).at(2));
			break;
		}
	}
	return pixel;
}

vector<unsigned char> complex_amplitude::form_pixel(unsigned char value) {
	return form_pixel(value, color);
}

void complex_amplitude::add_channels(vector<unsigned char>& pixel, int count_RGB_channels) {
	if (pixel.size() == 1 && count_RGB_channels == 3) {
		pixel.push_back(pixel.at(0));
		pixel.push_back(pixel.at(0));
	}
	if (pixel.size() == 3 && count_RGB_channels > 3) {
		for (int i = 3; i < count_RGB_channels; i++) {
			pixel.push_back(0);
		}
	}
}

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

void complex_amplitude::FFT2D(int expansion) {
	_FFT2D(1, expansion);
}

void complex_amplitude::IFFT2D(int expansion) {
	_FFT2D(-1, expansion);
}

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

void complex_amplitude::fftshift() {
	circshift(size.width/2, size.height/2);
}

void complex_amplitude::ifftshift() {
	circshift((size.width + 1) / 2, (size.height + 1) / 2);
}

map<unsigned char, vector<unsigned char>>& complex_amplitude::get_cmap(scheme color_scheme, map<unsigned char, vector<unsigned char>>& cmap) {
	string buf;
	ifstream in("cmap/" + _to_string(color_scheme) + ".cmap", ios::in);
	vector<unsigned char> buf_vector;
	if (!in.fail()) {
		for (int i = 0; i < 256; i++) {
			buf_vector = vector<unsigned char>(3);
			in >> buf;
			buf_vector.at(2) = stod(buf);
			in >> buf;
			buf_vector.at(1) = stod(buf);
			in >> buf;
			buf_vector.at(0) = stod(buf);
			cmap[i] = buf_vector;
			buf_vector.clear();
		}
	}
	else {
		throw runtime_error("Файл " + _to_string(color_scheme) + ".cmap не найден!");
	}
	in.close();
	return cmap;
}

string _to_string(scheme color) {
	switch (color) {
	case scheme::gray: return "gray";
	case scheme::only_blue_channel: return "blue";
	case scheme::only_red_channel: return "red";
	case scheme::only_green_channel: return "green";
	case scheme::dusk: return "dusk";
	case scheme::dawn: return "dawn";
	case scheme::fire: return "fire";
	case scheme::seashore: return "seashore";
	case scheme::kryptonite: return "kryptonite";
	case scheme::teals: return "teals";
	case scheme::rainbow: return "rainbow";
	default: return "zero";
	}
}
