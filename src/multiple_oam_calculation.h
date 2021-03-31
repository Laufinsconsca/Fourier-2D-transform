#pragma once
#include <iostream>
#include <functional>
#include "bmp.h"
#include <cstdio>
#include "complex_amplitude.h"
#include "vortex.h";
#include "makedir.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

/** Multiple OAM calculation */
inline void multiple_oam_calculation() {
	image_size size(256, 256);
	string folder;
	char* temp = (char*)malloc(200);
	string name;
	string temp_name;
	string c_fi;

//#define WITH_HOLES

	// <constantDeclaration>
	//------------------------------------------------------------------------------
	// <colorScheme>
	//--------------------------------------------------------------------------
	scheme color_scheme = scheme::fire;
	//--------------------------------------------------------------------------
	// </colorScheme>
	//------------------------------------------------------------------------------
	// <inputDirectory>
	//--------------------------------------------------------------------------
	string inputDirectory = "L:\\backup\\input";
	//--------------------------------------------------------------------------
	// </inputDirectory>
	//--------------------------------------------------------------------------
	// <outputDirectory>
	//--------------------------------------------------------------------------
	string outputDirectory = "D:\\oam_exp_exact_calculated_512";
	//--------------------------------------------------------------------------
	// </outputDirectory>
	//--------------------------------------------------------------------------
	// <outputFilenames>
	//--------------------------------------------------------------------------
	string c_oam = "\\oam_tp=";
	//--------------------------------------------------------------------------
	// </outputFilenames>
	//--------------------------------------------------------------------------
	// <increase>
	//--------------------------------------------------------------------------
	const int increase = 4;
	//--------------------------------------------------------------------------
	// </increase>
	//--------------------------------------------------------------------------
	// <topological charges>
	//--------------------------------------------------------------------------
	const int number_of_tp = 1;
	//--------------------------------------------------------------------------
	// </topological charges>
	//--------------------------------------------------------------------------
	// <power of fi>
	//--------------------------------------------------------------------------
	const int number_of_pow_fi = 2;
	//--------------------------------------------------------------------------
	// </power of fi>
	//--------------------------------------------------------------------------
	// <dictances to the center>
	//--------------------------------------------------------------------------
	const int number_of_distances = 1;
	//--------------------------------------------------------------------------
	// </dictances to the center>
	//--------------------------------------------------------------------------
	// <the radius of holes>
	//--------------------------------------------------------------------------
	const int number_radius_of_holes = 2;
	//--------------------------------------------------------------------------
	// </the radius of holes>
	//--------------------------------------------------------------------------
	// <angle of hole>
	//--------------------------------------------------------------------------
	const int from = 0;
	const int to = 330;
	const int step = 30;
	//--------------------------------------------------------------------------
	// </angle of hole>
//------------------------------------------------------------------------------
// </constantDeclaration>
//------------------------------------------------------------------------------
// <initVariables>
//------------------------------------------------------------------------------
	// <topological charges>
	//--------------------------------------------------------------------------
	string c_tp_s[number_of_tp] = {
		{"1"},
	};
	//--------------------------------------------------------------------------
	// </topological charges>
	//--------------------------------------------------------------------------
	// <the distances from the center of distribution to the enter of the holes>
	//--------------------------------------------------------------------------
	string c_r_d_s[number_of_distances] = {
		{"3"}
	};
	//--------------------------------------------------------------------------
	// </the distances from the center of distribution to the enter of the holes>
	//--------------------------------------------------------------------------
	// <the radius of holes>
	//--------------------------------------------------------------------------
	string c_r_hole_s[number_radius_of_holes] = {
		{"10"},
		{"20"},
	};
	//--------------------------------------------------------------------------
	// </the radius of holes>
	//--------------------------------------------------------------------------
	// <the power of fi>
	//--------------------------------------------------------------------------
	string c_pow_fi_s[number_of_pow_fi] = {
		{"1.5"},
		{"2.5"}
	};
	//--------------------------------------------------------------------------
	// </the power of fi>
//------------------------------------------------------------------------------
// </initVariables>
//------------------------------------------------------------------------------
	for (int tp = 0; tp < number_of_tp; tp++) {
		string c_tp = c_tp_s[tp];
		for (int pow_fi = 0; pow_fi < number_of_pow_fi; pow_fi++) {
			string c_pow_fi = c_pow_fi_s[pow_fi];
			BMP amplitude_e(inputDirectory + "\\tp=" + c_tp + "\\fi=" + c_pow_fi + "\\REZ_A.bmp");
			BMP phase_e(inputDirectory + "\\tp=" + c_tp + "\\fi=" + c_pow_fi + "\\REZ_P.bmp");
			complex_amplitude IB_E(amplitude_e, phase_e);
			folder = outputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi;
			makedir(folder.c_str(), temp);
			temp_name = c_tp + ",pow_fi=" + c_pow_fi + ".bmp";

			//------------------------------------------------------------------------------
			//<record reference OAM>
			//------------------------------------------------------------------------------
			BMP oam(color_scheme);
			double* oam_with_min_max = IB_E.get_oam(oam);

			string oam_n = to_string(oam_with_min_max[0]);
			ofstream fout_oam(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ".oam", ios::trunc);
			fout_oam << oam_n;
			fout_oam.close();
			string min = to_string(oam_with_min_max[1]);
			ofstream fout_min(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ".min", ios::trunc);
			fout_min << min;
			fout_min.close();
			string max = to_string(oam_with_min_max[2]);
			ofstream fout_max(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ".max", ios::trunc);
			fout_max << max;
			fout_max.close();

			oam.write(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ".bmp");
			//------------------------------------------------------------------------------
			//</record reference OAM>
			//------------------------------------------------------------------------------

#ifdef WITH_HOLES
	//------------------------------------------------------------------------------
	//<modeling distorted distributions>
	//------------------------------------------------------------------------------
			for (int r_d = 0; r_d < number_of_distances; r_d++) {
				string c_r_d = c_r_d_s[r_d];
				for (int r_hole = 0; r_hole < number_radius_of_holes; r_hole++) {
					for (int fi = from; fi <= to; fi += step) {
						c_fi = to_string(fi);
						string c_r_hole = c_r_hole_s[r_hole];
						BMP amplitude(inputDirectory + "\\tp=" + c_tp + "\\fi=" + c_pow_fi + "\\r=" + c_r_hole + "\\" + c_fi + "\\REZ_A.bmp");
						BMP phase(inputDirectory + "\\tp=" + c_tp + "\\fi=" + c_pow_fi + "\\r=" + c_r_hole + "\\" + c_fi + "\\REZ_P.bmp");
						complex_amplitude IB(amplitude, phase);
						folder = outputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi + "\\r_hole=" + c_r_hole + "\\fi=" + c_fi;
						makedir(folder.c_str(), temp);
						temp_name = c_tp + ",pow_fi=" + c_pow_fi + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp";

						//------------------------------------------------------------------------------
						//<record distorted OAM>
						//------------------------------------------------------------------------------
						BMP oam(color_scheme);
						double* oam_with_min_max = IB.get_oam(oam);

						string oam_n = to_string(oam_with_min_max[0]);
						ofstream fout_oam(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".oam", ios::trunc);
						fout_oam << oam_n;
						fout_oam.close();
						string min = to_string(oam_with_min_max[1]);
						ofstream fout_min(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".min", ios::trunc);
						fout_min << min;
						fout_min.close();
						string max = to_string(oam_with_min_max[2]);
						ofstream fout_max(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".max", ios::trunc);
						fout_max << max;
						fout_max.close();

						oam.write(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp");
						//------------------------------------------------------------------------------
						//</record distorted OAM>
						//------------------------------------------------------------------------------
					}
				}
			}
			//------------------------------------------------------------------------------
			//</modeling distorted distributions>
			//------------------------------------------------------------------------------
			#endif
		}
	}
}
