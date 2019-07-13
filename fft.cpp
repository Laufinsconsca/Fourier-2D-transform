#include "stdafx.h"
#include <vector>
#include <complex>
#include <iostream>
#include <fstream>
#include <iomanip>

#define PI 3.1415926535
#define fftshift(in, x, y) circshift(in, x, y, (x/2), (y/2))
#define ifftshift(in, x, y) circshift(in, x, y, ((x+1)/2), ((y+1)/2))

using namespace std;

complex<double>** circshift(complex<double> **in, int xdim, int ydim, int xshift, int yshift);

unsigned char *readBMP(const char *fname, int &mx, int &my);
int power_of_2(int n, int *m, int *twopm);
int FFT(int dir, int m, double *x, double *y);
int FFT2D(complex<double> **c, int nx, int ny, int dir);

int main()
{
	int height, width, direct;

	//the height and width of the intensity and phase must be the same

	char * filename_intensity, * filename_phase;
	filename_intensity = (char*)malloc(50); // 50 is the size limit of the file name length
	filename_phase = (char*)malloc(50);
	cin >> direct >> filename_intensity >> filename_phase;

	unsigned char* intensity = readBMP(filename_intensity, width, height);
	free(filename_intensity);

	unsigned char* phase = readBMP(filename_phase, width, height);
	free(filename_phase);

	ofstream f;
	f.open("output.txt", ios::out);

	complex<double> **incident_beam = (complex<double>**)malloc(height * sizeof(complex<double>));

	for (int i = 0; i < width; i++) {          
		*(incident_beam + i) = (complex<double>*)malloc(width * sizeof(complex<double>));
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			double tmp1 = (int)(*(intensity + i * width + j));
			double tmp2 = 2 * PI*(int)(*(phase + i * width + j)) / 255;
			*(*(incident_beam + i) + j) = complex<double>(tmp1*cos(tmp2), tmp1*sin(tmp2));
		}
	}
	FFT2D(incident_beam, width, height, direct);

	complex<double> **output_beam;
	if (direct == 1) {
		output_beam = fftshift(incident_beam, width, height);
	}
	else if (direct == -1) {
		output_beam = ifftshift(incident_beam, width, height);
	}
	else {
		cout << "Input correct value";
		system("pause>nul");
		return 0;
	}
	free(incident_beam);

	// finding the maximum for normalization
	int max = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			double abs = sqrt((*(*(output_beam + i) + j)).real()*(*(*(output_beam + i) + j)).real() + (*(*(output_beam + i) + j)).imag()*(*(*(output_beam + i) + j)).imag());
			if (abs > max) {
				max = abs;
			}
		}
	}
	//--------------------------------------------
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			f << 255*sqrt((*(*(output_beam + i) + j)).real()*(*(*(output_beam + i) + j)).real() + (*(*(output_beam + i) + j)).imag()*(*(*(output_beam + i) + j)).imag())/max << (char)9;
		}
		f << endl;
	}

	f.close();
	cout << "Done" << endl;
	system("pause>nul");
	return 0;
}

unsigned char *readBMP(const char *fname, int &mx, int &my)
{
	FILE *f = fopen(fname, "rb");
	if (!f) return NULL;

	unsigned char *info = (unsigned char*)malloc(54);
	fread(info, 1, 54, f); // read the 54-byte header
	mx = *(int*)(info + 18); // extract image height and width from header
	my = *(int*)(info + 22);
	int mx3 = (3 * mx + 3) & (-4);    // Compute row width in file, including padding to 4-byte boundary
	unsigned char *tmp_buf = new unsigned  char[mx3*my];    // читаем данные
	fread(tmp_buf, 1, mx3*my, f);
	// данные прочитаны
	fclose(f);

	// выделим память для результата
	int *v = new int[mx*my];

	// перенос данных 
	unsigned char *ptr = (unsigned char *)v;
	for (int y = my - 1; y >= 0; y--) {
		unsigned char *pRow = tmp_buf + mx3 * y;
		for (int x = 0; x < mx; x++) {
			*ptr++ = *(pRow + 2);
			*ptr++ = *(pRow + 1);
			*ptr++ = *pRow;
			pRow += 3;
			ptr++;
		}
	}

	unsigned char *ans = new unsigned char[mx*my];
	
	for (int i = 0; i < my; i++) {
		for (int j = 0; j < mx; j++) {
			*(ans + i*mx + j) = (*(v + i * mx + j) & 255);
		}
	}
	delete[]tmp_buf;
	delete[]v;
	return ans;    // OK
}

