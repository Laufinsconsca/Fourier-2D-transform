#include "bmp.h"

string _to_string(scheme color);

BMP::BMP() : number_RGB_channels(4) {
	initHeader();
}

BMP::BMP(scheme color) : BMP() {
	this->color_scheme = color;
}

BMP::BMP(vector<vector<vector<unsigned char>>>& picture) : pixels(picture) {
	size.height = (int)picture.at(0).size();
	size.width = (int)picture.size();
	number_RGB_channels = picture.at(0).at(0).size();
	initHeader();
}

BMP::BMP(vector<vector<vector<unsigned char>>>& picture, scheme color) : BMP(picture) {
	this->color_scheme = color;
}

BMP::BMP(vector<vector<double>>& picture, scheme color_scheme) {
	number_RGB_channels = 4;
	this->color_scheme = color_scheme;
	size.height = (int)picture.at(0).size();
	size.width = (int)picture.size();
	initHeader();
	cmap = get_cmap(color_scheme, cmap);
	pixels.reserve(size.height*size.width* number_RGB_channels);
	vector<vector<unsigned char>> dest_row;
	for (vector<double> source_row : picture) {
		dest_row.reserve(size.width*number_RGB_channels);
		for (double pixel : source_row) {
			dest_row.push_back(form_pixel(round(pixel)));
		}
		pixels.push_back(dest_row);
		dest_row.clear();
	}
}

BMP::BMP(vector<vector<double>>& picture) : BMP(picture, color_scheme) {}

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
	number_RGB_channels = obj.number_RGB_channels;
	color_scheme = obj.color_scheme;
	size = obj.size;
	bmpFileHeader = obj.bmpFileHeader;
	bmpInfoHeader = obj.bmpInfoHeader;
	colorProfile = obj.colorProfile;
}

BMP& BMP::operator=(const BMP& obj) {
	if (this == &obj) return *this;
	pixels = obj.pixels;
	number_RGB_channels = obj.number_RGB_channels;
	color_scheme = obj.color_scheme;
	size = obj.size;
	bmpFileHeader = obj.bmpFileHeader;
	bmpInfoHeader = obj.bmpInfoHeader;
	colorProfile = obj.colorProfile;
	return *this;
}

const unsigned char& BMP::operator()(int const row, int const column, int channel){
	return this->pixels.at(row).at(column), channel;
}

unsigned char BMP::apply_scheme(vector<unsigned char> pixel) {
	return pixel.at(0);
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
	bmp.number_RGB_channels = 4;
	int source_number_RGB_channels = bmp.toNumber(buf_vector) / 8;
	buf_vector.clear();
	input.seekg(long long(offset) - 30, input.cur);
	/*</header reading>*/
	/*<pixels reading>*/
	bmp.pixels.reserve(bmp.size.height);
	for (int i = 0; i < bmp.size.height; i++) {
		bmp.pixels.push_back(vector<vector<unsigned char>>());
		bmp.pixels.at(i).reserve(bmp.size.width);
		for (int j = 0; j < bmp.size.width; j++) {
			vector<unsigned char> pixel;
			pixel.reserve(bmp.number_RGB_channels);
			if (source_number_RGB_channels == 1) {
				if (!(input >> buf)) return input;
				pixel.push_back(buf);
				pixel.push_back(buf);
				pixel.push_back(buf);
				pixel.push_back(255);
			}
			else {
				for (int k = 0; k < source_number_RGB_channels; k++) {
					if (!(input >> buf)) return input;
					pixel.push_back(buf);
				}
			}
			bmp.pixels.back().push_back(pixel);
		}
	}
	if (bmp.number_RGB_channels == 1) {
		bmp.color_scheme = scheme::gray;
	}
	reverse(bmp.pixels.begin(), bmp.pixels.end());
	bmp.initHeader();
	return input;
}

vector<unsigned char> BMP::serialize(scheme color_scheme)
{
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
			for (int i = 0; i < number_RGB_channels; i++) {
				serializedBMP.push_back(pixel.at(i));
			}
		}
	});
	return serializedBMP;
}

const image_size BMP::get_size() {
	return size;
}

const scheme BMP::get_color(){
	return color_scheme;
}

const int BMP::get_number_RGB_channels() {
	return number_RGB_channels;
}

vector<vector<vector<unsigned char>>>::iterator BMP::first_row() {
	return pixels.begin();
}

vector<vector<vector<unsigned char>>>::iterator BMP::last_row() {
	return pixels.end();
}

ostream& operator<<(ostream& out, BMP& bmp){
	vector<unsigned char> data = bmp.serialize(bmp.color_scheme);
	for (unsigned char value : data) {
		out << value;
	}
	return out;
}

void BMP::write(string filename, scheme color) {
	if (this->color_scheme != color) this->color_scheme = color;
	ofstream output(filename, ios::binary | ios::trunc | ios::out);
	if (!output) throw runtime_error("Writing to " + filename + " file is not possible!");
	output << *this;
	output.close();
}

void BMP::write(string filename) {
	write(filename, this->color_scheme);
}

vector<unsigned char> BMP::form_pixel(unsigned char value) {
	return form_pixel(value, color_scheme);
}

vector<unsigned char> BMP::form_pixel(unsigned char value, scheme color) {
	vector<unsigned char> pixel;
	unsigned char char_value = round(255 - value);
	pixel.push_back(cmap.at(char_value).at(0));
	pixel.push_back(cmap.at(char_value).at(1));
	pixel.push_back(cmap.at(char_value).at(2));
	pixel.push_back(255); //for alpha channel;
	return pixel;
}


map<unsigned char, vector<unsigned char>>& BMP::get_cmap(scheme color_scheme, map<unsigned char, vector<unsigned char>>& cmap) {
	string buf;
	ifstream in("src\\cmap\\" + _to_string(color_scheme) + ".cmap", ios::in);
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