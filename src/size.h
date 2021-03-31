#pragma once

typedef struct struct_size {
	int height;
	int width;
	struct_size() : height(0), width(0) {}
	struct_size(int width, int height) : width(width), height(height) {}
	struct_size(const struct_size& obj) : width(obj.width), height(obj.height) {}
	boolean operator ==(const struct_size& obj) {
		return height == obj.height && width == obj.width;
	}
	boolean operator !=(const struct_size& obj) {
		return !(*this == obj);
	}
} image_size;
