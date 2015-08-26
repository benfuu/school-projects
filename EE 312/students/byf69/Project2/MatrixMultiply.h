/*
 * MatrixMultiply.h
 *
 * function declarations (prototypes) and global variable declarations
 * for the Matrix Multiplication project
 */

#ifndef _MatrixMultiply_h
#define _MatrixMultiply_h 1

/* typedef can be used to create a type alias. The statement:
 *		typedef oldType newType;
 * makes newType into an alias of oldType. oldType must actually be a type (e.g., int, double, double*)
 * and newType must be a name (e.g., int32_t, IEEEFloat, PtrDouble)
 * 
 * I'm creating an alias for double* 
 * I'm naming that alias PtrDouble
 * in that way, I can use the name PtrDouble whenever I need a pointer to a double
 * That makes it much easier for me to declare and work with arrays of pointers to doubles
 * i.e., the matrix list is an array
 * each element in the array is a pointer
 * and each pointer points to the first element (a double) in a matrix
 * so the matrix list is an array of PtrDouble elements, 
 * hence PtrDouble matrix_list[]
 */
typedef double* PtrDouble;

void multiplyMatrices(double a[], const unsigned a_rows, const unsigned a_cols,
										  double b[], const unsigned b_cols,
										  double c[]);

void multiplyMatrixChain(PtrDouble matrix_list[], // an array of pointers to matrices
												 unsigned rows[], // rows[i] is the number of rows in matrix_list[i]
												 unsigned cols[], // cols[i] is the number of cols in matrix_list[i]
												 unsigned num_matrices, // the number of matrices in matrix_list
												 double out[], // the output matrix
												 unsigned out_rows, unsigned out_cols); // the dimensions of the output matrix


#endif _Project8_h