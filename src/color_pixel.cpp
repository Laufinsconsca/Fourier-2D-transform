#include "color_pixel.h"

color_pixel::color_pixel() {
	vector<unsigned char> pixel();
	color = scheme::color;
}
color_pixel::color_pixel(vector<unsigned char>& const obj) {
	pixel = obj;
	color = scheme_determinate(obj);
}
color_pixel::color_pixel(color_pixel& const obj) {
	vector<unsigned char> pixel(obj.pixel);
	color = obj.color;
}
color_pixel& color_pixel::operator=(const color_pixel& obj) {
	if (this == &obj) return *this;
	pixel = obj.pixel;
	color = obj.color;
	return *this;
}
boolean color_pixel::operator==(const color_pixel& obj) {
	return color == obj.color;
}

scheme color_pixel::get_color() {
	return color;
}
