#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_ROWS 3           // This is the dimension of matrix A (row)
#define MAX_COLS 3           // This is the dimension of matrix A (col)
#define TOTAL_BITS 16        // 16-bit word length
#define INITIAL_INT_BITS 6   // Minimum integer bits
#define INITIAL_FRAC_BITS 2  // Start with Q14.2
#define MAX_FRAC_BITS 10     // Maximum fractional bits

typedef int16_t fixed_t;     // we can use fixed_t instead of int16_t now

/* ------------------------------------------------------------------
 * Inline Functions for Fixed-Point Arithmetic
 * ------------------------------------------------------------------ */

// Converts float -> fixed
static inline fixed_t float_to_fixed(float x, int frac_bits)
{
    return (fixed_t)(x * (1 << frac_bits) + (x >= 0 ? 0.5f : -0.5f)); //converts to fixed point format and prevents rounding errors
}

// Converts fixed -> float
static inline float fixed_to_float(fixed_t x, int frac_bits)
{
    return (float)x / (float)(1 << frac_bits);
}

// Multiplies two fixed-point numbers
static inline fixed_t fixed_multiply(fixed_t a, fixed_t b, int frac_bits)
{
    int32_t product = (int32_t)a * (int32_t)b;
    return (fixed_t)((product + (1 << (frac_bits - 1))) >> frac_bits);
}

// Divides a by b in fixed-point
static inline fixed_t fixed_divide(fixed_t a, fixed_t b, int frac_bits)
{
    if (b == 0) return 0; // or handle error
    int32_t numerator = (int32_t)a << frac_bits;
    return (fixed_t)(numerator / b);
}

/* ------------------------------------------------------------------
 * Utility: Re-scale 1D array by multiplying each element by 2
 * (Because when frac_bits++, we are effectively doubling scale).
 * ------------------------------------------------------------------ */
static inline void re_scale_array(fixed_t *arr, int size)
{
    // Multiply each element by 2
    for (int i = 0; i < size; i++)
    {
        // Since it's 1 bit shift, we do << 1
        arr[i] <<= 1;
    }
}

/* ------------------------------------------------------------------
 * Utility: Re-scale 2D array by multiplying each element by 2
 * If you decide to also re-scale matrix data, use this.
 * (We skip it in the solver below, but you could do it if needed.)
 * ------------------------------------------------------------------ */
static inline void re_scale_array_2d(fixed_t arr[MAX_ROWS][MAX_COLS], int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            arr[i][j] <<= 1;
        }
    }
}

fixed_t fixed_sqrt(fixed_t number, int frac_bits) //Newton-Raphson Square Root in Fixed-Point
{
    if (number < 0) return -1;
    fixed_t tolerance = float_to_fixed(1e-4f, frac_bits);

    fixed_t guess = (number > float_to_fixed(1.0f, frac_bits))
                  ? (number >> 1)
                  : float_to_fixed(1.0f, frac_bits);

    fixed_t diff;
    do {
        fixed_t temp       = fixed_divide(number, guess, frac_bits);
        fixed_t sum        = guess + temp;
        fixed_t next_guess = fixed_divide(sum, float_to_fixed(2.0f, frac_bits), frac_bits);

        diff  = next_guess - guess;
        guess = next_guess;
    } while (diff > tolerance || diff < -tolerance);

    return guess;
}


fixed_t vector_norm(fixed_t *vec, int size, int frac_bits) //Vector norm function in fixed point
{
    fixed_t sum = 0;
    for (int i = 0; i < size; i++) {
        sum += fixed_multiply(vec[i], vec[i], frac_bits);
    }
    return fixed_sqrt(sum, frac_bits);
}

fixed_t matrix_norm(fixed_t matrix[MAX_ROWS][MAX_COLS], int rows, int cols, int frac_bits) //Matrix norm function in fixed point
{
    fixed_t sum = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sum += fixed_multiply(matrix[i][j], matrix[i][j], frac_bits);
        }
    }
    return fixed_sqrt(sum, frac_bits);
}

void normalize_matrix(fixed_t inMat[MAX_ROWS][MAX_COLS], fixed_t outMat[MAX_ROWS][MAX_COLS],
                      int rows, int cols, fixed_t norm, int frac_bits)    //Normalization of matrix function
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            outMat[i][j] = fixed_divide(inMat[i][j], norm, frac_bits);
        }
    }
}

