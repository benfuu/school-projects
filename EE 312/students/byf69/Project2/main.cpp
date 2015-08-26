#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "MatrixMultiply.h"


void generateIdentityMatrix(double a[], const uint32_t rows, const uint32_t cols) {
  uint32_t i, j; // two loop variables, uninteresting in all respects.
  i = 0;
  while(i < rows) {
  	j = 0;
		while(j < cols) {
			if (i == j) { a[(i * cols) + j] = 1.0; }
			else { a[(i * cols) + j] = 0; }
			j += 1;
		}
		i += 1;
	}
}

void perturbMatrix(double a[], const uint32_t rows, const uint32_t cols,
									 uint32_t src_row[], double perturb_values[],
									 const uint32_t num_perturbations) {
  uint32_t i, k; // two loop variables, uninteresting in all respects.
  uint32_t dst_row; // index of the row being written into
  i = 0;
	while (i < num_perturbations) {
		assert(src_row[i] < rows);
		/* apply perturbation i to every row (except to itself) */
		dst_row = 0;
		while (dst_row < rows) {
			if (dst_row != src_row[i]) {
				/* take the src row, and scale it by the perturbation value
				 * and add the resulting row to the destination row */
				k = 0;
				while (k < cols) {
					a[(dst_row * cols) + k] += perturb_values[i] * a[(src_row[i] * cols) + k];
					k += 1;
				} // end for each element in a row 
			} // end if dst_row != src_row 
			dst_row += 1;
		} // end for each dst_row in the matrix 
		i += 1;
	} // end for each perturbation 
}

/*
 * generate a matrix with random elements.
 * the matrix should be stored in array a in row-major order
 * the parameters 'rows' and 'cols' specify the size of the matrix
 */
void generateRandomMatrix(double a[], const uint32_t rows, const uint32_t cols) {
	const int max_value = 100; // maximum scale factor to apply to a row
	const int perturbations = 7;
	uint32_t src_rows[7];
	double perturb_values[7];
	int k; // an unininteresting loop variable

	/* randomly perturb the matrix seven times, that should scramble things pretty well */
	generateIdentityMatrix(a, rows, cols);

	k = 0;
	while (k < perturbations) {
		src_rows[k] = rand() % rows;
		perturb_values[k] = max_value - (rand() % (2 * max_value));
		k += 1;
	}
	
	perturbMatrix(a, rows, cols, src_rows, perturb_values, perturbations);
}

int assertEqualMatrices(double a[], double b[], const uint32_t rows, const uint32_t cols) {
  uint32_t i, j; // two loop variables, uninteresting in all respects.

	i = 0;
	while (i < rows) {
	  j = 0;
		while (j < cols) {
			double difference = fabs(a[(i * cols) + j] - b[(i * cols) + j]);
			if (difference > 1.0e-6) {
				fprintf(stderr, 
					"Sorry, matrices do not match, first difference at row %d and col %d\n",
					i, j);
				return false;
			}
			j += 1;
		}
		i += 1;
	}
	return true;
}

void copyMatrix(double dest[], double src[], const uint32_t rows, const uint32_t cols) {
	uint32_t i = 0;
	while (i < rows) {
		uint32_t j = 0;
		while (j < cols) {
			dest[(i * cols) + j] = src[(i * cols) + j];
			j += 1;
		}
		i += 1;
	}
}


void printMatrix(double a[], const uint32_t rows, const uint32_t cols) {

	uint32_t i = 0;
	while (i < rows) {
		uint32_t j = 0;
		while (j < cols) {
			printf("%g, ", a[(i * cols) + j]); // %g is just like %f, only better
			j += 1;
		}
		printf("\n");
		i += 1;
	}
}


