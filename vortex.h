#include <Windows.h>
#include <string>
#include <functional>
#pragma once

using namespace std;

class vortex {
	function<double(double)> f_tp;
public:
	double pow_fi;
	double r_d;
	double r_hole;
	double fi;
	vortex(function<double(double)> f, double pow_fi, double r_d, double r_hole, double fi);
	vortex(double tp, double pow_fi, double r_d, double r_hole, double fi);
	double tp(double r);
};
