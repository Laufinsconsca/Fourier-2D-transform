#include "tp.h"
#include <stdexcept>

// c*r^n - general view
TP::TP(string tp) {
	if (tp[0] == 'r') { 
		if (tp.size() == 1) {
			c = 1;
			p = 1;
		} else if (tp[1] == '^') {
			try {
				c = 1;
				p = stod(tp.substr(2, tp.size() - 2));
			}
			catch (...) {
				throw runtime_error("Invalid topological charge function");
			}
		}
		else { 
			throw runtime_error("Invalid topological charge function"); 
		}
	} else 
	if (tp[tp.size() - 1] == 'r') {
		if (tp[tp.size() - 2] == '^') {
			try {
				c = 1;
				p = stod(tp.substr(0, tp.size() - 2));
			}
			catch (...) {
				throw runtime_error("Invalid topological charge function");
			}
		}
		else {
			try {
				c = stod(tp.substr(0, tp.size() - 1));
				p = 1;
			}
			catch (...) {
				throw runtime_error("Invalid topological charge function");
			}
		}
	} else {
		try {
			c = stod(tp);
			p = 0;
		}
		catch (...) {
			throw runtime_error("Invalid topological charge function");
		}
	}
}

double TP::operator()(double q){
	return c * pow(q, p);
}