void normalize_vector(fixed_t *inVec, fixed_t *outVec, int size, fixed_t norm, int frac_bits) //Normalization of vector function
{
    for (int i = 0; i < size; i++) {
        outVec[i] = fixed_divide(inVec[i], norm, frac_bits);
    }
}


void matrix_vector_multiply(fixed_t matrix[MAX_ROWS][MAX_COLS], fixed_t *vector, fixed_t *result,
                            int rows, int cols, int frac_bits)
{
    for (int i = 0; i < rows; i++) {
        fixed_t accum = 0;
        for (int j = 0; j < cols; j++) {
            accum += fixed_multiply(matrix[i][j], vector[j], frac_bits);
        }
        result[i] = accum;
    }
}

void matrix_multiply(fixed_t mat1[MAX_ROWS][MAX_COLS], fixed_t mat2[MAX_ROWS][MAX_COLS],
                     fixed_t result[MAX_ROWS][MAX_COLS],
                     int rows1, int cols1, int cols2, int frac_bits)
{
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            fixed_t accum = 0;
            for (int k = 0; k < cols1; k++) {
                accum += fixed_multiply(mat1[i][k], mat2[k][j], frac_bits);
            }
            result[i][j] = accum;
        }
    }
}

void transpose_matrix(fixed_t input[MAX_ROWS][MAX_COLS], fixed_t output[MAX_COLS][MAX_ROWS],
                      int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            output[j][i] = input[i][j];
        }
    }
}

void vector_subtract(fixed_t *vec1, fixed_t *vec2, fixed_t *result, int size)
{
    for (int i = 0; i < size; i++) {
        result[i] = vec1[i] - vec2[i];
    }
}

void vector_add(fixed_t *vec1, fixed_t *vec2, fixed_t *result, int size)
{
    for (int i = 0; i < size; i++) {
        result[i] = vec1[i] + vec2[i];
    }
}


