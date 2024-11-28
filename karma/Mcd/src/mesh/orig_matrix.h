typedef double Float;
typedef Float Matrix3x3[3][3];

void      matmult(int ma, int nab, int nb, Float *a, Float *b, Float *c);

void      randmat(int m, int n, Float *a);

void      printmat(int m, int n, Float *a);

double    dotprod(int n, Float *a, Float *b);

void      matmult3x3(Matrix3x3 a, Matrix3x3 b, Matrix3x3 c);

Float     square_norm3x3(const Matrix3x3 a);

void      transpose3x3(Matrix3x3 at, const Matrix3x3 a);

void      cholesky_factor3x3(Matrix3x3 a);

int       test_cholesky_factor3x3(Matrix3x3 a);

void      invert_lower3x3(Matrix3x3 l);

int       test_invert_lower3x3(Matrix3x3 l);

void      multiply_l_lt3x3(Matrix3x3 l);

void      MatrixInvert3x3(Matrix3x3 a);

int       check_3x3_inverter(Matrix3x3 a, Float tol);

void      make3x3pd(Matrix3x3 a, Float low, Float high);

void      make3x3pd(Matrix3x3 a);

void      sym_schur2(Matrix3x3 a, int p, int q, Float *s, Float *c);

void      test_SYM_SCHUR2();

void      jacobi_update_cols(Matrix3x3 a, int i, int j, Float c, Float s);

void      jacobi_update_rows(Matrix3x3 a, int i, int j, Float c, Float s);

int       jacobi3x3(Matrix3x3 a, Matrix3x3 r);

int       test_jacobi3x3();

void      test_jacobi_sample(int sample);

void      sample_3x3inverter();
