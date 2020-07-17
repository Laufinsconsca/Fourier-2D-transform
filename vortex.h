/**********************************************************************************************//**
 * @file	mpTest\vortex.h.
 *
 * Declares the vortex class (this class contains a structure describing the vortex phase 
 * according to its definition e^(i*tp*(fi^n)), 
 * tp - topological charge,
 * n - power of fi)
 **************************************************************************************************/

#include <Windows.h>
#include <string>
#include <functional>
#pragma once

using namespace std;

/** A vortex. */
class vortex {
	/** The topological charge (is presented as functional object due to the functional dependence of the topological charge. */
	function<double(double)> f_tp;
public:
	/** The power of fi. */
	double pow_fi;
	/** The distance from the center of an vortex to the center of the hole. */
	double r_d;
	/** The hole radius. */
	double r_hole;
	/** The angle of the hole. */
	double fi;
	
	vortex(function<double(double)> f, double pow_fi, double r_d, double r_hole, double fi);
	vortex(double tp, double pow_fi, double r_d, double r_hole, double fi);
	double tp(double r);
};