void solve_fixed_point(fixed_t A[MAX_ROWS][MAX_COLS], fixed_t *y, int M, fixed_t chi, fixed_t err) // Main Iterative Solver with fractional bits re-scaling in the inner loop
{
    fixed_t Anorm[MAX_ROWS][MAX_COLS] = {{0}};
    fixed_t ynorm[MAX_ROWS]           = {0};
    fixed_t x[MAX_COLS]               = {0};
    fixed_t r[MAX_ROWS];
    fixed_t dx[MAX_COLS];
    fixed_t dxk[MAX_COLS];
    fixed_t dxk_prev[MAX_COLS];
    fixed_t diff_dxk[MAX_COLS]        = {0};
    fixed_t AtAdxk_prev[MAX_COLS]     = {0};

    int frac_bits = INITIAL_FRAC_BITS;

    // 1. Normalize A & y
    fixed_t norm_A = matrix_norm(A, MAX_ROWS, MAX_COLS, frac_bits);
    normalize_matrix(A, Anorm, MAX_ROWS, MAX_COLS, norm_A, frac_bits);

    fixed_t norm_y = matrix_norm(y, MAX_ROWS, 1, frac_bits);
    normalize_vector(y, ynorm, MAX_ROWS, norm_y, frac_bits);

    memcpy(r, ynorm, MAX_ROWS * sizeof(fixed_t));

    // 2. Prepare At, AtA
    fixed_t At[MAX_COLS][MAX_ROWS];
    fixed_t AtA[MAX_COLS][MAX_COLS];
    transpose_matrix(Anorm, At, MAX_ROWS, MAX_COLS);
    matrix_multiply(At, Anorm, AtA, MAX_COLS, MAX_ROWS, MAX_COLS, frac_bits);

    // tau = (2 - chi) / norm(AtA)
    fixed_t two_minus_chi = float_to_fixed(2.0f, frac_bits) - chi;
    fixed_t norm_AtA      = matrix_norm(AtA, MAX_COLS, MAX_COLS, frac_bits);
    fixed_t tau           = fixed_divide(two_minus_chi, norm_AtA, frac_bits);

    // Outer loop
    for (int outer_i = 0; outer_i < M; outer_i++)
    {
        // b = tau * (At * r)
        matrix_vector_multiply(At, r, dx, MAX_COLS, MAX_ROWS, frac_bits);
        for (int j = 0; j < MAX_COLS; j++) {
            dx[j] = fixed_multiply(dx[j], tau, frac_bits);
        }

        // Inner loop
        int k = 1;
        memset(dxk, 0, sizeof(dxk));
        memset(dxk_prev, 0, sizeof(dxk_prev));
        memset(diff_dxk, 0, sizeof(diff_dxk));

        while (!((vector_norm(diff_dxk, MAX_COLS, frac_bits) < err) && (k != 1)))
        {
            if (k == 1)
            {
                memcpy(dxk, dx, MAX_COLS * sizeof(fixed_t));
            }
            else
            {
                // dxk = dxk - (AtA * dxk) + dx
                matrix_vector_multiply(AtA, dxk, AtAdxk_prev, MAX_COLS, MAX_COLS, frac_bits);
                vector_subtract(dxk, AtAdxk_prev, dxk, MAX_COLS);
                vector_add(dxk, dx, dxk, MAX_COLS);
            }

            // Compute difference
            vector_subtract(dxk, dxk_prev, diff_dxk, MAX_COLS);

            // Example: every 2 steps, if still not converged, increase frac_bits
            if ((k % 2 == 0) && (frac_bits < MAX_FRAC_BITS))
            {
                frac_bits++;

                // Re-scale solver arrays to reflect newly added fractional bit
                re_scale_array(dxk,       MAX_COLS);
                re_scale_array(dxk_prev,  MAX_COLS);
                re_scale_array(diff_dxk,  MAX_COLS);
                re_scale_array(x,         MAX_COLS);
                re_scale_array(r,         MAX_ROWS);
                // If you stored b, you might re-scale that too,
                // but often b gets recalculated each iteration anyway.
            }

            memcpy(dxk_prev, dxk, MAX_COLS * sizeof(fixed_t));
            k++;
        }

        // dx = dxk
        memcpy(dx, dxk, MAX_COLS * sizeof(fixed_t));

        // x = x + dx
        vector_add(x, dx, x, MAX_COLS);

        // r = ynorm - (Anorm*x)
        matrix_vector_multiply(Anorm, x, r, MAX_ROWS, MAX_COLS, frac_bits);
        for (int j = 0; j < MAX_ROWS; j++)
        {
            r[j] = ynorm[j] - r[j];
        }
    }

    // Scale x back to original magnitude: x[i] *= (norm_y / norm_A)
    for (int i = 0; i < MAX_COLS; i++)
    {
        float floatX = fixed_to_float(x[i],       frac_bits);
        float floatY = fixed_to_float(norm_y,     frac_bits);
        float floatA = fixed_to_float(norm_A,     frac_bits);

        float scaled  = floatX * (floatY / floatA);
        x[i] = float_to_fixed(scaled, frac_bits);

        printf("Final x[%d] = %.6f\n", i, fixed_to_float(x[i], frac_bits));
    }
}


int main(void)
{
    // 3x3 matrix A to test the functionality
    fixed_t A[MAX_ROWS][MAX_COLS] = {
        { float_to_fixed(2.0f, INITIAL_FRAC_BITS), float_to_fixed(3.0f, INITIAL_FRAC_BITS), float_to_fixed(1.0f, INITIAL_FRAC_BITS) },
        { float_to_fixed(1.0f, INITIAL_FRAC_BITS), float_to_fixed(1.0f, INITIAL_FRAC_BITS), float_to_fixed(1.0f, INITIAL_FRAC_BITS) },
        { float_to_fixed(3.0f, INITIAL_FRAC_BITS), float_to_fixed(2.0f, INITIAL_FRAC_BITS), float_to_fixed(2.0f, INITIAL_FRAC_BITS) }
    };

    // 3x1 vector y
    fixed_t y[MAX_ROWS] = {
        float_to_fixed(13.0f, INITIAL_FRAC_BITS),
        float_to_fixed(6.0f,  INITIAL_FRAC_BITS),
        float_to_fixed(15.0f, INITIAL_FRAC_BITS)
    };

    // Convert chi & err to fixed
    fixed_t chi_fixed = float_to_fixed(1.0f, INITIAL_FRAC_BITS);
    fixed_t err_fixed = float_to_fixed(1e-6f, INITIAL_FRAC_BITS);

    // Solve with 10 outer iterations
    solve_fixed_point(A, y, 10, chi_fixed, err_fixed);

    return 0;
}
