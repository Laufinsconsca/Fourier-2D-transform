#ifndef __BMP__
#define __BMP__
#include <vector>
#include <Windows.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include "scheme.h"
#include "size.h"

using namespace std;

class color_pixel;
class BMP {
	int number_RGB_channels = 4;
	scheme color_scheme = scheme::color;
	static int const BMPFILEHEADERsize = 14;
	static int const BMPINFOHEADERsize = 124;
	static int const COLORPROFILEsize = 12;
	image_size size;
	map<unsigned char, vector<unsigned char>> cmap;

private:
	vector<vector<vector<unsigned char>>> pixels;
	vector<vector<int>> bmpFileHeader;
	vector<vector<int>> bmpInfoHeader;
	vector<vector<int>> colorProfile;

	inline void initHeader() {
		bmpFileHeader = { {0x4D42, 2}, {size.width * size.height * number_RGB_channels + BMPFILEHEADERsize + BMPINFOHEADERsize + COLORPROFILEsize, 4}, {0, 2}, {0, 2}, {BMPFILEHEADERsize + BMPINFOHEADERsize + COLORPROFILEsize, 4} };
		bmpInfoHeader = { {BMPINFOHEADERsize, 4}, {size.width, 4}, {size.height, 4}, {1, 2}, {number_RGB_channels * 8, 2}, {BI_BITFIELDS, 4}, {0, 4}, {0xEC4, 4}, {0xEC4, 4}, {0, 4}, {0, 4},
			{0x00FF0000, 4}, {0x0000FF00, 4}, {0x000000FF, 4}, {-16777216 /*0xFF000000*/, 4}, {LCS_WINDOWS_COLOR_SPACE, 4}, {0, 36}, {0, 4}, {0, 4}, {0, 4},
			{0, 4}, {0, 4}, {0, 4}, {0, 4} };
		colorProfile = { {0x000000FF, 4}, {0x0000FF00, 4}, {0x00FF0000, 4} };
	}
	vector<unsigned char> toBinary(vector<int> number);
	static int toNumber(vector<unsigned char> binary) {
		int temp = 0;
		for (int i = 0; i < binary.size(); i++) {
			temp |= binary.at(i) << (8 * i);
		}
		return temp;
	};
	vector<unsigned char> form_pixel(unsigned char value, scheme color);
	vector<unsigned char> form_pixel(unsigned char value);
	map<unsigned char, vector<unsigned char>>& get_cmap(scheme color_scheme, map<unsigned char, vector<unsigned char>>& cmap);

public:
	BMP();
	BMP(scheme color);
	BMP(vector<vector<vector<unsigned char>>>& picture);
	BMP(vector<vector<vector<unsigned char>>>& picture, scheme color);
	BMP(vector<vector<double>>& picture);
	BMP(vector<vector<double>>& picture, scheme color);
	BMP(string filename);
	BMP(const BMP& obj);

	BMP& operator=(const BMP& obj);
	const unsigned char& operator()(int const row, int const column, int channel);
	friend ostream& operator<< (ostream& out, BMP& bmp);
	friend istream& operator>>(istream& in, BMP& bmp);

	vector<vector<vector<unsigned char>>>::iterator first_row();
	vector<vector<vector<unsigned char>>>::iterator last_row();
	vector<unsigned char> serialize(scheme color);
	const image_size get_size();
	const scheme get_color();
	const int get_number_RGB_channels();
	unsigned char apply_scheme(vector<unsigned char> pixel);
	void write(string filename, scheme color);
	void write(string filename);
};

#endif