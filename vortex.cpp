#include "vortex.h"

vortex::vortex(function<double(double)> f, double pow_fi, double r_d, double r_hole, double fi) : f_tp(f), pow_fi(pow_fi), r_d(r_d), r_hole(r_hole), fi(fi) {}

vortex::vortex(double tp, double pow_fi, double r_d, double r_hole, double fi) : pow_fi(pow_fi), r_d(r_d), r_hole(r_hole), fi(fi) {
	f_tp = [=](double r) {return tp; };
}

double vortex::tp(double q) {
	return f_tp(q);
}

