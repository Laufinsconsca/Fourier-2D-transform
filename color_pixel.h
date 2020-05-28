#pragma once
#include "bmp.h"

class color_pixel {
scheme color;
vector<unsigned char> pixel;
private:
	static scheme scheme_determinate(vector<unsigned char> pixel) {
		int n = 5;
		if (pixel.at(0) == 0 && pixel.at(1) == 0 && pixel.at(2) == 0) {
			return scheme::zero;
		} else if (pixel.at(0) == pixel.at(1) && pixel.at(1) == pixel.at(2)) {
			return scheme::gray;
		} else if (pixel.at(1) == 0 && pixel.at(2) == 0) {
			return scheme::only_blue_channel;
		} else if (pixel.at(0) == 0 && pixel.at(2) == 0) {
			return scheme::only_green_channel;
		} else if (pixel.at(0) == 0 && pixel.at(1) == 0) {
			return scheme::only_red_channel;
		} else {
			return scheme::color;
		}
	};
public:
	color_pixel();
	scheme get_color();
	color_pixel(vector<unsigned char>& const obj);
	color_pixel(color_pixel& const obj);
	color_pixel& operator=(const color_pixel& obj);
	boolean operator==(const color_pixel& obj);
};

