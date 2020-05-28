#include "vortex.h"
#include "tp.h"

vortex::vortex(string tp, double pow_fi, double r_d, double r_hole, double fi) : pow_fi(pow_fi), r_d(r_d), r_hole(r_hole), fi(fi), tp_is_const(false) {
	tp_s = new TP(tp);
}
vortex::vortex(double tp, double pow_fi, double r_d, double r_hole, double fi) : tp_n(tp), pow_fi(pow_fi), r_d(r_d), r_hole(r_hole), fi(fi), tp_is_const(true) {}

double vortex::tp(double q) {
	if (tp_is_const) {
		return tp_n;
	}
	else {
		return (*tp_s)(q);
	}
}

