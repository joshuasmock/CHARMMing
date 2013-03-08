dnl
dnl
define(`SYMV2_NAME', `ifelse(
  `$2&&$3', `$1&&$1', `BLAS_$1symv2$4', 
  `BLAS_$1symv2_$2_$3$4')')dnl
dnl
dnl
define(`SYMV2_PARAMS', 
  `enum blas_order_type order, enum blas_uplo_type uplo, int n, $1_scalar alpha,
   const $2_array a, int lda, const $3_array x_head, const $3_array x_tail,
   int incx, $1_scalar beta, $1_array y,
   int incy`'ifelse(`$4', `_x', `, enum blas_prec_type prec')')dnl
dnl
dnl
define(`SYMV2_HEAD', 
  `void SYMV2_NAME($1, $2, $3, $4)(SYMV2_PARAMS($1, $2, $3, $4))')dnl
dnl
dnl
define(`SYMV2_ARGS', 
`if_blas(``s, s, s', `d, d, d', 
 `c, c, c', `z, z, z',')dnl
`d, d, s', `d, s, d', 
 `d, s, s', `z, z, c', 
 `z, c, z', `z, c, c', 
 `c, c, s', `c, s, c', 
 `c, s, s', `z, z, d', 
 `z, d, z', `z, d, d', 
 `s, s, s, _x', `d, d, d, _x', 
 `c, c, c, _x', `z, z, z, _x', 
 `d, d, s, _x', `d, s, d, _x', 
 `d, s, s, _x', `z, z, c, _x', 
 `z, c, z, _x', `z, c, c, _x', 
 `c, c, s, _x', `c, s, c, _x', 
 `c, s, s, _x', `z, z, d, _x', 
 `z, d, z, _x', `z, d, d, _x'')dnl
dnl
dnl
