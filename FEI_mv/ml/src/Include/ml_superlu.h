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




/********************************************************************* */
/* See the file COPYRIGHT for a complete copyright notice, contact      */
/* person and disclaimer.                                               */        
/* ******************************************************************** */

/********************************************************************* */
/*          Utilities for Aztec/SuperLU users                          */
/********************************************************************* */


#ifndef __MLSUPERLU__
#define __MLSUPERLU__

#include "ml_common.h"
#include "ml_solver.h"

typedef struct ML_Sm_Schwarz_Data_Struct ML_Sm_Schwarz_Data;

struct ML_Sm_Schwarz_Data_Struct 
{
   int           Nrows;
   int           **bmat_ia;
   int           **bmat_ja;
   double        **bmat_aa;
   int           **aux_bmat_ia;
   int           **aux_bmat_ja;
   double        **aux_bmat_aa;
   ML_CommInfoOP *getrow_comm;
   int           nblocks;
   int           *blk_info;
   int           *blk_size;
   int           **blk_indices;
   int           **perm_r;
   int           **perm_c;
#if defined(SUPERLU)
   SuperMatrix   **slu_Amat;
   SuperMatrix   **slu_Lmat;
   SuperMatrix   **slu_Umat;
#endif
};

#ifndef ML_CPP
#ifdef __cplusplus
extern "C" {
#endif
#endif

extern int ML_SuperLU_Solve(ML_Solver *vsolver,int ilen,double *x,int olen,
			    double *rhs);

#ifdef WKC
/* These double *'s are Epetra_MultiVector *'s in disguise */
extern int ML_SuperLU_Solve_WKC(ML_Solver *vsolver,int ilen,double *x,int olen,
			    double *rhs);
#endif

extern int ML_SuperLU_SolveLocal(void *vsolver, double *x, double *rhs);
extern int ML_CSolve_Clean_SuperLU( void *vsolver, ML_CSolveFunc *func);
extern  int ML_Smoother_Create_Schwarz_Data(ML_Sm_Schwarz_Data **data);
extern  int ML_Smoother_VBlockSchwarzDecomposition(ML_Sm_Schwarz_Data *, 
                    ML_Operator *, ML_Comm *, int, int *,int*,double *,int *, 
                    int *,int);
extern void ML_SuperLU_Set_Tile( int nprocs, int* tsz, int* stile, 
                                 int* mtile, int* ltile);

#ifndef ML_CPP
#ifdef __cplusplus
}
#endif
#endif

#endif
