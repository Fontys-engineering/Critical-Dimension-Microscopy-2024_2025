#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define MAX_ROWS 3 // This is the dimension of matrix A (row)
#define MAX_COLS 3 // This is the dimension of matrix A (col)

// Custom implementation of the square root function since there's a compiler error for the default sqrt fucntion
double custom_sqrt(double number) {
    if (number < 0.0) {
        return -1.0; // Return -1.0 for negative input to indicate an error
    }

    double tolerance = 1e-15; // Double precision tolerance (15 decimal places)
    double guess = number > 1.0 ? number / 2.0 : 1.0; // Improved initial guess
    double diff;

    do {
        double next_guess = 0.5 * (guess + number / guess); // Newton-Raphson formula
        diff = next_guess - guess;
        guess = next_guess;
    } while (diff > tolerance || diff < -tolerance); // Continue until within tolerance

    return guess;
}

// Function to calculate the norm of a vector
double vector_norm(double *vec, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += vec[i] * vec[i];
    }
    return custom_sqrt(sum);
}

// Function to calculate the norm of a matrix
double matrix_norm(double matrix[MAX_ROWS][MAX_COLS], int rows, int cols) {
    double sum = 0.0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sum += matrix[i][j] * matrix[i][j];
        }
    }
    return custom_sqrt(sum);
}

// Function to normalize a matrix to scale them between -1 and 1
void normalize_matrix(double matrix[MAX_ROWS][MAX_COLS], double result[MAX_ROWS][MAX_COLS], int rows, int cols, double norm) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = matrix[i][j] / norm;
        }
    }
}

// Function to normalize a vector to scale them between -1 and 1
void normalize_vector(double *vec, double *result, int size, double norm) {
    for (int i = 0; i < size; i++) {
        result[i] = vec[i] / norm;
    }
}

// Function to multiply a matrix by a vector for the purpose of test -- This will be acquired from PL via AXI bus
void matrix_vector_multiply(double matrix[MAX_ROWS][MAX_COLS], double *vector, double *result, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        result[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

// Function to multiply two matrices for the purpose of test -- This will be acquired from PL via AXI bus
void matrix_multiply(double mat1[MAX_ROWS][MAX_COLS], double mat2[MAX_ROWS][MAX_COLS], double result[MAX_ROWS][MAX_COLS], int rows1, int cols1, int cols2) {
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < cols1; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

// Function to transpose a matrix to be able to acquire A' (A transpose)
void transpose_matrix(double input[MAX_ROWS][MAX_COLS], double output[MAX_COLS][MAX_ROWS], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            output[j][i] = input[i][j]; // Swapping indices row to col
        }
    }
}


// Function to subtract two vectors with the same size
void vector_subtract(double *vec1, double *vec2, double *result, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = vec1[i] - vec2[i];
    }
}

// Function to add two vectors with the same size
void vector_add(double *vec1, double *vec2, double *result, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = vec1[i] + vec2[i];
    }
}


