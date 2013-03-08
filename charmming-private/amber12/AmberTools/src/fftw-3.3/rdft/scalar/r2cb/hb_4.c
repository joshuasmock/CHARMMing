/*
 * Copyright (c) 2003, 2007-11 Matteo Frigo
 * Copyright (c) 2003, 2007-11 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Wed Jul 27 06:18:36 EDT 2011 */

#include "codelet-rdft.h"

#ifdef HAVE_FMA

/* Generated by: ../../../genfft/gen_hc2hc.native -fma -reorder-insns -schedule-for-pipeline -compact -variables 4 -pipeline-latency 4 -sign 1 -n 4 -dif -name hb_4 -include hb.h */

/*
 * This function contains 22 FP additions, 12 FP multiplications,
 * (or, 16 additions, 6 multiplications, 6 fused multiply/add),
 * 27 stack variables, 0 constants, and 16 memory accesses
 */
#include "hb.h"

static void hb_4(R *cr, R *ci, const R *W, stride rs, INT mb, INT me, INT ms)
{
     {
	  INT m;
	  for (m = mb, W = W + ((mb - 1) * 6); m < me; m = m + 1, cr = cr + ms, ci = ci - ms, W = W + 6, MAKE_VOLATILE_STRIDE(rs)) {
	       E T8, Th, Ta, T7, Ti, T9;
	       {
		    E Td, Tg, T3, T6, Tu, Tm, Tx, Tr;
		    {
			 E Tq, Tl, T4, T5, Tk, Tp;
			 {
			      E Tb, Tc, Te, Tf, T1, T2;
			      Tb = ci[WS(rs, 3)];
			      Tc = cr[WS(rs, 2)];
			      Te = ci[WS(rs, 2)];
			      Tf = cr[WS(rs, 3)];
			      T1 = cr[0];
			      Tq = Tb + Tc;
			      Td = Tb - Tc;
			      T2 = ci[WS(rs, 1)];
			      Tl = Te + Tf;
			      Tg = Te - Tf;
			      T4 = cr[WS(rs, 1)];
			      T5 = ci[0];
			      T3 = T1 + T2;
			      Tk = T1 - T2;
			 }
			 Tp = T4 - T5;
			 T6 = T4 + T5;
			 Tu = Tk + Tl;
			 Tm = Tk - Tl;
			 Tx = Tq - Tp;
			 Tr = Tp + Tq;
			 T8 = T3 - T6;
		    }
		    cr[0] = T3 + T6;
		    {
			 E Tj, To, Tw, Tv;
			 Tj = W[0];
			 ci[0] = Td + Tg;
			 To = W[1];
			 {
			      E Tt, Ts, Tn, Ty;
			      Tt = W[4];
			      Ts = Tj * Tr;
			      Tn = Tj * Tm;
			      Tw = W[5];
			      Ty = Tt * Tx;
			      Tv = Tt * Tu;
			      ci[WS(rs, 1)] = FMA(To, Tm, Ts);
			      cr[WS(rs, 1)] = FNMS(To, Tr, Tn);
			      ci[WS(rs, 3)] = FMA(Tw, Tu, Ty);
			 }
			 cr[WS(rs, 3)] = FNMS(Tw, Tx, Tv);
			 Th = Td - Tg;
			 Ta = W[3];
			 T7 = W[2];
		    }
	       }
	       Ti = Ta * T8;
	       T9 = T7 * T8;
	       ci[WS(rs, 2)] = FMA(T7, Th, Ti);
	       cr[WS(rs, 2)] = FNMS(Ta, Th, T9);
	  }
     }
}

static const tw_instr twinstr[] = {
     {TW_FULL, 1, 4},
     {TW_NEXT, 1, 0}
};

static const hc2hc_desc desc = { 4, "hb_4", twinstr, &GENUS, {16, 6, 6, 0} };

void X(codelet_hb_4) (planner *p) {
     X(khc2hc_register) (p, hb_4, &desc);
}
#else				/* HAVE_FMA */

/* Generated by: ../../../genfft/gen_hc2hc.native -compact -variables 4 -pipeline-latency 4 -sign 1 -n 4 -dif -name hb_4 -include hb.h */

/*
 * This function contains 22 FP additions, 12 FP multiplications,
 * (or, 16 additions, 6 multiplications, 6 fused multiply/add),
 * 13 stack variables, 0 constants, and 16 memory accesses
 */
#include "hb.h"

static void hb_4(R *cr, R *ci, const R *W, stride rs, INT mb, INT me, INT ms)
{
     {
	  INT m;
	  for (m = mb, W = W + ((mb - 1) * 6); m < me; m = m + 1, cr = cr + ms, ci = ci - ms, W = W + 6, MAKE_VOLATILE_STRIDE(rs)) {
	       E T3, Ti, T6, Tm, Tc, Tn, Tf, Tj;
	       {
		    E T1, T2, T4, T5;
		    T1 = cr[0];
		    T2 = ci[WS(rs, 1)];
		    T3 = T1 + T2;
		    Ti = T1 - T2;
		    T4 = cr[WS(rs, 1)];
		    T5 = ci[0];
		    T6 = T4 + T5;
		    Tm = T4 - T5;
	       }
	       {
		    E Ta, Tb, Td, Te;
		    Ta = ci[WS(rs, 3)];
		    Tb = cr[WS(rs, 2)];
		    Tc = Ta - Tb;
		    Tn = Ta + Tb;
		    Td = ci[WS(rs, 2)];
		    Te = cr[WS(rs, 3)];
		    Tf = Td - Te;
		    Tj = Td + Te;
	       }
	       cr[0] = T3 + T6;
	       ci[0] = Tc + Tf;
	       {
		    E T8, Tg, T7, T9;
		    T8 = T3 - T6;
		    Tg = Tc - Tf;
		    T7 = W[2];
		    T9 = W[3];
		    cr[WS(rs, 2)] = FNMS(T9, Tg, T7 * T8);
		    ci[WS(rs, 2)] = FMA(T9, T8, T7 * Tg);
	       }
	       {
		    E Tk, To, Th, Tl;
		    Tk = Ti - Tj;
		    To = Tm + Tn;
		    Th = W[0];
		    Tl = W[1];
		    cr[WS(rs, 1)] = FNMS(Tl, To, Th * Tk);
		    ci[WS(rs, 1)] = FMA(Th, To, Tl * Tk);
	       }
	       {
		    E Tq, Ts, Tp, Tr;
		    Tq = Ti + Tj;
		    Ts = Tn - Tm;
		    Tp = W[4];
		    Tr = W[5];
		    cr[WS(rs, 3)] = FNMS(Tr, Ts, Tp * Tq);
		    ci[WS(rs, 3)] = FMA(Tp, Ts, Tr * Tq);
	       }
	  }
     }
}

static const tw_instr twinstr[] = {
     {TW_FULL, 1, 4},
     {TW_NEXT, 1, 0}
};

static const hc2hc_desc desc = { 4, "hb_4", twinstr, &GENUS, {16, 6, 6, 0} };

void X(codelet_hb_4) (planner *p) {
     X(khc2hc_register) (p, hb_4, &desc);
}
#endif				/* HAVE_FMA */
