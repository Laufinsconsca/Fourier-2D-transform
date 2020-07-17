#include <iostream>
#include <functional>
#include "bmp.h"
#include <cstdio>
#include "complex_amplitude.h"
#include "vortex.h";
#include "makedir.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

/** Multiple processing (for multiple calculation of the Fourier transform from different distributions) */
inline void multiple_processing() {
image_size size(256, 256);
string folder;
char* temp = (char*)malloc(200);
string name;
string temp_name;
string c_fi;

//#define INTENSITY
//#define AMPLITUDE
//#define PHASE
//#define SIMILARITY
#define OAM
#define WITH_HOLES

	// <constantDeclaration>
	//------------------------------------------------------------------------------
	// <colorScheme>
	//--------------------------------------------------------------------------
	scheme color_scheme = scheme::gray;
	//--------------------------------------------------------------------------
	// </colorScheme>
	//------------------------------------------------------------------------------
	// <inputDirectory>
	//--------------------------------------------------------------------------
	string inputDirectory = "D:\\input";
	//--------------------------------------------------------------------------
	// </inputDirectory>
	//--------------------------------------------------------------------------
	// <outputDirectory>
	//--------------------------------------------------------------------------
	string outputDirectory = "D:\\oam_dist";
	//--------------------------------------------------------------------------
	// </outputDirectory>
	//--------------------------------------------------------------------------
	// <outputFilenames>
	//--------------------------------------------------------------------------
	string c_intensity = "\\intensity_tp=";
	string c_amplitude = "\\amplitude_tp=";
	string c_phase = "\\phase_tp=";
	string c_similarity_intensity = "\\similarity_intensity_tp=";
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
	const int number_of_tp = 2;
	//--------------------------------------------------------------------------
	// </topological charges>
	//--------------------------------------------------------------------------
	// <power of fi>
	//--------------------------------------------------------------------------
	const int number_of_pow_fi = 1;
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
	const int number_radius_of_holes = 4;
	//--------------------------------------------------------------------------
	// </the radius of holes>
	//--------------------------------------------------------------------------
	// <angle of hole>
	//--------------------------------------------------------------------------
	const int from = 15;
	const int to = 360;
	const int step = 15;
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
		//{"1"},
		//{"3"},
		{"5"},
		{"10"},
		//{"r^0.5"},
		//{"r"},
		//{"r^1.5"},
		//{"r^2"},
		//{"1.5^r"},
		//{"erf(r)"},
	};
	//--------------------------------------------------------------------------
	// </topological charges>
	//--------------------------------------------------------------------------
	// <the distances from the center of distribution to the enter of the holes>
	//--------------------------------------------------------------------------
	string c_r_d_s[number_of_distances] = {
		//{"1"},
		//{"2"},
		{"3"}
	};
	//--------------------------------------------------------------------------
	// </the distances from the center of distribution to the enter of the holes>
	//--------------------------------------------------------------------------
	// <the radius of holes>
	//--------------------------------------------------------------------------
	string c_r_hole_s[number_radius_of_holes] = {
		{"0.15"},
		{"0.2"},
		{"0.25"},
		{"0.75"}
	};
	//--------------------------------------------------------------------------
	// </the radius of holes>
	//--------------------------------------------------------------------------
	// <the power of fi>
	//--------------------------------------------------------------------------
	string c_pow_fi_s[number_of_pow_fi] = {
		//{"1"},
		//{"1.5"},
		//{"2"},
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
		//------------------------------------------------------------------------------
		// <zoom rule>
		//------------------------------------------------------------------------------
		int extension = 2;// pow_fi == 2 || pow_fi == 3 ? 4 : pow_fi == 1 ? 8 : 16;
		//------------------------------------------------------------------------------
		// </zoom rule>
		//------------------------------------------------------------------------------
		BMP vortex_e(inputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi + "\\tp=" + c_tp + ",pow_fi=" + c_pow_fi + ".bmp");
		BMP gauss_e(inputDirectory + "\\gauss\\gauss.bmp");
		complex_amplitude IB_E(gauss_e, vortex_e);
		IB_E.FFT2D(extension);
		folder = outputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi;
		makedir(folder.c_str(), temp);
		temp_name = c_tp + ",pow_fi=" + c_pow_fi + ".bmp";
		//------------------------------------------------------------------------------
		//<record reference intensity>
		//------------------------------------------------------------------------------
		#ifdef INTENSITY
			IB_E.write(folder + c_intensity + temp_name, out_field_type::intensity, color_scheme);
		#endif
		//------------------------------------------------------------------------------
		//</record reference intensity>
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		//<record reference amplitude>
		//------------------------------------------------------------------------------
		#ifdef AMPLITUDE
			IB_E.write(folder + c_amplitude + temp_name, out_field_type::amplitude, color_scheme);
		#endif
		//------------------------------------------------------------------------------
		//</record reference amplitude>
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		//<record reference phase>
		//------------------------------------------------------------------------------
		#ifdef PHASE
			IB_E.write(folder + c_phase + temp_name, out_field_type::argument, color_scheme);
		#endif
		//------------------------------------------------------------------------------
		//</record reference phase>
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		//<record reference OAM>
		//------------------------------------------------------------------------------
		#ifdef OAM
			BMP oam(color_scheme);
			string oam_n = to_string(IB_E.get_oam(oam)[0]);
			ofstream fout(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ".oam", ios::trunc);
			fout << oam_n;
			fout.close();
			oam.write(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",OAM=" + oam_n + ".bmp");
		#endif
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
						BMP vortex(inputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi + "\\r_d=" + c_r_d + "\\r_hole=" + c_r_hole + "\\fi=" + c_fi + "\\tp=" + c_tp + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp");
						BMP gauss(inputDirectory + "\\gauss\\r_d=" + c_r_d + "\\r_hole=" + c_r_hole + "\\fi=" + c_fi + "\\r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp");
						complex_amplitude IB(gauss, vortex);
						IB.FFT2D(extension);
						folder = outputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi + "\\r_d=" + c_r_d + "\\r_hole=" + c_r_hole + "\\fi=" + c_fi;
						makedir(folder.c_str(), temp);
						temp_name = c_tp + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp";
						//IB.Conj();
						//IB = IB*IB_E;
						//IB.WriteFile(name, IB_E); // записываем коррел€ционную интенсивность

						//------------------------------------------------------------------------------
						//<record distorted intensity>
						//------------------------------------------------------------------------------
						#ifdef INTENSITY
							name = folder + c_intensity + temp_name;
							IB.write(name, out_field_type::intensity, color_scheme);
						#endif
						//------------------------------------------------------------------------------
						//</record distorted intensity>
						//------------------------------------------------------------------------------
						//sprintf_s(name, "%s%s%s", folder, c_amplitude, temp);
						//------------------------------------------------------------------------------
						//<record distorted amplitude>
						//------------------------------------------------------------------------------
						#ifdef AMPLITUDE
							name = folder + c_amplitude + temp_name;
							IB.write(name, out_field_type::amplitude, color_scheme);
						#endif
						//------------------------------------------------------------------------------
						//</record distorted amplitude>
						//------------------------------------------------------------------------------
						//sprintf_s(name, "%s%s%s", folder, c_phase, temp);
						//------------------------------------------------------------------------------
						//<record distorted phase>
						//------------------------------------------------------------------------------
						#ifdef PHASE
							name = folder + c_phase + temp_name;
							IB.write(name, out_field_type::argument, color_scheme);
						#endif
						//------------------------------------------------------------------------------
						//</record distorted phase>
						//------------------------------------------------------------------------------
						//sprintf_s(name, "%s%s%s", folder, c_similarity_intensity, temp);
						//------------------------------------------------------------------------------
						//<record distorted similarity>
						//------------------------------------------------------------------------------
						#ifdef SIMILARITY
							//IB.Conj();
							IB = IB + IB_E;
							IB.WriteFile(name, 0);
							//IB_E.WriteFile(name, 2);
						#endif
						//------------------------------------------------------------------------------
						//</record distorted similarity>
						//------------------------------------------------------------------------------
						//------------------------------------------------------------------------------
						//<record distorted OAM>
						//------------------------------------------------------------------------------
						#ifdef OAM
							BMP oam(color_scheme);
							string oam_n = to_string(IB.get_oam(oam)[0]);
							ofstream fout(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".oam", ios::trunc);
							fout << oam_n;
							fout.close();
							oam.write(folder + c_oam + c_tp + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ",OAM=" + oam_n + ".bmp");
						#endif
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
#undef INTENSITY
#undef PHASE
}