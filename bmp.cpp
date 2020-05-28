#include "bmp.h"
#include "color_pixel.h"

BMP::BMP() {
	count_RGB_channels = 4;
	initHeader();
}

BMP::BMP(vector<vector<vector<unsigned char>>>& picture) : pixels(picture) {
	size.height = (int)picture.at(0).size();
	size.width = (int)picture.size();
	count_RGB_channels = picture.at(0).at(0).size();
	initHeader();
}

BMP::BMP(vector<vector<vector<unsigned char>>>& picture, scheme color) : BMP(picture) {
	this->color = color;
}

BMP::BMP(string filename) : BMP() {
	ifstream in(filename, ios::binary | ios::in);
	in.unsetf(ios_base::skipws);
	if (!in.fail()) {
		if (!(in >> *this)) throw runtime_error("File " + filename + " is empty or contains invalid data!");
	}
	else {
		throw runtime_error("File " + filename + " not found!");
	}
	in.close();
}

BMP::BMP(const BMP& obj){
	pixels = obj.pixels;
	count_RGB_channels = obj.count_RGB_channels;
	color = obj.color;
	size = obj.size;
	bmpFileHeader = obj.bmpFileHeader;
	bmpInfoHeader = obj.bmpInfoHeader;
	colorProfile = obj.colorProfile;
}

BMP& BMP::operator=(const BMP& obj) {
	if (this == &obj) return *this;
	pixels = obj.pixels;
	count_RGB_channels = obj.count_RGB_channels;
	color = obj.color;
	size = obj.size;
	bmpFileHeader = obj.bmpFileHeader;
	bmpInfoHeader = obj.bmpInfoHeader;
	colorProfile = obj.colorProfile;
	return *this;
}

const unsigned char& BMP::operator()(int const row, int const column, scheme const color){
	return apply_scheme(this->pixels.at(row).at(column), color);
}

unsigned char BMP::apply_scheme(vector<unsigned char> pixel, scheme color) {
	switch (color) {
	case scheme::gray: {
		if (this->color == color || this->color == scheme::only_blue_channel) {
			return pixel.at(0);
		} else if (this->color == scheme::only_green_channel) {
			return pixel.at(1);
		} else if (this->color == scheme::only_red_channel) {
			return pixel.at(2);
		} else {
			return static_cast<unsigned char>(0.11 * pixel.at(0) + 0.59 * pixel.at(1) + 0.3 * pixel.at(2));
		}
	}
	case scheme::only_blue_channel: {
		return pixel.at(0);
	}
	case scheme::only_green_channel: {
		return pixel.at(1);
	}
	case scheme::only_red_channel: {
		return pixel.at(2);
	}
	default: {
		throw runtime_error("Incorrect color name");
	}
	}
};

unsigned char BMP::apply_scheme(vector<unsigned char> pixel) {
	return apply_scheme(pixel, color);
}

void BMP::initHeader() {
	bmpFileHeader = { {0x4D42, 2}, {size.width * size.height * count_RGB_channels + BMPFILEHEADERsize + BMPINFOHEADERsize + COLORPROFILEsize, 4}, {0, 2}, {0, 2}, {BMPFILEHEADERsize + BMPINFOHEADERsize + COLORPROFILEsize, 4} };
	bmpInfoHeader = { {BMPINFOHEADERsize, 4}, {size.width, 4}, {size.height, 4}, {1, 2}, {count_RGB_channels * 8, 2}, {BI_BITFIELDS, 4}, {0, 4}, {0xEC4, 4}, {0xEC4, 4}, {0, 4}, {0, 4},
		{0x00FF0000, 4}, {0x0000FF00, 4}, {0x000000FF, 4}, {-16777216 /*0xFF000000*/, 4}, {LCS_WINDOWS_COLOR_SPACE, 4}, {0, 36}, {0, 4}, {0, 4}, {0, 4},
		{0, 4}, {0, 4}, {0, 4}, {0, 4} };
	colorProfile = { {0x000000FF, 4}, {0x0000FF00, 4}, {0x00FF0000, 4} };
}

vector<unsigned char> BMP::toBinary(vector<int> number){
	vector<unsigned char> binary;
	for (int i = 0; i < number.at(1); i++) {
		binary.push_back(number.at(0) >> (8 * i));
	}
	return binary;
}

