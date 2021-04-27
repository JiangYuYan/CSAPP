/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, row, col, x0, x1, x2, x3, x4, x5, x6, x7;
  switch (M) {
    case 32:
      for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
          if (i == j) {
            for (row = i*8; row < (i+1)*8; ++row) {
              col = j*8;
              x0 = A[row][col];
              x1 = A[row][col+1];
              x2 = A[row][col+2];
              x3 = A[row][col+3];
              x4 = A[row][col+4];
              x5 = A[row][col+5];
              x6 = A[row][col+6];
              x7 = A[row][col+7];
              B[row][col] = x0;
              B[row][col+1] = x1;
              B[row][col+2] = x2;
              B[row][col+3] = x3;
              B[row][col+4] = x4;
              B[row][col+5] = x5;
              B[row][col+6] = x6;
              B[row][col+7] = x7;
            }
            for (row = 0; row < 8; ++row) {
              for (col = row+1; col < 8; ++col) {
                x0 = B[i*8+row][j*8+col];
                B[i*8+row][j*8+col] = B[j*8+col][i*8+row];
                B[j*8+col][i*8+row] = x0;
              }
            }
          } else {
            for (row = i*8; row < (i+1)*8; ++row) {
              for (col = j*8; col < (j+1)*8; ++col) {
                B[row][col] = A[col][row];
              }
            }
          }
          
        }
      }
      // for (i = 0; i < 4; ++i) {
      //   for (j = 0; j < 4; ++j) {
      //     for (row = 0; row < 8; ++row) {
      //       x0 = A[j*8][i*8+row];
      //       x1 = A[j*8+1][i*8+row];
      //       x2 = A[j*8+2][i*8+row];
      //       x3 = A[j*8+3][i*8+row];
      //       x4 = A[j*8+4][i*8+row];
      //       x5 = A[j*8+5][i*8+row];
      //       x6 = A[j*8+6][i*8+row];
      //       x7 = A[j*8+7][i*8+row];
      //       B[i*8+row][j*8] = x0;
      //       B[i*8+row][j*8+1] = x1;
      //       B[i*8+row][j*8+2] = x2;
      //       B[i*8+row][j*8+3] = x3;
      //       B[i*8+row][j*8+4] = x4;
      //       B[i*8+row][j*8+5] = x5;
      //       B[i*8+row][j*8+6] = x6;
      //       B[i*8+row][j*8+7] = x7;
      //     }
      //   }
      // }
      break;
    case 64:
      for (i = 0; i < 8; ++i) {
        for (j = 0; j < 8; ++j) {
          if (i*8+j >= 61) continue;
          x0 = (i*8 + j + 1) / 8; // [id+1]row
          x1 = (j + 1) % 8; // [id+1]col
          x2 = (i*8 + j + 2) / 8; // [id+2]row
          x3 = (j + 2) % 8; // [id+2]col
          if (i == j) { // use cell[id+1] and cell[id+2]
            for (row = 0; row < 4; ++row) {
              for (col = 0; col < 8; ++col) {
                B[x0*8+row][x1*8+col] = A[j*8+row][i*8+col];
              }
            }
            for (row = 4; row < 8; ++row) {
              for (col = 0; col < 8; ++col) {
                B[x2*8+row-4][x3*8+col] = A[j*8+row][i*8+col];
              }
            }
            for (row = 0; row < 4; ++row) {
              for (col = 0; col < 4; ++col) {
                B[i*8+row][j*8+col] = B[x0*8+col][x1*8+row];
              }
              for (col = 4; col < 8; ++col) {
                B[i*8+row][j*8+col] = B[x2*8+col-4][x3*8+row];
              }
            }
            for (row = 4; row < 8; ++row) {
              for (col = 0; col < 4; ++col) {
                B[i*8+row][j*8+col] = B[x0*8+col][x1*8+row];
              }
              for (col = 4; col < 8; ++col) {
                B[i*8+row][j*8+col] = B[x2*8+col-4][x3*8+row];
              }
            }
          } else { // just use cell[id] when i != j
            for (row = 0; row < 4; ++row) {
              for (col = 0; col < 4; ++col) {
                B[i*8+row][j*8+col] = A[j*8+col][i*8+row];
              }
            } // A left-up reverse to B left-up
            for (row = 0; row < 4; ++row) {
              for (col = 4; col < 8; ++col) {
                B[i*8+row][j*8+col] = A[j*8+col-4][i*8+row+4];
              }
            } // A right-up reverse to B right-up 
            for (row = 4; row < 8; ++row) {
              // save B right-up
              x0 = B[i*8+row-4][j*8+4];
              x1 = B[i*8+row-4][j*8+5];
              x2 = B[i*8+row-4][j*8+6];
              x3 = B[i*8+row-4][j*8+7];
                
              // A left-bottom reverse to B right-up
              B[i*8+row-4][j*8+4] = A[j*8+4][i*8+row-4];
              B[i*8+row-4][j*8+5] = A[j*8+5][i*8+row-4];
              B[i*8+row-4][j*8+6] = A[j*8+6][i*8+row-4];
              B[i*8+row-4][j*8+7] = A[j*8+7][i*8+row-4]; 

              // saved B right-up to B left-bottom
              B[i*8+row][j*8] = x0;
              B[i*8+row][j*8+1] = x1;
              B[i*8+row][j*8+2] = x2;
              B[i*8+row][j*8+3] = x3;
            } // A left-bottom reverse to B right-up; saved B right-up move to B left-bottom
            for (row = 4; row < 8; ++row) {
              for (col = 4; col < 8; ++col) {
                B[i*8+row][j*8+col] = A[j*8+col][i*8+row];
              }
            } // A right-bottom reverse to B right-bottom;
            
          }
          
        }
      }
      // deal with cell 63
      i = 7;
      j = 7;
      x0 = 7; // [id+1]row
      x1 = 5; // [id+1]col
      x2 = 7; // [id+2]row
      x3 = 6; // [id+2]col
      for (row = 0; row < 4; ++row) {
        for (col = 0; col < 8; ++col) {
          B[x0*8+row][x1*8+col] = A[j*8+row][i*8+col];
        }
      }
      for (row = 4; row < 8; ++row) {
        for (col = 0; col < 8; ++col) {
          B[x2*8+row-4][x3*8+col] = A[j*8+row][i*8+col];
        }
      }
      for (row = 0; row < 4; ++row) {
        for (col = 0; col < 4; ++col) {
          B[i*8+row][j*8+col] = B[x0*8+col][x1*8+row];
        }
        for (col = 4; col < 8; ++col) {
          B[i*8+row][j*8+col] = B[x2*8+col-4][x3*8+row];
        }
      }
      for (row = 4; row < 8; ++row) {
        for (col = 0; col < 4; ++col) {
          B[i*8+row][j*8+col] = B[x0*8+col][x1*8+row];
        }
        for (col = 4; col < 8; ++col) {
          B[i*8+row][j*8+col] = B[x2*8+col-4][x3*8+row];
        }
      }
      // deal with cell 61 and 62
      i = 7;
      for (j = 5; j < 7; ++j) {
        for (row = 0; row < 4; ++row) {
          for (col = 0; col < 4; ++col) {
            B[i*8+row][j*8+col] = A[j*8+col][i*8+row];
          }
        } // A left-up reverse to B left-up
        for (row = 0; row < 4; ++row) {
          for (col = 4; col < 8; ++col) {
            B[i*8+row][j*8+col] = A[j*8+col-4][i*8+row+4];
          }
        } // A right-up reverse to B right-up 
        for (row = 4; row < 8; ++row) {
          // save B right-up
          x0 = B[i*8+row-4][j*8+4];
          x1 = B[i*8+row-4][j*8+5];
          x2 = B[i*8+row-4][j*8+6];
          x3 = B[i*8+row-4][j*8+7];
            
          // A left-bottom reverse to B right-up
          B[i*8+row-4][j*8+4] = A[j*8+4][i*8+row-4];
          B[i*8+row-4][j*8+5] = A[j*8+5][i*8+row-4];
          B[i*8+row-4][j*8+6] = A[j*8+6][i*8+row-4];
          B[i*8+row-4][j*8+7] = A[j*8+7][i*8+row-4]; 

          // saved B right-up to B left-bottom
          B[i*8+row][j*8] = x0;
          B[i*8+row][j*8+1] = x1;
          B[i*8+row][j*8+2] = x2;
          B[i*8+row][j*8+3] = x3;
        } // A left-bottom reverse to B right-up; saved B right-up move to B left-bottom
        for (row = 4; row < 8; ++row) {
          for (col = 4; col < 8; ++col) {
            B[i*8+row][j*8+col] = A[j*8+col][i*8+row];
          }
        } // A right-bottom reverse to B right-bottom;
      }
      break;
    case 61:
      // bottom row
      i = 7;
      for (col = 0; col < 64; ++col) {
        for (row = i*8; row < 61; ++row) {
          B[row][col] = A[col][row];
        }
      }
      // for (j = 0; j < 8; ++j) {
      //   for (row = 0; row < 5; ++row) {
      //     x0 = A[j*8][i*8+row];
      //     x1 = A[j*8+1][i*8+row];
      //     x2 = A[j*8+2][i*8+row];
      //     x3 = A[j*8+3][i*8+row];
      //     x4 = A[j*8+4][i*8+row];
      //     x5 = A[j*8+5][i*8+row];
      //     x6 = A[j*8+6][i*8+row];
      //     x7 = A[j*8+7][i*8+row];
      //     B[i*8+row][j*8] = x0;
      //     B[i*8+row][j*8+1] = x1;
      //     B[i*8+row][j*8+2] = x2;
      //     B[i*8+row][j*8+3] = x3;
      //     B[i*8+row][j*8+4] = x4;
      //     B[i*8+row][j*8+5] = x5;
      //     B[i*8+row][j*8+6] = x6;
      //     B[i*8+row][j*8+7] = x7;
      //   }
      // }
      // right col
      j = 8;
      for (row = 0; row < 56; ++row) {
        for (col = 8*j; col < 67; ++col) {
          B[row][col] = A[col][row];
        }
      }
      // right-bottom
      i = 7; j = 8;
      for (row = i*8; row < 61; ++row) {
        for (col = j*8; col < 67; ++col) {
          B[row][col] = A[col][row];
        }
      }
      // for (row = 0; row < 5; ++row) {
      //   x0 = A[j*8][i*8+row];
      //   x1 = A[j*8+1][i*8+row];
      //   x2 = A[j*8+2][i*8+row];
      //   B[i*8+row][j*8] = x0;
      //   B[i*8+row][j*8+1] = x1;
      //   B[i*8+row][j*8+2] = x2;
      // }
      // others
      for (i = 0; i < 7; ++i) {
        for (j = 0; j < 8; ++j) {
          for (col = 0; col < 8; ++col) {
            x0 = A[j*8+col][i*8];
            x1 = A[j*8+col][i*8+1];
            x2 = A[j*8+col][i*8+2];
            x3 = A[j*8+col][i*8+3];
            x4 = A[j*8+col][i*8+4];
            x5 = A[j*8+col][i*8+5];
            x6 = A[j*8+col][i*8+6];
            x7 = A[j*8+col][i*8+7];
            B[i*8][j*8+col] = x0;
            B[i*8+1][j*8+col] = x1;
            B[i*8+2][j*8+col] = x2;
            B[i*8+3][j*8+col] = x3;
            B[i*8+4][j*8+col] = x4;
            B[i*8+5][j*8+col] = x5;
            B[i*8+6][j*8+col] = x6;
            B[i*8+7][j*8+col] = x7;
          }
        }
      }
  } 
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

