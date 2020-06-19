#include <direct.h>

//------------------------------------------------------------------------------
//<creating a folder structure>
//------------------------------------------------------------------------------
//<args>
//		filename -> full path of expected directory
//		temp 	 -> temp char array (the length is determined based on the length
//            		of the full path to the expected directory
//</args>
//<example>
//		filename = "C:\\outer\\inner\\nestedDirectory"
//</example>
//------------------------------------------------------------------------------
inline void makedir(const char* filename, char* temp) {
	int k = 0;
	int count = 0;
	for (int j = 2;; j++) {
		for (int i = 0; !(filename[i-1] == '\\' && count == j); i++) {
			if (filename[i] == '\0') {
				temp[i] = '\0';
				_mkdir(temp);
				return;
			}
			temp[i] = filename[i];
			k++;
			if (filename[i] == '\\') {
				count++;
			}
		}
		temp[k] = '\0';
		_mkdir(temp);
		count = 0;
		k = 0;
	}
}
//------------------------------------------------------------------------------
//</creating a folder structure>
//------------------------------------------------------------------------------