void testStage1(void) {
	double a[100];
	double b[100];
	double c[100];

  /* these arrays are used only for test C */
	uint32_t perturb_rows[5] = { 2, 4, 6, 1, 7 };
	double perturb_vals[5] = { 1.0, -1.0, 2.0, -2.5, 3.0};
	double expected[] = {
		1, -10, 0, 0, 18, 0, -12, 3, 0, 0, 
		0, -6.5, 0, 0, 10.5, 0, -7, 3, 0, 0, 
		0, -10, 0, 0, 18, 0, -12, 3, 0, 0, 
		0, -10, 0, 1, 18, 0, -12, 3, 0, 0, 
		0, -10, 1, 0, 20, 0, -12, 3, 0, 0, 
		0, -10, 0, 0, 18, 1, -12, 3, 0, 0, 
		0, -10, 0, 0, 20, 0, -13, 3, 0, 0, 
		0, -2.5, 0, 0, 4.5, 0, -3, 1, 0, 0, 
		0, -10, 0, 0, 18, 0, -12, 3, 1, 0, 
		0, -10, 0, 0, 18, 0, -12, 3, 0, 1 };


	generateRandomMatrix(a, 10, 10);
	generateIdentityMatrix(b, 10, 10);
	multiplyMatrices(a, 10, 10, b, 10, c);
	printf("Stage 1 test A:...");
	if (! assertEqualMatrices(a, c, 10, 10)) {
		return;
	}
	printf("passed :-)\n");
	
	multiplyMatrices(b, 10, 10, a, 10, c);
	printf("Stage 1 test B:...");
	if (! assertEqualMatrices(a, c, 10, 10)) {
		return;
	}
	printf("passed :-)\n");
	
	generateIdentityMatrix(a, 10, 10);
	perturbMatrix(a, 10, 10, perturb_rows, perturb_vals, 5);
	
	copyMatrix(a, b, 10, 10);
	perturbMatrix(b, 10, 10, perturb_rows, perturb_vals, 5);	
	multiplyMatrices(a, 10, 10, b, 10, c);

	printf("Stage 1 test C:...");
	
	if (! assertEqualMatrices(c, expected, 10, 10)) {
		return;
	}		
	printf("passed! :-)\n");
}

void testStage2(void) {
	double a[100];
	double b[100];
	double c[100];
	PtrDouble matrix_list[5];
	uint32_t rows[5];
	uint32_t cols[5];
	double a_squared[100];
	double a_cubed[100];
	int k;
	
	generateRandomMatrix(a, 10, 10);
	generateIdentityMatrix(b, 10, 10);
	
	printf("Stage 2 test A: chain of identy matrices...");
	matrix_list[0] = a;
	rows[0] = 10;
	cols[0] = 10;
	k = 1;
	while (k < 5) {
		matrix_list[k] = b;
		rows[k] = 10;
		cols[k] = 10;
		k += 1;
	}
	multiplyMatrixChain(matrix_list, rows, cols, 5, c, 10, 10);
	if (! assertEqualMatrices(a, c, 10, 10)) { 
		return;
	}
	printf("passed :-)\n");
	
	printf("Stage 2 test B: chain of square matrices...");
	multiplyMatrices(a, 10, 10, a, 10, a_squared);
	matrix_list[2] = a;
	multiplyMatrixChain(matrix_list, rows, cols, 5, c, 10, 10);
	if (! assertEqualMatrices(a_squared, c, 10, 10)) {
		return;
	}
	printf("passed :-)\n");

	printf("Stage 2 test C: chain of square matrices...");
	multiplyMatrices(a_squared, 10, 10, a, 10, a_cubed);
	matrix_list[4] = a;
	multiplyMatrixChain(matrix_list, rows, cols, 5, c, 10, 10);
	if (! assertEqualMatrices(a_cubed, c, 10, 10)) {
		return;
	}
	printf("passed :-)\n");
}

void testStage3(void) {
	double a[100];
	double b[50];
	double c[50];
	PtrDouble matrix_list[5];
	uint32_t rows[5];
	uint32_t cols[5];
	double d[100];
	double expected[100];
	uint32_t i, j, k; // uninteresting loop variables
	
	generateIdentityMatrix(a, 10, 10);
	generateIdentityMatrix(b, 10, 5);
	generateIdentityMatrix(c, 5, 10);
	
	/* a is 10x10, 
	 * ab is 10x5
	 * abc is 10x10
	 * abcb is 10x5
	 * abcbc is 10x10
	 */
	

	printf("Stage 3 chains of rectangular matrices...");
	matrix_list[0] = a;
	rows[0] = 10;
	cols[0] = 10;
	k = 1;
	while (k < 5) {
		matrix_list[k] = b;
		rows[k] = 10;
		cols[k] = 5;
		k += 2;
	}
	
	k = 2;
	while (k < 5) {
		matrix_list[k] = c;
		rows[k] = 5;
		cols[k] = 10;
		k += 2;
	}
	multiplyMatrixChain(matrix_list, rows, cols, 5, d, 10, 10);
	
	copyMatrix(expected, a, 10, 10);
	i = 5;
	while (i < 10) {
	  j = 5;
		while (j < 10) {
			expected[(i * 10) + j] = 0.0;
			j += 1;
		}
		i += 1;
	}
	
	if (! assertEqualMatrices(expected, d, 10, 10)) { 
		return;
	}
	printf("passed :-)\n");
}


int main(void) {
	testStage1();
	testStage2();
	testStage3();
	return 0;
}