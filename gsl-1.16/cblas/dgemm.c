#include <gsl/gsl_math.h>
#include <gsl/gsl_cblas.h>
#include "cblas.h"
#include "error_cblas_l3.h"

void
cblas_dgemm (const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
			 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
			 const int K, const double alpha, const double *A, const int lda,
			 const double *B, const int ldb, const double beta, double *C,
			 const int ldc)
{
#define BASE double
//#include "source_gemm_r.h"

	INDEX i, j, k;
	INDEX n1, n2;
	INDEX ldf, ldg;
	int TransF, TransG;
	__declspec(align(32)) const BASE *F, *G;


	CHECK_ARGS14(GEMM, Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);

	if (alpha == 0.0 && beta == 1.0)
		return;

	if (Order == CblasRowMajor) {
		n1 = M;
		n2 = N;
		F = A;
		ldf = lda;
		TransF = (TransA == CblasConjTrans) ? CblasTrans : TransA;
		G = B;
		ldg = ldb;
		TransG = (TransB == CblasConjTrans) ? CblasTrans : TransB;
	}
	else {
		n1 = N;
		n2 = M;
		F = B;
		ldf = ldb;
		TransF = (TransB == CblasConjTrans) ? CblasTrans : TransB;
		G = A;
		ldg = lda;
		TransG = (TransA == CblasConjTrans) ? CblasTrans : TransA;
	}

	/* form  y := beta*y */


	if (beta == 0.0) {
#pragma parallel
		for (i = 0; i < n1; i++) {
			for (j = 0; j < n2; j++) {
				C[ldc * i + j] = 0.0;
			}
		}
	}
	else if (beta != 1.0) {
#pragma parallel
		for (i = 0; i < n1; i++) {
			for (j = 0; j < n2; j++) {
				C[ldc * i + j] *= beta;
			}
		}
	}

	if (alpha == 0.0)
		return;

	if (alpha == 1.0)
	{
		if (TransF == CblasNoTrans && TransG == CblasNoTrans) {

			/* form  C := alpha*A*B + C */

#pragma parallel
			for (k = 0; k < K; k++) {

				for (i = 0; i < n1; i++) {
					const BASE temp = F[ldf * i + k];
					if (temp != 0.0) {
						for (j = 0; j < n2; j++) {
							C[ldc * i + j] += temp * G[ldg * k + j];
						}
					}
				}
			}

		}
		else if (TransF == CblasNoTrans && TransG == CblasTrans) {

			/* form  C := alpha*A*B' + C */

#pragma parallel
			for (i = 0; i < n1; i++) {
				for (j = 0; j < n2; j++) {
					BASE temp = 0.0;
					for (k = 0; k < K; k++) {
						temp += F[ldf * i + k] * G[ldg * j + k];
					}
					C[ldc * i + j] += temp;
				}
			}

		}
		else if (TransF == CblasTrans && TransG == CblasNoTrans) {


#pragma parallel
			for (k = 0; k < K; k++) {

				for (i = 0; i < n1; i++) {
					const BASE temp = F[ldf * k + i];
					if (temp != 0.0) {
						for (j = 0; j < n2; j++) {
							C[ldc * i + j] += temp * G[ldg * k + j];
						}
					}
				}
			}

		}
		else if (TransF == CblasTrans && TransG == CblasTrans) {
#pragma parallel
			for (i = 0; i < n1; i++) {

				for (j = 0; j < n2; j++) {
					BASE temp = 0.0;
					for (k = 0; k < K; k++) {
						temp += F[ldf * k + i] * G[ldg * j + k];
					}
					C[ldc * i + j] += temp;
				}
			}

		}
		else {
			BLAS_ERROR("unrecognized operation");
		}
	}
	else
	{
		if (TransF == CblasNoTrans && TransG == CblasNoTrans) {

			/* form  C := alpha*A*B + C */

			
#pragma parallel
			for (k = 0; k < K; k++) {

				for (i = 0; i < n1; i++) {
					const BASE temp = alpha * F[ldf * i + k];
					if (temp != 0.0) {
						for (j = 0; j < n2; j++) {
							C[ldc * i + j] += temp * G[ldg * k + j];
						}
					}
				}
			}

		}
		else if (TransF == CblasNoTrans && TransG == CblasTrans) {

			/* form  C := alpha*A*B' + C */
#pragma parallel
			for (i = 0; i < n1; i++) {

				for (j = 0; j < n2; j++) {
					BASE temp = 0.0;
					for (k = 0; k < K; k++) {
						temp += F[ldf * i + k] * G[ldg * j + k];
					}
					C[ldc * i + j] += alpha * temp;
				}
			}

		}
		else if (TransF == CblasTrans && TransG == CblasNoTrans) {

#pragma parallel
			for (k = 0; k < K; k++) {

				for (i = 0; i < n1; i++) {
					const BASE temp = alpha * F[ldf * k + i];
					if (temp != 0.0) {
						for (j = 0; j < n2; j++) {
							C[ldc * i + j] += temp * G[ldg * k + j];
						}
					}
				}
			}

		}
		else if (TransF == CblasTrans && TransG == CblasTrans) {
#pragma parallel
			for (i = 0; i < n1; i++) {

				for (j = 0; j < n2; j++) {
					BASE temp = 0.0;
					for (k = 0; k < K; k++) {
						temp += F[ldf * k + i] * G[ldg * j + k];
					}
					C[ldc * i + j] += alpha * temp;
				}
			}

		}
		else {
			BLAS_ERROR("unrecognized operation");
		}
	}

#undef BASE
}