/*-------------------------------------------------------------------------
Perform a 2D FFT inplace given a complex 2D array
The direction dir, 1 for forward, -1 for reverse
The size of the array (nx,ny)
Return false if there are memory problems or
the dimensions are not powers of 2
*/
int FFT2D(complex<double> **c, int nx, int ny, int dir)
{
	int i, j;
	int m, twopm;
	double *real, *imag;

	/* Transform the rows */
	real = (double *)malloc(nx * sizeof(double));
	imag = (double *)malloc(nx * sizeof(double));
	if (real == NULL || imag == NULL)
		return(false);
	if (!power_of_2(nx, &m, &twopm) || twopm != nx)
		return(false);
	for (j = 0; j < ny; j++) {
		for (i = 0; i < nx; i++) {
			*(real + i) = (*((*(c + i)) + j)).real();
			*(imag + i) = (*((*(c + i)) + j)).imag();
		}
		FFT(dir, m, real, imag);
		for (i = 0; i < nx; i++) {
			(*((*(c + i)) + j)) = complex<double>(*(real + i), *(imag + i));
		}
	}
	free(real);
	free(imag);

	/* Transform the columns */
	real = (double *)malloc(ny * sizeof(double));
	imag = (double *)malloc(ny * sizeof(double));
	if (real == NULL || imag == NULL)
		return(false);
	if (!power_of_2(ny, &m, &twopm) || twopm != ny)
		return(false);
	for (i = 0; i < nx; i++) {
		for (j = 0; j < ny; j++) {
			*(real + j) = (*((*(c + i)) + j)).real();
			*(imag + j) = (*((*(c + i)) + j)).imag();
		}
		FFT(dir, m, real, imag);
		for (j = 0; j < ny; j++) {
			(*((*(c + i)) + j)) = complex<double>(*(real + j), *(imag + j));
		}
	}
	free(real);
	free(imag);

	return(true);
}

/*-------------------------------------------------------------------------
This computes an in-place complex-to-complex FFT
x and y are the real and imaginary arrays of 2^m points.
dir =  1 gives forward transform
dir = -1 gives reverse transform
*/
int FFT(int dir, int m, double *x, double *y)
{
	long nn, i, i1, j, k, i2, l, l1, l2;
	double c1, c2, tx, ty, t1, t2, u1, u2, z;

	/* Calculate the number of points */
	nn = 1;
	for (i = 0; i < m; i++) {
		nn *= 2;
	}

	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for (i = 0; i < nn - 1; i++) {
		if (i < j) {
			tx = *(x + i);
			ty = *(y + i);
			*(x + i) = *(x + j);
			*(y + i) = *(y + j);
			*(x + j) = tx;
			*(y + j) = ty;
		}
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0;
	l2 = 1.0;
	for (l = 0; l < m; l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0;
		for (j = 0; j < l1; j++) {
			for (i = j; i < nn; i += l2) {
				i1 = i + l1;
				t1 = u1 * *(x + i1) - u2 * *(y + i1);
				t2 = u1 * *(y + i1) + u2 * *(x + i1);
				*(x + i1) = *(x + i) - t1;
				*(y + i1) = *(y + i) - t2;
				*(x + i) += t1;
				*(y + i) += t2;
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
	if (dir == 1) {
		for (i = 0; i < nn; i++) {
			*(x + i) /= (double)nn;
			*(y + i) /= (double)nn;
		}
	}

	return(true);
}

/*-------------------------------------------------------------------------
Calculate the closest but lower power of two of a number
twopm = 2**m <= n
Return true if 2**m == n
*/
int power_of_2(int n, int *m, int *twopm)
{
	if (n <= 1) {
		*m = 0;
		*twopm = 1;
		return(false);
	}

	*m = 1;
	*twopm = 2;
	do {
		(*m)++;
		(*twopm) *= 2;
	} while (2 * (*twopm) <= n);

	if (*twopm != n)
		return(false);
	else
		return(true);
}

complex<double>** circshift(complex<double> **in, int xdim, int ydim, int xshift, int yshift)
{
	complex<double> **out = (complex<double>**)malloc(ydim * sizeof(complex<double>));
	for (int i = 0; i < xdim; i++) {
		*(out + i) = (complex<double>*)malloc(xdim * sizeof(complex<double>));
	}
	for (int i = 0; i < xdim; i++) {
		int ii = (i + xshift) % xdim;
		for (int j = 0; j < ydim; j++) {
			int jj = (j + yshift) % ydim;
			*(*(out + ii) + jj) = *(*(in + i) + j);
		}
	}
	return out;
}