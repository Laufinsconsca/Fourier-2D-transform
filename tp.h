#pragma once
#include "vortex.h"

class TP {
	double p;
	double c;
public:
	TP() : p(0.), c(0.) {};
	TP(string tp);
	double operator()(double q);
};

