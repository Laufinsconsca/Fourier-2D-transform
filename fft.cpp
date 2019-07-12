#include "stdafx.h"
#include <vector>
#include <complex>
#include <iostream>
#include <fstream>
#include <iomanip>
#define PI 3.1415926535

using namespace std;

unsigned char* readBMP(char * filename);
int power_of_2(int n, int *m, int *twopm);
int FFT(int dir, int m, double *x, double *y);
int FFT2D(complex<double> **c, int nx, int ny, int dir);
void swap(complex<double> *v1, complex<double> *v2);
void fftshift(complex<double> *data, int count);
void ifftshift(complex<double> *data, int count);

int main()
{
	int height, width, size;

	//the height and width of the intensity and phase must be the same

	char * filename_intensity, * filename_phase;
	filename_intensity = (char*)malloc(50); // 50 is the size limit of the file name length
	filename_phase = (char*)malloc(50);
	cin >> filename_intensity >> filename_phase;

	unsigned char* intensity = readBMP(filename_intensity);
	free(filename_intensity);
	size = (((int)(*(intensity - 1))) << 16) + (int)(*(intensity - 2)) + (int)(*(intensity - 3));
	height = ((int)(*(intensity + size))) << 8 + (int)(*(intensity + size + 1));
	width = ((int)(*(intensity + size + 2))) << 8 + (int)(*(intensity + size + 3));

	unsigned char* phase = readBMP(filename_phase);
	free(filename_phase);

	ofstream f;
	f.open("output.txt", ios::out);

	complex<double> **incident_beam = (complex<double>**)malloc(height * sizeof(complex<double>));

	for (int i = 0; i < width; i++) {          
		*(incident_beam + i) = (complex<double>*)malloc(width * sizeof(complex<double>));
	}

	for (int i = height; i > 0; i--) {
		for (int j = 0; j < width; j++) {
			double tmp1 = (int)(*(intensity + 3 * (i * width + j)));
			double tmp2 = 2 * PI*(int)(*(phase + 3 * (i * width + j))) / 255;
			*(*(incident_beam + (height-i)) + j) = complex<double>(tmp1*cos(tmp2), tmp1*sin(tmp2));
		}
	}
	FFT2D(incident_beam, width, height, 1);
	// finding the maximum for normalization
	int max = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int abs = sqrt((*(*(incident_beam + i) + j)).real()*(*(*(incident_beam + i) + j)).real() + (*(*(incident_beam + i) + j)).imag()*(*(*(incident_beam + i) + j)).imag());
			if (abs > max) {
				max = abs;
			}
		}
	}
	//--------------------------------------------
	for (int i = height; i > 0; i--) {
		for (int j = 0; j < width; j++) {
			f << 255*sqrt((*(*(incident_beam + (height - i)) + j)).real()*(*(*(incident_beam + (height - i)) + j)).real() + (*(*(incident_beam + (height - i)) + j)).imag()*(*(*(incident_beam + (height - i)) + j)).imag())/max << (char)9;
		}
		f << endl;
	}
	f.close();
	cout << "Done" << endl;
	system("pause>nul");
	return 0;
}

unsigned char* readBMP(char * filename)
{
	int height, width, i;
	FILE* f = fopen(filename, "rb"); // rb - read binary
	unsigned char *info = (unsigned char*)malloc(54);
	fread(info, 1, 54, f); // read the 54-byte header
								   
	width = *(int*)(info + 18); // extract image height and width from header
	height = *(int*)(info + 22);

	int size = 3 * width * height;
	unsigned char* data = (unsigned char*)malloc(size) + 4; // allocate 3 bytes per pixel
	fread(data, 1, size, f); // read the rest of the data at once
	fclose(f);

	*(data - 1) = size >> 16;
	*(data - 2) = (size & 65535) >> 8;
	*(data - 3) = size & 255;
	*(data + size) = height >> 8;
	*(data + size + 1) = height & 255;
	*(data + size + 2) = width >> 8;
	*(data + size + 3) = width & 255;

	return data;
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
		//--------------------------------------------
		// TODO 
		// it would be necessary to optimize somehow
		complex<double> *tmp = (complex<double>*)malloc(nx * sizeof(complex<double>));
		for (int k = 0; k < nx; k++) {
			*(tmp + k) = complex<double>(*(real + k), *(imag + k));
		}
		if (dir == 1) {
			fftshift(tmp, nx);
		}
		else if (dir == -1) {
			ifftshift(tmp, nx);
		}
		for (int k = 0; k < nx; k++) {
			*(real + k) = ((complex<double>)*(tmp + k)).real();
			*(imag + k) = ((complex<double>)*(tmp + k)).imag();
		}
		free(tmp);
		//--------------------------------------------
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
		//--------------------------------------------
		// TODO
		complex<double> *tmp = (complex<double>*)malloc(ny * sizeof(complex<double>));
		for (int k = 0; k < ny; k++) {
			*(tmp + k) = complex<double>(*(real + k), *(imag + k));
		}
		if (dir == 1) {
			fftshift(tmp, ny);
		}
		else if (dir == -1) {
			ifftshift(tmp, ny);
		}
		for (int k = 0; k < ny; k++) {
			*(real + k) = ((complex<double>)*(tmp + k)).real();
			*(imag + k) = ((complex<double>)*(tmp + k)).imag();
		}
		free(tmp);
		//--------------------------------------------
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


void swap(complex<double> *v1, complex<double> *v2)
{
	complex<double> tmp = *v1;
	*v1 = *v2;
	*v2 = tmp;
}

void fftshift(complex<double> *data, int count)
{
	int k = 0;
	int c = (int)floor((float)count / 2);
	if (count % 2 == 0)
	{
		for (k = 0; k < c; k++)
			swap(data + k, data + k + c);
	}
	else
	{
		complex<double> tmp = *data;
		for (k = 0; k < c; k++)
		{
			*(data + k) = *(data + c + k + 1);
			*(data + c + k + 1) = *(data + k + 1);
		}
		*(data + c) = tmp;
	}
}

void ifftshift(complex<double> *data, int count)
{
	int k = 0;
	int c = (int)floor((float)count / 2);
	if (count % 2 == 0)
	{
		for (k = 0; k < c; k++)
			swap(data + k, data + k + c);
	}
	else
	{
		complex<double> tmp = *(data + count - 1);
		for (k = c - 1; k >= 0; k--)
		{
			*(data + c + k + 1) = *(data + k);
			*(data + k) = *(data + c + k);
		}
		*(data + c) = tmp;
	}
}