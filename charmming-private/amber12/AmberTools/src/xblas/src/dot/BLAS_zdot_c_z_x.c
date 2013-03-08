#include "blas_extended.h"
#include "blas_extended_private.h"
void BLAS_zdot_c_z_x(enum blas_conj_type conj, int n, const void *alpha,
		     const void *x, int incx, const void *beta,
		     const void *y, int incy,
		     void *r, enum blas_prec_type prec)

/*
 * Purpose
 * =======
 * 
 * This routine computes the inner product:
 * 
 *     r <- beta * r + alpha * SUM_{i=0, n-1} x[i] * y[i].
 * 
 * Arguments
 * =========
 *  
 * conj   (input) enum blas_conj_type
 *        When x and y are complex vectors, specifies whether vector
 *        components x[i] are used unconjugated or conjugated. 
 * 
 * n      (input) int
 *        The length of vectors x and y.
 * 
 * alpha  (input) const void*
 * 
 * x      (input) const void*
 *        Array of length n.
 * 
 * incx   (input) int
 *        The stride used to access components x[i].
 *
 * beta   (input) const void*
 *
 * y      (input) const void*
 *        Array of length n.
 *      
 * incy   (input) int
 *        The stride used to access components y[i].
 *
 * r      (input/output) void*
 * 
 * prec   (input) enum blas_prec_type
 *        Specifies the internal precision to be used.
 *        = blas_prec_single: single precision.
 *        = blas_prec_double: double precision.
 *        = blas_prec_extra : anything at least 1.5 times as accurate
 *                            than double, and wider than 80-bits.
 *                            We use double-double in our implementation.
 *
 */
{
  static const char routine_name[] = "BLAS_zdot_c_z_x";

  switch (prec) {
  case blas_prec_single:
  case blas_prec_double:
  case blas_prec_indigenous:
    {
      int i, ix = 0, iy = 0;
      double *r_i = (double *) r;
      const float *x_i = (float *) x;
      const double *y_i = (double *) y;
      double *alpha_i = (double *) alpha;
      double *beta_i = (double *) beta;
      float x_ii[2];
      double y_ii[2];
      double r_v[2];
      double prod[2];
      double sum[2];
      double tmp1[2];
      double tmp2[2];


      /* Test the input parameters. */
      if (n < 0)
	BLAS_error(routine_name, -2, n, NULL);
      else if (incx == 0)
	BLAS_error(routine_name, -5, incx, NULL);
      else if (incy == 0)
	BLAS_error(routine_name, -8, incy, NULL);

      /* Immediate return. */
      if (((beta_i[0] == 1.0 && beta_i[1] == 0.0))
	  && (n == 0 || (alpha_i[0] == 0.0 && alpha_i[1] == 0.0)))
	return;



      r_v[0] = r_i[0];
      r_v[1] = r_i[0 + 1];
      sum[0] = sum[1] = 0.0;
      incx *= 2;
      incy *= 2;
      if (incx < 0)
	ix = (-n + 1) * incx;
      if (incy < 0)
	iy = (-n + 1) * incy;

      if (conj == blas_conj) {
	for (i = 0; i < n; ++i) {
	  x_ii[0] = x_i[ix];
	  x_ii[1] = x_i[ix + 1];
	  y_ii[0] = y_i[iy];
	  y_ii[1] = y_i[iy + 1];
	  x_ii[1] = -x_ii[1];
	  {
	    prod[0] = (double) x_ii[0] * y_ii[0] - (double) x_ii[1] * y_ii[1];
	    prod[1] = (double) x_ii[0] * y_ii[1] + (double) x_ii[1] * y_ii[0];
	  }			/* prod = x[i]*y[i] */
	  sum[0] = sum[0] + prod[0];
	  sum[1] = sum[1] + prod[1];	/* sum = sum+prod */
	  ix += incx;
	  iy += incy;
	}			/* endfor */
      } else {
	/* do not conjugate */

	for (i = 0; i < n; ++i) {
	  x_ii[0] = x_i[ix];
	  x_ii[1] = x_i[ix + 1];
	  y_ii[0] = y_i[iy];
	  y_ii[1] = y_i[iy + 1];

	  {
	    prod[0] = (double) x_ii[0] * y_ii[0] - (double) x_ii[1] * y_ii[1];
	    prod[1] = (double) x_ii[0] * y_ii[1] + (double) x_ii[1] * y_ii[0];
	  }			/* prod = x[i]*y[i] */
	  sum[0] = sum[0] + prod[0];
	  sum[1] = sum[1] + prod[1];	/* sum = sum+prod */
	  ix += incx;
	  iy += incy;
	}			/* endfor */
      }

      {
	tmp1[0] = (double) sum[0] * alpha_i[0] - (double) sum[1] * alpha_i[1];
	tmp1[1] = (double) sum[0] * alpha_i[1] + (double) sum[1] * alpha_i[0];
      }				/* tmp1 = sum*alpha */
      {
	tmp2[0] = (double) r_v[0] * beta_i[0] - (double) r_v[1] * beta_i[1];
	tmp2[1] = (double) r_v[0] * beta_i[1] + (double) r_v[1] * beta_i[0];
      }				/* tmp2 = r*beta */
      tmp1[0] = tmp1[0] + tmp2[0];
      tmp1[1] = tmp1[1] + tmp2[1];	/* tmp1 = tmp1+tmp2 */
      ((double *) r)[0] = tmp1[0];
      ((double *) r)[1] = tmp1[1];	/* r = tmp1 */


    }
    break;
  case blas_prec_extra:
    {
      int i, ix = 0, iy = 0;
      double *r_i = (double *) r;
      const float *x_i = (float *) x;
      const double *y_i = (double *) y;
      double *alpha_i = (double *) alpha;
      double *beta_i = (double *) beta;
      float x_ii[2];
      double y_ii[2];
      double r_v[2];
      double head_prod[2], tail_prod[2];
      double head_sum[2], tail_sum[2];
      double head_tmp1[2], tail_tmp1[2];
      double head_tmp2[2], tail_tmp2[2];
      FPU_FIX_DECL;

      /* Test the input parameters. */
      if (n < 0)
	BLAS_error(routine_name, -2, n, NULL);
      else if (incx == 0)
	BLAS_error(routine_name, -5, incx, NULL);
      else if (incy == 0)
	BLAS_error(routine_name, -8, incy, NULL);

      /* Immediate return. */
      if (((beta_i[0] == 1.0 && beta_i[1] == 0.0))
	  && (n == 0 || (alpha_i[0] == 0.0 && alpha_i[1] == 0.0)))
	return;

      FPU_FIX_START;

      r_v[0] = r_i[0];
      r_v[1] = r_i[0 + 1];
      head_sum[0] = head_sum[1] = tail_sum[0] = tail_sum[1] = 0.0;
      incx *= 2;
      incy *= 2;
      if (incx < 0)
	ix = (-n + 1) * incx;
      if (incy < 0)
	iy = (-n + 1) * incy;

      if (conj == blas_conj) {
	for (i = 0; i < n; ++i) {
	  x_ii[0] = x_i[ix];
	  x_ii[1] = x_i[ix + 1];
	  y_ii[0] = y_i[iy];
	  y_ii[1] = y_i[iy + 1];
	  x_ii[1] = -x_ii[1];
	  {
	    double cd[2];
	    cd[0] = (double) x_ii[0];
	    cd[1] = (double) x_ii[1];
	    {
	      /* Compute complex-extra = complex-double * complex-double. */
	      double head_t1, tail_t1;
	      double head_t2, tail_t2;
	      /* Real part */
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[0] * split;
		a1 = con - y_ii[0];
		a1 = con - a1;
		a2 = y_ii[0] - a1;
		con = cd[0] * split;
		b1 = con - cd[0];
		b1 = con - b1;
		b2 = cd[0] - b1;

		head_t1 = y_ii[0] * cd[0];
		tail_t1 =
		  (((a1 * b1 - head_t1) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[1] * split;
		a1 = con - y_ii[1];
		a1 = con - a1;
		a2 = y_ii[1] - a1;
		con = cd[1] * split;
		b1 = con - cd[1];
		b1 = con - b1;
		b2 = cd[1] - b1;

		head_t2 = y_ii[1] * cd[1];
		tail_t2 =
		  (((a1 * b1 - head_t2) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      head_t2 = -head_t2;
	      tail_t2 = -tail_t2;
	      {
		/* Compute double-double = double-double + double-double. */
		double bv;
		double s1, s2, t1, t2;

		/* Add two hi words. */
		s1 = head_t1 + head_t2;
		bv = s1 - head_t1;
		s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

		/* Add two lo words. */
		t1 = tail_t1 + tail_t2;
		bv = t1 - tail_t1;
		t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

		s2 += t1;

		/* Renormalize (s1, s2)  to  (t1, s2) */
		t1 = s1 + s2;
		s2 = s2 - (t1 - s1);

		t2 += s2;

		/* Renormalize (t1, t2)  */
		head_t1 = t1 + t2;
		tail_t1 = t2 - (head_t1 - t1);
	      }
	      head_prod[0] = head_t1;
	      tail_prod[0] = tail_t1;
	      /* Imaginary part */
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[1] * split;
		a1 = con - y_ii[1];
		a1 = con - a1;
		a2 = y_ii[1] - a1;
		con = cd[0] * split;
		b1 = con - cd[0];
		b1 = con - b1;
		b2 = cd[0] - b1;

		head_t1 = y_ii[1] * cd[0];
		tail_t1 =
		  (((a1 * b1 - head_t1) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[0] * split;
		a1 = con - y_ii[0];
		a1 = con - a1;
		a2 = y_ii[0] - a1;
		con = cd[1] * split;
		b1 = con - cd[1];
		b1 = con - b1;
		b2 = cd[1] - b1;

		head_t2 = y_ii[0] * cd[1];
		tail_t2 =
		  (((a1 * b1 - head_t2) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      {
		/* Compute double-double = double-double + double-double. */
		double bv;
		double s1, s2, t1, t2;

		/* Add two hi words. */
		s1 = head_t1 + head_t2;
		bv = s1 - head_t1;
		s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

		/* Add two lo words. */
		t1 = tail_t1 + tail_t2;
		bv = t1 - tail_t1;
		t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

		s2 += t1;

		/* Renormalize (s1, s2)  to  (t1, s2) */
		t1 = s1 + s2;
		s2 = s2 - (t1 - s1);

		t2 += s2;

		/* Renormalize (t1, t2)  */
		head_t1 = t1 + t2;
		tail_t1 = t2 - (head_t1 - t1);
	      }
	      head_prod[1] = head_t1;
	      tail_prod[1] = tail_t1;
	    }
	  }			/* prod = x[i]*y[i] */
	  {
	    double head_t, tail_t;
	    double head_a, tail_a;
	    double head_b, tail_b;
	    /* Real part */
	    head_a = head_sum[0];
	    tail_a = tail_sum[0];
	    head_b = head_prod[0];
	    tail_b = tail_prod[0];
	    {
	      /* Compute double-double = double-double + double-double. */
	      double bv;
	      double s1, s2, t1, t2;

	      /* Add two hi words. */
	      s1 = head_a + head_b;
	      bv = s1 - head_a;
	      s2 = ((head_b - bv) + (head_a - (s1 - bv)));

	      /* Add two lo words. */
	      t1 = tail_a + tail_b;
	      bv = t1 - tail_a;
	      t2 = ((tail_b - bv) + (tail_a - (t1 - bv)));

	      s2 += t1;

	      /* Renormalize (s1, s2)  to  (t1, s2) */
	      t1 = s1 + s2;
	      s2 = s2 - (t1 - s1);

	      t2 += s2;

	      /* Renormalize (t1, t2)  */
	      head_t = t1 + t2;
	      tail_t = t2 - (head_t - t1);
	    }
	    head_sum[0] = head_t;
	    tail_sum[0] = tail_t;
	    /* Imaginary part */
	    head_a = head_sum[1];
	    tail_a = tail_sum[1];
	    head_b = head_prod[1];
	    tail_b = tail_prod[1];
	    {
	      /* Compute double-double = double-double + double-double. */
	      double bv;
	      double s1, s2, t1, t2;

	      /* Add two hi words. */
	      s1 = head_a + head_b;
	      bv = s1 - head_a;
	      s2 = ((head_b - bv) + (head_a - (s1 - bv)));

	      /* Add two lo words. */
	      t1 = tail_a + tail_b;
	      bv = t1 - tail_a;
	      t2 = ((tail_b - bv) + (tail_a - (t1 - bv)));

	      s2 += t1;

	      /* Renormalize (s1, s2)  to  (t1, s2) */
	      t1 = s1 + s2;
	      s2 = s2 - (t1 - s1);

	      t2 += s2;

	      /* Renormalize (t1, t2)  */
	      head_t = t1 + t2;
	      tail_t = t2 - (head_t - t1);
	    }
	    head_sum[1] = head_t;
	    tail_sum[1] = tail_t;
	  }			/* sum = sum+prod */
	  ix += incx;
	  iy += incy;
	}			/* endfor */
      } else {
	/* do not conjugate */

	for (i = 0; i < n; ++i) {
	  x_ii[0] = x_i[ix];
	  x_ii[1] = x_i[ix + 1];
	  y_ii[0] = y_i[iy];
	  y_ii[1] = y_i[iy + 1];

	  {
	    double cd[2];
	    cd[0] = (double) x_ii[0];
	    cd[1] = (double) x_ii[1];
	    {
	      /* Compute complex-extra = complex-double * complex-double. */
	      double head_t1, tail_t1;
	      double head_t2, tail_t2;
	      /* Real part */
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[0] * split;
		a1 = con - y_ii[0];
		a1 = con - a1;
		a2 = y_ii[0] - a1;
		con = cd[0] * split;
		b1 = con - cd[0];
		b1 = con - b1;
		b2 = cd[0] - b1;

		head_t1 = y_ii[0] * cd[0];
		tail_t1 =
		  (((a1 * b1 - head_t1) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[1] * split;
		a1 = con - y_ii[1];
		a1 = con - a1;
		a2 = y_ii[1] - a1;
		con = cd[1] * split;
		b1 = con - cd[1];
		b1 = con - b1;
		b2 = cd[1] - b1;

		head_t2 = y_ii[1] * cd[1];
		tail_t2 =
		  (((a1 * b1 - head_t2) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      head_t2 = -head_t2;
	      tail_t2 = -tail_t2;
	      {
		/* Compute double-double = double-double + double-double. */
		double bv;
		double s1, s2, t1, t2;

		/* Add two hi words. */
		s1 = head_t1 + head_t2;
		bv = s1 - head_t1;
		s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

		/* Add two lo words. */
		t1 = tail_t1 + tail_t2;
		bv = t1 - tail_t1;
		t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

		s2 += t1;

		/* Renormalize (s1, s2)  to  (t1, s2) */
		t1 = s1 + s2;
		s2 = s2 - (t1 - s1);

		t2 += s2;

		/* Renormalize (t1, t2)  */
		head_t1 = t1 + t2;
		tail_t1 = t2 - (head_t1 - t1);
	      }
	      head_prod[0] = head_t1;
	      tail_prod[0] = tail_t1;
	      /* Imaginary part */
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[1] * split;
		a1 = con - y_ii[1];
		a1 = con - a1;
		a2 = y_ii[1] - a1;
		con = cd[0] * split;
		b1 = con - cd[0];
		b1 = con - b1;
		b2 = cd[0] - b1;

		head_t1 = y_ii[1] * cd[0];
		tail_t1 =
		  (((a1 * b1 - head_t1) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      {
		/* Compute double_double = double * double. */
		double a1, a2, b1, b2, con;

		con = y_ii[0] * split;
		a1 = con - y_ii[0];
		a1 = con - a1;
		a2 = y_ii[0] - a1;
		con = cd[1] * split;
		b1 = con - cd[1];
		b1 = con - b1;
		b2 = cd[1] - b1;

		head_t2 = y_ii[0] * cd[1];
		tail_t2 =
		  (((a1 * b1 - head_t2) + a1 * b2) + a2 * b1) + a2 * b2;
	      }
	      {
		/* Compute double-double = double-double + double-double. */
		double bv;
		double s1, s2, t1, t2;

		/* Add two hi words. */
		s1 = head_t1 + head_t2;
		bv = s1 - head_t1;
		s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

		/* Add two lo words. */
		t1 = tail_t1 + tail_t2;
		bv = t1 - tail_t1;
		t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

		s2 += t1;

		/* Renormalize (s1, s2)  to  (t1, s2) */
		t1 = s1 + s2;
		s2 = s2 - (t1 - s1);

		t2 += s2;

		/* Renormalize (t1, t2)  */
		head_t1 = t1 + t2;
		tail_t1 = t2 - (head_t1 - t1);
	      }
	      head_prod[1] = head_t1;
	      tail_prod[1] = tail_t1;
	    }
	  }			/* prod = x[i]*y[i] */
	  {
	    double head_t, tail_t;
	    double head_a, tail_a;
	    double head_b, tail_b;
	    /* Real part */
	    head_a = head_sum[0];
	    tail_a = tail_sum[0];
	    head_b = head_prod[0];
	    tail_b = tail_prod[0];
	    {
	      /* Compute double-double = double-double + double-double. */
	      double bv;
	      double s1, s2, t1, t2;

	      /* Add two hi words. */
	      s1 = head_a + head_b;
	      bv = s1 - head_a;
	      s2 = ((head_b - bv) + (head_a - (s1 - bv)));

	      /* Add two lo words. */
	      t1 = tail_a + tail_b;
	      bv = t1 - tail_a;
	      t2 = ((tail_b - bv) + (tail_a - (t1 - bv)));

	      s2 += t1;

	      /* Renormalize (s1, s2)  to  (t1, s2) */
	      t1 = s1 + s2;
	      s2 = s2 - (t1 - s1);

	      t2 += s2;

	      /* Renormalize (t1, t2)  */
	      head_t = t1 + t2;
	      tail_t = t2 - (head_t - t1);
	    }
	    head_sum[0] = head_t;
	    tail_sum[0] = tail_t;
	    /* Imaginary part */
	    head_a = head_sum[1];
	    tail_a = tail_sum[1];
	    head_b = head_prod[1];
	    tail_b = tail_prod[1];
	    {
	      /* Compute double-double = double-double + double-double. */
	      double bv;
	      double s1, s2, t1, t2;

	      /* Add two hi words. */
	      s1 = head_a + head_b;
	      bv = s1 - head_a;
	      s2 = ((head_b - bv) + (head_a - (s1 - bv)));

	      /* Add two lo words. */
	      t1 = tail_a + tail_b;
	      bv = t1 - tail_a;
	      t2 = ((tail_b - bv) + (tail_a - (t1 - bv)));

	      s2 += t1;

	      /* Renormalize (s1, s2)  to  (t1, s2) */
	      t1 = s1 + s2;
	      s2 = s2 - (t1 - s1);

	      t2 += s2;

	      /* Renormalize (t1, t2)  */
	      head_t = t1 + t2;
	      tail_t = t2 - (head_t - t1);
	    }
	    head_sum[1] = head_t;
	    tail_sum[1] = tail_t;
	  }			/* sum = sum+prod */
	  ix += incx;
	  iy += incy;
	}			/* endfor */
      }

      {
	/* Compute complex-extra = complex-extra * complex-double. */
	double head_a0, tail_a0;
	double head_a1, tail_a1;
	double head_t1, tail_t1;
	double head_t2, tail_t2;
	head_a0 = head_sum[0];
	tail_a0 = tail_sum[0];
	head_a1 = head_sum[1];
	tail_a1 = tail_sum[1];
	/* real part */
	{
	  /* Compute double-double = double-double * double. */
	  double a11, a21, b1, b2, c11, c21, c2, con, t1, t2;

	  con = head_a0 * split;
	  a11 = con - head_a0;
	  a11 = con - a11;
	  a21 = head_a0 - a11;
	  con = alpha_i[0] * split;
	  b1 = con - alpha_i[0];
	  b1 = con - b1;
	  b2 = alpha_i[0] - b1;

	  c11 = head_a0 * alpha_i[0];
	  c21 = (((a11 * b1 - c11) + a11 * b2) + a21 * b1) + a21 * b2;

	  c2 = tail_a0 * alpha_i[0];
	  t1 = c11 + c2;
	  t2 = (c2 - (t1 - c11)) + c21;

	  head_t1 = t1 + t2;
	  tail_t1 = t2 - (head_t1 - t1);
	}
	{
	  /* Compute double-double = double-double * double. */
	  double a11, a21, b1, b2, c11, c21, c2, con, t1, t2;

	  con = head_a1 * split;
	  a11 = con - head_a1;
	  a11 = con - a11;
	  a21 = head_a1 - a11;
	  con = alpha_i[1] * split;
	  b1 = con - alpha_i[1];
	  b1 = con - b1;
	  b2 = alpha_i[1] - b1;

	  c11 = head_a1 * alpha_i[1];
	  c21 = (((a11 * b1 - c11) + a11 * b2) + a21 * b1) + a21 * b2;

	  c2 = tail_a1 * alpha_i[1];
	  t1 = c11 + c2;
	  t2 = (c2 - (t1 - c11)) + c21;

	  head_t2 = t1 + t2;
	  tail_t2 = t2 - (head_t2 - t1);
	}
	head_t2 = -head_t2;
	tail_t2 = -tail_t2;
	{
	  /* Compute double-double = double-double + double-double. */
	  double bv;
	  double s1, s2, t1, t2;

	  /* Add two hi words. */
	  s1 = head_t1 + head_t2;
	  bv = s1 - head_t1;
	  s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

	  /* Add two lo words. */
	  t1 = tail_t1 + tail_t2;
	  bv = t1 - tail_t1;
	  t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

	  s2 += t1;

	  /* Renormalize (s1, s2)  to  (t1, s2) */
	  t1 = s1 + s2;
	  s2 = s2 - (t1 - s1);

	  t2 += s2;

	  /* Renormalize (t1, t2)  */
	  head_t1 = t1 + t2;
	  tail_t1 = t2 - (head_t1 - t1);
	}
	head_tmp1[0] = head_t1;
	tail_tmp1[0] = tail_t1;
	/* imaginary part */
	{
	  /* Compute double-double = double-double * double. */
	  double a11, a21, b1, b2, c11, c21, c2, con, t1, t2;

	  con = head_a1 * split;
	  a11 = con - head_a1;
	  a11 = con - a11;
	  a21 = head_a1 - a11;
	  con = alpha_i[0] * split;
	  b1 = con - alpha_i[0];
	  b1 = con - b1;
	  b2 = alpha_i[0] - b1;

	  c11 = head_a1 * alpha_i[0];
	  c21 = (((a11 * b1 - c11) + a11 * b2) + a21 * b1) + a21 * b2;

	  c2 = tail_a1 * alpha_i[0];
	  t1 = c11 + c2;
	  t2 = (c2 - (t1 - c11)) + c21;

	  head_t1 = t1 + t2;
	  tail_t1 = t2 - (head_t1 - t1);
	}
	{
	  /* Compute double-double = double-double * double. */
	  double a11, a21, b1, b2, c11, c21, c2, con, t1, t2;

	  con = head_a0 * split;
	  a11 = con - head_a0;
	  a11 = con - a11;
	  a21 = head_a0 - a11;
	  con = alpha_i[1] * split;
	  b1 = con - alpha_i[1];
	  b1 = con - b1;
	  b2 = alpha_i[1] - b1;

	  c11 = head_a0 * alpha_i[1];
	  c21 = (((a11 * b1 - c11) + a11 * b2) + a21 * b1) + a21 * b2;

	  c2 = tail_a0 * alpha_i[1];
	  t1 = c11 + c2;
	  t2 = (c2 - (t1 - c11)) + c21;

	  head_t2 = t1 + t2;
	  tail_t2 = t2 - (head_t2 - t1);
	}
	{
	  /* Compute double-double = double-double + double-double. */
	  double bv;
	  double s1, s2, t1, t2;

	  /* Add two hi words. */
	  s1 = head_t1 + head_t2;
	  bv = s1 - head_t1;
	  s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

	  /* Add two lo words. */
	  t1 = tail_t1 + tail_t2;
	  bv = t1 - tail_t1;
	  t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

	  s2 += t1;

	  /* Renormalize (s1, s2)  to  (t1, s2) */
	  t1 = s1 + s2;
	  s2 = s2 - (t1 - s1);

	  t2 += s2;

	  /* Renormalize (t1, t2)  */
	  head_t1 = t1 + t2;
	  tail_t1 = t2 - (head_t1 - t1);
	}
	head_tmp1[1] = head_t1;
	tail_tmp1[1] = tail_t1;
      }
      /* tmp1 = sum*alpha */
      {
	/* Compute complex-extra = complex-double * complex-double. */
	double head_t1, tail_t1;
	double head_t2, tail_t2;
	/* Real part */
	{
	  /* Compute double_double = double * double. */
	  double a1, a2, b1, b2, con;

	  con = r_v[0] * split;
	  a1 = con - r_v[0];
	  a1 = con - a1;
	  a2 = r_v[0] - a1;
	  con = beta_i[0] * split;
	  b1 = con - beta_i[0];
	  b1 = con - b1;
	  b2 = beta_i[0] - b1;

	  head_t1 = r_v[0] * beta_i[0];
	  tail_t1 = (((a1 * b1 - head_t1) + a1 * b2) + a2 * b1) + a2 * b2;
	}
	{
	  /* Compute double_double = double * double. */
	  double a1, a2, b1, b2, con;

	  con = r_v[1] * split;
	  a1 = con - r_v[1];
	  a1 = con - a1;
	  a2 = r_v[1] - a1;
	  con = beta_i[1] * split;
	  b1 = con - beta_i[1];
	  b1 = con - b1;
	  b2 = beta_i[1] - b1;

	  head_t2 = r_v[1] * beta_i[1];
	  tail_t2 = (((a1 * b1 - head_t2) + a1 * b2) + a2 * b1) + a2 * b2;
	}
	head_t2 = -head_t2;
	tail_t2 = -tail_t2;
	{
	  /* Compute double-double = double-double + double-double. */
	  double bv;
	  double s1, s2, t1, t2;

	  /* Add two hi words. */
	  s1 = head_t1 + head_t2;
	  bv = s1 - head_t1;
	  s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

	  /* Add two lo words. */
	  t1 = tail_t1 + tail_t2;
	  bv = t1 - tail_t1;
	  t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

	  s2 += t1;

	  /* Renormalize (s1, s2)  to  (t1, s2) */
	  t1 = s1 + s2;
	  s2 = s2 - (t1 - s1);

	  t2 += s2;

	  /* Renormalize (t1, t2)  */
	  head_t1 = t1 + t2;
	  tail_t1 = t2 - (head_t1 - t1);
	}
	head_tmp2[0] = head_t1;
	tail_tmp2[0] = tail_t1;
	/* Imaginary part */
	{
	  /* Compute double_double = double * double. */
	  double a1, a2, b1, b2, con;

	  con = r_v[1] * split;
	  a1 = con - r_v[1];
	  a1 = con - a1;
	  a2 = r_v[1] - a1;
	  con = beta_i[0] * split;
	  b1 = con - beta_i[0];
	  b1 = con - b1;
	  b2 = beta_i[0] - b1;

	  head_t1 = r_v[1] * beta_i[0];
	  tail_t1 = (((a1 * b1 - head_t1) + a1 * b2) + a2 * b1) + a2 * b2;
	}
	{
	  /* Compute double_double = double * double. */
	  double a1, a2, b1, b2, con;

	  con = r_v[0] * split;
	  a1 = con - r_v[0];
	  a1 = con - a1;
	  a2 = r_v[0] - a1;
	  con = beta_i[1] * split;
	  b1 = con - beta_i[1];
	  b1 = con - b1;
	  b2 = beta_i[1] - b1;

	  head_t2 = r_v[0] * beta_i[1];
	  tail_t2 = (((a1 * b1 - head_t2) + a1 * b2) + a2 * b1) + a2 * b2;
	}
	{
	  /* Compute double-double = double-double + double-double. */
	  double bv;
	  double s1, s2, t1, t2;

	  /* Add two hi words. */
	  s1 = head_t1 + head_t2;
	  bv = s1 - head_t1;
	  s2 = ((head_t2 - bv) + (head_t1 - (s1 - bv)));

	  /* Add two lo words. */
	  t1 = tail_t1 + tail_t2;
	  bv = t1 - tail_t1;
	  t2 = ((tail_t2 - bv) + (tail_t1 - (t1 - bv)));

	  s2 += t1;

	  /* Renormalize (s1, s2)  to  (t1, s2) */
	  t1 = s1 + s2;
	  s2 = s2 - (t1 - s1);

	  t2 += s2;

	  /* Renormalize (t1, t2)  */
	  head_t1 = t1 + t2;
	  tail_t1 = t2 - (head_t1 - t1);
	}
	head_tmp2[1] = head_t1;
	tail_tmp2[1] = tail_t1;
      }				/* tmp2 = r*beta */
      {
	double head_t, tail_t;
	double head_a, tail_a;
	double head_b, tail_b;
	/* Real part */
	head_a = head_tmp1[0];
	tail_a = tail_tmp1[0];
	head_b = head_tmp2[0];
	tail_b = tail_tmp2[0];
	{
	  /* Compute double-double = double-double + double-double. */
	  double bv;
	  double s1, s2, t1, t2;

	  /* Add two hi words. */
	  s1 = head_a + head_b;
	  bv = s1 - head_a;
	  s2 = ((head_b - bv) + (head_a - (s1 - bv)));

	  /* Add two lo words. */
	  t1 = tail_a + tail_b;
	  bv = t1 - tail_a;
	  t2 = ((tail_b - bv) + (tail_a - (t1 - bv)));

	  s2 += t1;

	  /* Renormalize (s1, s2)  to  (t1, s2) */
	  t1 = s1 + s2;
	  s2 = s2 - (t1 - s1);

	  t2 += s2;

	  /* Renormalize (t1, t2)  */
	  head_t = t1 + t2;
	  tail_t = t2 - (head_t - t1);
	}
	head_tmp1[0] = head_t;
	tail_tmp1[0] = tail_t;
	/* Imaginary part */
	head_a = head_tmp1[1];
	tail_a = tail_tmp1[1];
	head_b = head_tmp2[1];
	tail_b = tail_tmp2[1];
	{
	  /* Compute double-double = double-double + double-double. */
	  double bv;
	  double s1, s2, t1, t2;

	  /* Add two hi words. */
	  s1 = head_a + head_b;
	  bv = s1 - head_a;
	  s2 = ((head_b - bv) + (head_a - (s1 - bv)));

	  /* Add two lo words. */
	  t1 = tail_a + tail_b;
	  bv = t1 - tail_a;
	  t2 = ((tail_b - bv) + (tail_a - (t1 - bv)));

	  s2 += t1;

	  /* Renormalize (s1, s2)  to  (t1, s2) */
	  t1 = s1 + s2;
	  s2 = s2 - (t1 - s1);

	  t2 += s2;

	  /* Renormalize (t1, t2)  */
	  head_t = t1 + t2;
	  tail_t = t2 - (head_t - t1);
	}
	head_tmp1[1] = head_t;
	tail_tmp1[1] = tail_t;
      }				/* tmp1 = tmp1+tmp2 */
      ((double *) r)[0] = head_tmp1[0];
      ((double *) r)[1] = head_tmp1[1];	/* r = tmp1 */

      FPU_FIX_STOP;
    }
    break;
  }
}
