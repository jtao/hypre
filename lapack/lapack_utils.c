/*BHEADER**********************************************************************
 * Copyright (c) 2006   The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by the HYPRE team. UCRL-CODE-222953.
 * All rights reserved.
 *
 * This file is part of HYPRE (see http://www.llnl.gov/CASC/hypre/).
 * Please see the COPYRIGHT_and_LICENSE file for the copyright notice, 
 * disclaimer, contact information and the GNU Lesser General Public License.
 *
 * HYPRE is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License (as published by the Free Software
 * Foundation) version 2.1 dated February 1999.
 *
 * HYPRE is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the terms and conditions of the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Revision$
 ***********************************************************************EHEADER*/



#include "f2c.h"
#include "hypre_lapack.h"

#define log10e 0.43429448190325182765

#ifdef KR_headers
double log();
double d_lg10(x) doublereal *x;
#else
/*
#undef abs
#include "math.h"
*/
double log(double); /* declaration added 2/17/00 */
double d_lg10(doublereal *x)
#endif
{
return( log10e * log(*x) );
}
#include "f2c.h"

#ifdef KR_headers
double d_sign(a,b) doublereal *a, *b;
#else
double d_sign(doublereal *a, doublereal *b)
#endif
{
double x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}

#include "f2c.h"

#ifdef KR_headers
double pow_di(ap, bp) doublereal *ap; integer *bp;
#else
double pow_di(doublereal *ap, integer *bp)
#endif
{
double pow, x;
integer n;

pow = 1;
x = *ap;
n = *bp;

if(n != 0)
	{
	if(n < 0)
		{
		n = -n;
		x = 1/x;
		}
	for( ; ; )
		{
		if(n & 01)
			pow *= x;
		if(n >>= 1)
			x *= x;
		else
			break;
		}
	}
return(pow);
}
#include "f2c.h"

#ifdef KR_headers
double pow();
double pow_dd(ap, bp) doublereal *ap, *bp;
#else
#undef abs
#include "math.h"
double pow_dd(doublereal *ap, doublereal *bp)
#endif
{
return(pow(*ap, *bp) );
}

#include "f2c.h"

#ifdef KR_headers
int s_cat(lp, rpp, rnp, np, ll) char *lp, *rpp[]; ftnlen rnp[], *np, ll;
#else
int s_cat(char *lp, char *rpp[], ftnlen rnp[], ftnlen *np, ftnlen ll)
#endif
{
ftnlen i, n, nc;
char *f__rp;

n = (int)*np;
for(i = 0 ; i < n ; ++i)
	{
	nc = ll;
	if(rnp[i] < nc)
		nc = rnp[i];
	ll -= nc;
	f__rp = rpp[i];
	while(--nc >= 0)
		*lp++ = *f__rp++;
	}
while(--ll >= 0)
	*lp++ = ' ';
return 0;
}
