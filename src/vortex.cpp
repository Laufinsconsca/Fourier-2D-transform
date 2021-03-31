#include "vortex.h"

/**********************************************************************************************//**
 * Constructor.
 *
 * @parameters	f	  	The dependency topological charge from distance from the center of an vortex
 * @parameters	pow_fi	The power of fi.
 * @parameters	r_d   	The distance from the center of an vortex to the center of the hole.
 * @parameters	r_hole	The hole radius.
 * @parameters	fi	  	The angle of the hole.
 **************************************************************************************************/

vortex::vortex(function<double(double)> f, double pow_fi, double r_d, double r_hole, double fi) : f_tp(f), pow_fi(pow_fi), r_d(r_d), r_hole(r_hole), fi(fi) {}

/**********************************************************************************************//**
 * Constructor.
 *
 * @parameters	tp	  	The constant topological charge value (if there is no dependence on the distance).
 * @parameters	pow_fi	The power of fi.
 * @parameters	r_d   	The distance from the center of an vortex to the center of the hole.
 * @parameters	r_hole	The hole radius.
 * @parameters	fi	  	The angle of the hole.
 **************************************************************************************************/

vortex::vortex(double tp, double pow_fi, double r_d, double r_hole, double fi) : pow_fi(pow_fi), r_d(r_d), r_hole(r_hole), fi(fi) {
	f_tp = [=](double r) {return tp; };
}

/**********************************************************************************************//**
 * Get value of the topological charge for given distance.
 *
 * @parameters	distance Distance from the center of the image.
 *
 * @returns	The topological charge at a given distance from the center.
 **************************************************************************************************/

double vortex::tp(double distance) {
	return f_tp(distance);
}

