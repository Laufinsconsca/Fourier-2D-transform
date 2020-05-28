#include <Windows.h>
#include <string>
#pragma once

using namespace std;

class TP;
class vortex {
	TP* tp_s;
	double tp_n;
	double r_d;
	double r_hole;
	double fi;
public:
	bool tp_is_const;
	double pow_fi;
	vortex(string tp, double pow_fi, double r_d, double r_hole, double fi);
	vortex(double tp, double pow_fi, double r_d, double r_hole, double fi);
	double tp(double r);
};