istream& operator>>(istream& input, BMP& bmp) {
	/*<header reading>*/
	unsigned char buf;
	vector<unsigned char> buf_vector;
	input.seekg(10, input.cur);
	for (int i = 0; i < sizeof(int); i++) {
		if (!(input >> buf)) return input;
		buf_vector.push_back(buf);
	}
	int offset = bmp.toNumber(buf_vector);
	buf_vector.clear();
	input.seekg(4, input.cur);
	for (int j = 0; j < sizeof(int); j++) {
		if (!(input >> buf)) return input;
		buf_vector.push_back(buf);
	}
	bmp.size.width = bmp.toNumber(buf_vector);
	buf_vector.clear();
	for (int j = 0; j < sizeof(int); j++) {
		if (!(input >> buf)) return input;
		buf_vector.push_back(buf);
	}
	bmp.size.height = bmp.toNumber(buf_vector);
	buf_vector.clear();
	input.seekg(2, input.cur);
	for (int i = 0; i < sizeof(short); i++) {
		if (!(input >> buf)) return input;
		buf_vector.push_back(buf);
	}
	bmp.count_RGB_channels = bmp.toNumber(buf_vector) / 8;
	buf_vector.clear();
	input.seekg(long long(offset) - 30, input.cur);
	/*</header reading>*/
	/*<pixels reading>*/
	color_pixel etalon;
	boolean isMonochromatic = true;
	bmp.pixels.reserve(bmp.size.height);
	for (int i = 0; i < bmp.size.height; i++) {
		bmp.pixels.push_back(vector<vector<unsigned char>>());
		bmp.pixels.at(i).reserve(bmp.size.width);
		for (int j = 0; j < bmp.size.width; j++) {
			vector<unsigned char> pixel;
			pixel.reserve(bmp.count_RGB_channels);
			for (int k = 0; k < bmp.count_RGB_channels; k++) {
				if (!(input >> buf)) return input;
				pixel.push_back(buf);
			}
			if (bmp.count_RGB_channels != 1) {
				if (i == 0 && j == 0) {
					etalon = color_pixel(pixel);
				}
				else {
					isMonochromatic &= etalon == color_pixel(pixel) || scheme::zero == color_pixel(pixel).get_color();
				}
			}
			bmp.pixels.back().push_back(pixel);
		}
	}
	if (bmp.count_RGB_channels == 1) {
		bmp.color = scheme::gray;
	} else if (isMonochromatic) {
		bmp.color = etalon.get_color();
	}
	reverse(bmp.pixels.begin(), bmp.pixels.end());
	bmp.initHeader();
	return input;
}

vector<unsigned char> BMP::serialize(scheme color_scheme)
{
	if (color_scheme == scheme::gray) {
		bmpFileHeader.at(1).at(0) = size.width * size.height*3 + BMPFILEHEADERsize + BMPINFOHEADERsize + COLORPROFILEsize;
		bmpInfoHeader.at(4).at(0) = 24;
		count_RGB_channels = 3;
	}
	vector<unsigned char> serializedBMP;
	serializedBMP.reserve(bmpFileHeader.at(1).at(0));
	for (vector<int> data : bmpFileHeader) {
		for (unsigned char byte : toBinary(data)) {
			serializedBMP.push_back(byte);
		}
	}
	for (vector<int> data : bmpInfoHeader) {
		for (unsigned char byte : toBinary(data)) {
			serializedBMP.push_back(byte);
		}
	}
	for (vector<int> data : colorProfile) {
		for (unsigned char byte : toBinary(data)) {
			serializedBMP.push_back(byte);
		}
	}
	for_each(pixels.rbegin(), pixels.rend(), [&](vector<vector<unsigned char>> row) {
		unsigned char monochromatic_pixel;
		for (vector<unsigned char> pixel : row) {
			switch (color_scheme) {
			case scheme::only_blue_channel:
				serializedBMP.push_back(pixel.at(0));
				serializedBMP.push_back(0);
				serializedBMP.push_back(0);
				break;
			case scheme::only_green_channel:
				serializedBMP.push_back(0);
				serializedBMP.push_back(pixel.at(1));
				serializedBMP.push_back(0);
				break;
			case scheme::only_red_channel:
				serializedBMP.push_back(0);
				serializedBMP.push_back(0);
				serializedBMP.push_back(pixel.at(2));
				break;
			case scheme::gray:
				monochromatic_pixel = apply_scheme(pixel, color_scheme);
				for (int i = 0; i < 3; i++) {
					serializedBMP.push_back(monochromatic_pixel);
				}
				break;
			default: {
				for (int i = 0; i < 3; i++) {
					serializedBMP.push_back(pixel.at(i));
				}
			}
			}
			if (count_RGB_channels > 3) {
				for (int i = 3; i < count_RGB_channels; i++) {
					serializedBMP.push_back(pixel.at(i));
				}
			}
		}
	});
	return serializedBMP;
}

const image_size BMP::get_size() {
	return size;
}

const scheme BMP::get_color(){
	return color;
}

const int BMP::get_count_RGB_channels() {
	return count_RGB_channels;
}

vector<vector<vector<unsigned char>>>::iterator BMP::first_row() {
	return pixels.begin();
}

vector<vector<vector<unsigned char>>>::iterator BMP::last_row() {
	return pixels.end();
}

ostream& operator<<(ostream& out, BMP& bmp){
	vector<unsigned char> data = bmp.serialize(bmp.color);
	for (unsigned char value : data) {
		out << value;
	}
	return out;
}

void BMP::write(string filename, scheme color) {
	if (this->color != color) this->color = color;
	ofstream output(filename, ios::binary | ios::trunc | ios::out);
	if (!output) throw runtime_error("Writing to " + filename + " file is not possible!");
	output << *this;
	output.close();
}

void BMP::write(string filename) {
	write(filename, this->color);
}