/* whitebxf.c
   White box tests
   attempt to exercise paths through each tested function routine
   that might be missed by randomly chosen arguments.
   Float complex version.

   S. L. Moshier
   May, 2004  */

#include <stdio.h>
#include <stdlib.h>
#include "complex.h"

#define TEST_DENORMAL 1

struct oneargument
  {
    char *name;                 /* Name of the function. */
    float complex (*func) (float complex);
    float real_arg;            /* Function argument, assumed exact.  */
    float imag_arg;
    float real_ans;
    float imag_ans;
    float thresh;  /* Error report threshold on absolute error. */
  };


extern float complex csqrtf (float complex);
extern float fabsf (float);

#define E0 0.0f
#define E1 1.0e-8f

struct oneargument test1[] =
{
  {"csqrt", csqrtf, 0.0, 0.0, 0.0, 0.0, E0},

  {"csqrt", csqrtf, 1.0, 0.0, 1.0, 0.0, E0},

  {"csqrt", csqrtf, -1.0, 0.0, 0.0, 1.0, E0},

  {"csqrt", csqrtf, 0.0, 1.0,
   0.7071067811865475244008443621048490392848,
   0.7071067811865475244008443621048490392848, E0},

  {"csqrt", csqrtf, 0.0, -1.0,
    0.7071067811865475244008443621048490392848,
   -0.7071067811865475244008443621048490392848, E0},

  {"csqrt", csqrtf, 1.0, 1.0,
   1.098684113467809966039801195240678378544,
   0.455089860562227341304357757822468569620, E0},

  {"csqrt", csqrtf, 1.0, -1.0,
   1.098684113467809966039801195240678378544,
   -0.455089860562227341304357757822468569620, E0},

  {"csqrt", csqrtf, -1.0, 1.0,
   0.455089860562227341304357757822468569620,
   1.098684113467809966039801195240678378544, E0},

  {"csqrt", csqrtf, -1.0, -1.0,
   0.455089860562227341304357757822468569620,
   -1.098684113467809966039801195240678378544, E0},

  {"csqrt", csqrtf,
   3.4028234663852885981170418348451692544e38,  /* 2^128 * (1 - 2^(-24))  */
   3.4028234663852885981170418348451692544e38,
   2.0267144054983168049787510174924825580748e19,
   8.3949259381432729882118785162080155862810e18, E0},

#if TEST_DENORMAL
  {"csqrt", csqrtf,
   1.4012984643248170709237295832899161312802619e-45, /* 2^-149 */
   1.4012984643248170709237295832899161312802619e-45,
   4.1128054643427787980970034627701752008029081e-23,
   1.7035798027329537503686597356013897095512107e-23, E0},
#endif

  {"cgamma", cgammaf,
   -10.0,
   11.0,
   1.8896581551585941268183277714735737440003044673006562468038633994E-19,
   1.6860356142925055119515080405209322571626488144343712512476545549E-19, E1},

  {"cgamma", cgammaf,
   -12.0,
   1.5,
   -7.8114903240823718256851476555695205687736137502277187005346000794E-11,
   +1.0254664706795994053170795627391336801663920722672529176520225304E-10, E1},

  {"cgamma", cgammaf,
   -12.125,
   1.0,
   3.618023531174192933264798273963264945457019233416627741467959E-10,
   2.330103411747755125605926770018214246478657293246030008323202E-10, E1},

  {"cgamma", cgammaf,
   -13.0009765625,
   1.0,
   -2.327706973865316476259946089957270134102798822213590278096593E-11,
   -3.876430541830749061536622357221061513130756242702103455071466E-11, E1},

  {"cgamma", cgammaf,
   -14.0009765625,
   1.0,
   1.45734797810985630136574334184235619740643821837153827475922E-12,
   2.87277485373030363693475163004744665136382344011173847899039E-12, E1},

  {"cgamma", cgammaf,
   -18.0009765625,
   1.0,
   9.763934864019096775079245715330886189202004561644401182385824E-18,
   4.241841613401247152417685005320096714440705437028257582268460E-17, E1},

  {"cgamma", cgammaf,
   -20.0009765625,
   1.0,
   1.409523854905075647511868230009696147215283693862453650532909E-20,
   +1.133610564563788456521002175301118016190187883294500854171880E-19, E1},

  {"cgamma", cgammaf,
   -20.0009765625,
   0.0,
   -4.196574711899909535839233386935110336330739728057022815759103E-16,
   0.0, E1},

  {"null",   NULL, 0.0, 0.0, 0.0, 0.0, E0},
};


int
main ()
{
  float complex (*fun1) (float complex);
  float complex z, w;
  float er, ei, t;
  int i, errs, tests;

  errs = 0;
  tests = 0;
  i = 0;
  for (;;)
    {
      fun1 = test1[i].func;
      if (fun1 == NULL)
	break;

      z = test1[i].real_arg + test1[i].imag_arg * I;
      w = (*(fun1)) (z);

      er = creal(w) - test1[i].real_ans;
      ei = cimag(w) - test1[i].imag_ans;
      t = test1[i].thresh;
      if ((fabsf(er) > t) || (fabsf(ei) > t))
	{
	  errs += 1;
	  printf ("Line %d: %.9e %9e, s.b. %.9e %.9e\n", i + 1,
		  creal(w), cimag(w), test1[i].real_ans, test1[i].imag_ans);
	}
      i += 1;
      tests += 1;
    }
  printf ("%d errors in %d tests\n", errs, tests);
  exit (0);
}
