#ifndef __BMP__
#define __BMP__
#include <vector>
#include <Windows.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include "scheme.h"
#include "size.h"

using namespace std;

class color_pixel;
class BMP {
	int count_RGB_channels = 4;
	scheme color = scheme::color;
	static int const BMPFILEHEADERsize = 14;
	static int const BMPINFOHEADERsize = 124;
	static int const COLORPROFILEsize = 12;
	image_size size;

private:
	vector<vector<vector<unsigned char>>> pixels;
	vector<vector<int>> bmpFileHeader;
	vector<vector<int>> bmpInfoHeader;
	vector<vector<int>> colorProfile;

	void initHeader();
	vector<unsigned char> toBinary(vector<int> number);
	static int toNumber(vector<unsigned char> binary) {
		int temp = 0;
		for (int i = 0; i < binary.size(); i++) {
			temp |= binary.at(i) << (8 * i);
		}
		return temp;
	};

public:
	BMP();
	BMP(vector<vector<vector<unsigned char>>>& picture);
	BMP(vector<vector<vector<unsigned char>>>& picture, scheme color);
	BMP(string filename);
	BMP(const BMP& obj);

	BMP& operator=(const BMP& obj);
	const unsigned char& operator()(int const row, int const column, scheme const color);
	friend ostream& operator<< (ostream& out, BMP& bmp);
	friend istream& operator>>(istream& in, BMP& bmp);

	vector<unsigned char> serialize(scheme color);
	const image_size get_size();
	const scheme get_color();
	const int get_count_RGB_channels();
	unsigned char apply_scheme(vector<unsigned char> pixel, scheme color);
	unsigned char apply_scheme(vector<unsigned char> pixel);
	vector<vector<vector<unsigned char>>>::iterator first_row();
	vector<vector<vector<unsigned char>>>::iterator last_row();
	void write(string filename, scheme color);
	void write(string filename);
};

#endif