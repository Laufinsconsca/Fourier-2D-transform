#include <iostream>
#include <functional>
#include "bmp.h"
#include <cstdio>
#include "complex_amplitude.h"
#include "vortex.h";
#include "makedir.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

/** Multiple processing with gauss shifts (for multiple calculation of the Fourier transform from different distributions) */
inline void multiple_processing_gauss_shift() {
	image_size size(256, 256);
	string folder;
	char* temp = (char*)malloc(200);
	string name;
	string temp_name;
	string c_fi;

#define INTENSITY
#define AMPLITUDE
#define PHASE
//#define SIMILARITY
#define OAM
#undef WITH_HOLES

	// <constantDeclaration>
	//------------------------------------------------------------------------------
	// <colorScheme>
	//--------------------------------------------------------------------------
	scheme amplitude_color_scheme = scheme::gray;
	scheme phase_color_scheme = scheme::gray;
	scheme intensity_color_scheme = scheme::gray;
	scheme oam_color_scheme = scheme::fire;
	//--------------------------------------------------------------------------
	// </colorScheme>
	//------------------------------------------------------------------------------
	// <inputDirectory>
	//--------------------------------------------------------------------------
	string inputDirectory = "L:\\distributions\\input_512";
	//--------------------------------------------------------------------------
	// </inputDirectory>
	//--------------------------------------------------------------------------
	// <outputDirectory>
	//--------------------------------------------------------------------------
	string outputDirectory = "L:\\distributions\\dist_for_energy_flow1";
	//--------------------------------------------------------------------------
	// </outputDirectory>
	//--------------------------------------------------------------------------
	// <outputFilenames>
	//--------------------------------------------------------------------------
	string c_intensity = "\\intensity";
	string c_amplitude = "\\amplitude";
	string c_phase = "\\phase";
	string c_similarity_intensity = "\\similarity_intensity";
	string c_oam = "\\oam";
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
	// <vortex relative to base beam shift>
	//--------------------------------------------------------------------------
	const int number_of_ordering_beams = 1;
	//--------------------------------------------------------------------------
	// </vortex relative to base beam shift>
	//--------------------------------------------------------------------------
	// <power of fi>
	//--------------------------------------------------------------------------
	const int number_of_pow_fi = 4;
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
		{"1"},
		{"3"},
		//{"5"},
		//{"10"},
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
	// <vortex relative to base beam shift>
	//--------------------------------------------------------------------------
	string c_g_sh_s[number_of_ordering_beams] = {
		{"0"},
		//{"0.25"},
		//{"0.5"},
		//{"0.75"},
		//{"-0.25"},
		//{"-0.5"},
		//{"-0.75"},
	};
	//--------------------------------------------------------------------------
	// </vortex relative to base beam shift>
	//--------------------------------------------------------------------------
	// <the distances from the center of distribution to the enter of the holes>
	//--------------------------------------------------------------------------
	string c_r_d_s[number_of_distances] = {
		{"1"},
		//{"2"},
		//{"3"}
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
		{"1"},
		{"1.5"},
		{"2"},
		{"2.5"}
	};
	//--------------------------------------------------------------------------
	// </the power of fi>
//------------------------------------------------------------------------------
// </initVariables>
//------------------------------------------------------------------------------
	for (int tp = 0; tp < number_of_tp; tp++) {
		string c_tp = c_tp_s[tp];
		for (int sh = 0; sh < number_of_ordering_beams; sh++) {
			string c_g_sh = c_g_sh_s[sh];
			for (int pow_fi = 0; pow_fi < number_of_pow_fi; pow_fi++) {
				string c_pow_fi = c_pow_fi_s[pow_fi];
				//------------------------------------------------------------------------------
				// <zoom rule>
				//------------------------------------------------------------------------------
				int extension = pow_fi == 2 || pow_fi == 3 ? 8 : pow_fi == 1 ? 8 : 16;
				//------------------------------------------------------------------------------
				// </zoom rule>
				//------------------------------------------------------------------------------
				BMP vortex_e(inputDirectory + "\\tp=" + c_tp + "\\pow_fi=" + c_pow_fi + "\\tp=" + c_tp + ",pow_fi=" + c_pow_fi + ".bmp");
				BMP gauss_e(inputDirectory + "\\gauss_with_shift\\shift=" + c_g_sh + "\\gauss,shift=" + c_g_sh + ".bmp");
				complex_amplitude IB_E(gauss_e, vortex_e);
				IB_E.FFT2D(extension);
				folder = outputDirectory + "\\tp=" + c_tp + "\\gauss_shift=" + c_g_sh + "\\pow_fi=" + c_pow_fi;
				makedir(folder.c_str(), temp);
				temp_name = "_tp=" + c_tp + ",gauss_shift=" + c_g_sh + ",pow_fi=" + c_pow_fi + ".bmp";
				//------------------------------------------------------------------------------
				//<record reference intensity>
				//------------------------------------------------------------------------------
#ifdef INTENSITY
				IB_E.write(folder + c_intensity + temp_name, out_field_type::intensity, intensity_color_scheme);
#endif
				//------------------------------------------------------------------------------
				//</record reference intensity>
				//------------------------------------------------------------------------------

				//------------------------------------------------------------------------------
				//<record reference amplitude>
				//------------------------------------------------------------------------------
#ifdef AMPLITUDE
				IB_E.write(folder + c_amplitude + temp_name, out_field_type::amplitude, amplitude_color_scheme);
#endif
				//------------------------------------------------------------------------------
				//</record reference amplitude>
				//------------------------------------------------------------------------------

				//------------------------------------------------------------------------------
				//<record reference phase>
				//------------------------------------------------------------------------------
#ifdef PHASE
				IB_E.write(folder + c_phase + temp_name, out_field_type::phase, phase_color_scheme);
#endif
				//------------------------------------------------------------------------------
				//</record reference phase>
				//------------------------------------------------------------------------------

				//------------------------------------------------------------------------------
				//<record reference OAM>
				//------------------------------------------------------------------------------
#ifdef OAM
				BMP oam(oam_color_scheme);
				string oam_n = to_string(IB_E.get_oam(oam)[0]);
				ofstream fout(folder + c_oam + "_tp=" + c_tp + ",pow_fi=" + c_pow_fi + ".oam", ios::trunc);
				fout << oam_n;
				fout.close();
				oam.write(folder + c_oam + "_tp=" + c_tp + ",gauss_shift=" + c_g_sh + ",pow_fi=" + c_pow_fi + ".bmp");
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
							BMP gauss(inputDirectory + "\\gauss_with_shift\\shift=" + c_g_sh + "\\r_d=" + c_r_d + "\\r_hole=" + c_r_hole + "\\fi=" + c_fi + "\\shift=" + c_g_sh + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp");
							complex_amplitude IB(gauss, vortex);
							IB.FFT2D(extension);
							folder = outputDirectory + "\\tp=" + c_tp + "\\gauss_shift=" + c_g_sh + "\\pow_fi=" + c_pow_fi + "\\r_d=" + c_r_d + "\\r_hole=" + c_r_hole + "\\fi=" + c_fi;
							makedir(folder.c_str(), temp);
							temp_name = "_tp=" + c_tp + ",gauss_shift=" + c_g_sh + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp";
							//IB.Conj();
							//IB = IB*IB_E;
							//IB.WriteFile(name, IB_E); // записываем коррел€ционную интенсивность

							//------------------------------------------------------------------------------
							//<record distorted intensity>
							//------------------------------------------------------------------------------
#ifdef INTENSITY
							name = folder + c_intensity + temp_name;
							IB.write(name, out_field_type::intensity, intensity_color_scheme);
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
							IB.write(name, out_field_type::amplitude, amplitude_color_scheme);
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
							IB.write(name, out_field_type::argument, phase_color_scheme);
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
							BMP oam(oam_color_scheme);
							string oam_n = to_string(IB.get_oam(oam)[0]);
							ofstream fout(folder + c_oam + "_tp=" + c_tp + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".oam", ios::trunc);
							fout << oam_n;
							fout.close();
							oam.write(folder + c_oam + "_tp=" + c_tp + ",gauss_shift=" + c_g_sh + ",pow_fi=" + c_pow_fi + ",r_d=" + c_r_d + ",r_hole=" + c_r_hole + ",fi=" + c_fi + ".bmp");
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
}