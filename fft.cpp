#include "stdafx.h"
#include <vector>
#include <complex>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int width;
int height;

unsigned char* readBMP(char * filename);
int power_of_2(int n, int *m, int *twopm);
int FFT(int dir, int m, double *x, double *y);
int FFT2D(complex<double> **c, int nx, int ny, int dir);
void swap(complex<double> *v1, complex<double> *v2);
void fftshift(complex<double> *data, int count);
void ifftshift(complex<double> *data, int count);

int main()
{

	char * filename;
	filename = new char[50]; // 50 is the size limit of the file name length
	cin >> filename;
	unsigned char* result = readBMP(filename);
	free(filename);
	ofstream f;
	f.open("output.txt", ios::out);
	complex<double> **incident_beam = new complex<double>*[height];   // создаем двумерный массив 
	for (int i = 0; i < width; i++) {          
		incident_beam[i] = new complex<double>[width]; 
	}
	for (int i = height; i > 0; i--) {
		for (int j = 0; j < width; j++) {
			incident_beam[height - i][j] = complex<double>((int)result[3 * (i * width + j)], 0);
		}
	}
	FFT2D(incident_beam, width, height, 1);
	// finding the maximum for normalization
	int max = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int abs = sqrt(incident_beam[i][j].real()*incident_beam[i][j].real() + incident_beam[i][j].imag()*incident_beam[i][j].imag());
			if (abs > max) {
				max = abs;
			}
		}
	}
	//--------------------------------------------
	for (int i = height; i > 0; i--) {
		for (int j = 0; j < width; j++) {
			f << /*255**/sqrt(incident_beam[height - i][j].real()*incident_beam[height - i][j].real() + incident_beam[height - i][j].imag()*incident_beam[height - i][j].imag())/*/max*/ << (char)9;
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
	int i;
	FILE* f = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
								   
	width = *(int*)&info[18]; // extract image height and width from header
	height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);

	for (i = 0; i < size; i += 3)
	{
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

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
			real[i] = c[i][j].real();
			imag[i] = c[i][j].imag();
		}
		FFT(dir, m, real, imag);
		//--------------------------------------------
		// TODO 
		// it would be necessary to optimize somehow
		complex<double> *tmp = new complex<double>[nx];
		for (int k = 0; k < nx; k++) {
			tmp[k] = complex<double>(real[k], imag[k]);
		}
		if (dir == 1) {
			fftshift(tmp, nx);
		}
		else if (dir == -1) {
			ifftshift(tmp, nx);
		}
		for (int k = 0; k < nx; k++) {
			real[k] = tmp[k].real();
			imag[k] = tmp[k].imag();
		}
		free(tmp);
		//--------------------------------------------
		for (i = 0; i < nx; i++) {
			c[i][j] = complex<double>(real[i], imag[i]);
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
			real[j] = c[i][j].real();
			imag[j] = c[i][j].imag();
		}
		FFT(dir, m, real, imag);
		//--------------------------------------------
		// TODO
		complex<double> *tmp = new complex<double>[ny];
		for (int k = 0; k < ny; k++) {
			tmp[k] = complex<double>(real[k], imag[k]);
		}
		if (dir == 1) {
			fftshift(tmp, ny);
		}
		else if (dir == -1) {
			ifftshift(tmp, ny);
		}
		for (int k = 0; k < ny; k++) {
			real[k] = tmp[k].real();
			imag[k] = tmp[k].imag();
		}
		free(tmp);
		//--------------------------------------------
		for (j = 0; j < ny; j++) {
			c[i][j] = complex<double>(real[j], imag[j]);
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
	for (i = 0; i<m; i++)
		nn *= 2;

	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for (i = 0; i<nn - 1; i++) {
		if (i < j) {
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
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
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l<m; l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j<l1; j++) {
			for (i = j; i<nn; i += l2) {
				i1 = i + l1;
				t1 = u1 * x[i1] - u2 * y[i1];
				t2 = u1 * y[i1] + u2 * x[i1];
				x[i1] = x[i] - t1;
				y[i1] = y[i] - t2;
				x[i] += t1;
				y[i] += t2;
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
		for (i = 0; i<nn; i++) {
			x[i] /= (double)nn;
			y[i] /= (double)nn;
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
			swap(&data[k], &data[k + c]);
	}
	else
	{
		complex<double> tmp = data[0];
		for (k = 0; k < c; k++)
		{
			data[k] = data[c + k + 1];
			data[c + k + 1] = data[k + 1];
		}
		data[c] = tmp;
	}
}

void ifftshift(complex<double> *data, int count)
{
	int k = 0;
	int c = (int)floor((float)count / 2);
	if (count % 2 == 0)
	{
		for (k = 0; k < c; k++)
			swap(&data[k], &data[k + c]);
	}
	else
	{
		complex<double> tmp = data[count - 1];
		for (k = c - 1; k >= 0; k--)
		{
			data[c + k + 1] = data[k];
			data[k] = data[c + k];
		}
		data[c] = tmp;
	}
}