//Main Iterative solver function to be called in main()
void solve_fixed_point(double A[MAX_ROWS][MAX_COLS], double *y, int M, double chi, double err) { //passing A, y, M(#of outer loop), chi(safety margin), err (error tolerance)
    double Anorm[MAX_ROWS][MAX_COLS] = {0};
    double ynorm[MAX_ROWS] = {0};
    double x[MAX_COLS] = {0};
    double r[MAX_ROWS] = {0};
    double dx[MAX_COLS] = {0};
    //double dx_prev[MAX_COLS] = {0};
    double b[MAX_COLS] = {0};
    double dxk[MAX_COLS] = {0};
    double dxk_prev[MAX_COLS] = {0};
    double diff_dxk[MAX_COLS] = {0};
    double zero[MAX_COLS] = {0};  // will also be used as initial guess vector
    double AtAdxk_prev[MAX_COLS] = {0};


    double norm_A = matrix_norm(A, MAX_ROWS, MAX_COLS);
    normalize_matrix(A, Anorm, MAX_ROWS, MAX_COLS, norm_A);
    double norm_y = vector_norm(y, MAX_ROWS);
    normalize_vector(y, ynorm, MAX_ROWS, norm_y);

    memcpy(r, ynorm, MAX_ROWS * sizeof(double)); //initially r = y

    double At[MAX_COLS][MAX_ROWS] = {0};                  //define A' (transpose)
    transpose_matrix(Anorm, At, MAX_ROWS, MAX_COLS);      //At is also normalized since
    												                              //Since instead of A, Anorm is used
    double AtA[MAX_COLS][MAX_COLS] = {0};			            //Stores normalized AtA
    matrix_multiply(At, Anorm, AtA, MAX_COLS, MAX_ROWS, MAX_COLS);
    double tau = (2.0 - chi) / matrix_norm(AtA, MAX_COLS, MAX_COLS);

    printf("Step size tau: %.6f\n", tau);


    int k = 1; // Number of inner loop iterations

    //OUTER LOOP START
    for (int i = 0; i < M; i++) {
        matrix_vector_multiply(At, r, b, MAX_COLS, MAX_ROWS); // calculate correction vector b = tau * Anorm' * r
        for (int j = 0; j < MAX_COLS; j++) {
            b[j] *= tau;
        }

        //INNER LOOP START
        while( !((vector_norm(diff_dxk,MAX_COLS) < err) && !(k == 1)) )
        {
        	if(k==1){
        		memcpy(dxk_prev, zero, MAX_COLS * sizeof(double)); //dxk_prev = 0
        		memcpy(dxk, b, MAX_COLS * sizeof(double)); //dxk = b
        	}
        	else{
        		memcpy(dxk_prev, dxk, MAX_COLS * sizeof(double)); 				          //dxk_prev = dxk, updating the previos dxk before updating the dxk itself
        		matrix_vector_multiply(AtA, dxk, AtAdxk_prev, MAX_COLS, MAX_COLS);  //Multiply AtA (n x n) with dxk_prev (n x 1) and store in AtAdxk (n x 1)
        		vector_subtract(dxk, AtAdxk_prev, dxk, MAX_COLS); 				          //dxk = dxk - AtAdxk_prev
        		vector_add(dxk, b, dxk, MAX_COLS); 								                  //dxk = dxk - AtAdxk_prev + b
        	}
        	k = k+1; 														                                  //Increase k so that it won't give error in the next iteration
            vector_subtract(dxk, dxk_prev, diff_dxk, MAX_COLS); 			          //Update dxk-dxk_prev value to be checked at the next while condition
        }//INNER LOOP END

        memcpy(dx, dxk, MAX_COLS * sizeof(double)); 						//dx = dxk

       if(i ==1 ){
    	   vector_add(dx, zero, x, MAX_COLS); 									 // x = X0 + dx
       }
       else
       {
    	   vector_add(dx, x, x, MAX_COLS);										  // x = x + dx
       }
       k = 1;  																	              // reset k to 1
       matrix_vector_multiply(Anorm, x, r, MAX_ROWS, MAX_COLS);
       for (int j = 0; j < MAX_ROWS; j++) {										// r = ynorm - Anorm*x
           r[j] = ynorm[j] - r[j];
       }
    }//OUTER LOOP END

 //Scale x back to its original value since both A and y were normalized before
    for (int i = 0; i < MAX_COLS; i++) {
        x[i] *= norm_y / norm_A;
        printf("%.6f\n", x[i]);
    }
} //solver function end

int main() {
    double A[MAX_ROWS][MAX_COLS] = { //This is the test sytem matrix
        {2, 3, 1},
        {1, 1, 1},
        {3, 2, 2}
    };
    double y[MAX_ROWS] = {13, 6, 15}; // This is the output vector
    solve_fixed_point(A, y, 10, 1.0, 1e-6); //This should return approximately x = (3;2;1)
    return 0;
}
