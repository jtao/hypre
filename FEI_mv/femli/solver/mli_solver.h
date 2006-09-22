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




/******************************************************************************
 *
 * Header info for the MLI_Solver data structure
 *
 *****************************************************************************/

#ifndef __MLI_SOLVER_H__
#define __MLI_SOLVER_H__

/*--------------------------------------------------------------------------
 * defines 
 *--------------------------------------------------------------------------*/

#define MLI_SOLVER_JACOBI_ID        301
#define MLI_SOLVER_BJACOBI_ID       302
#define MLI_SOLVER_GS_ID            303
#define MLI_SOLVER_SGS_ID           304
#define MLI_SOLVER_BSGS_ID          305
#define MLI_SOLVER_PARASAILS_ID     306
#define MLI_SOLVER_MLS_ID           307
#define MLI_SOLVER_CHEBYSHEV_ID     308
#define MLI_SOLVER_CG_ID            309
#define MLI_SOLVER_SUPERLU_ID       310
#define MLI_SOLVER_SEQSUPERLU_ID    311
#define MLI_SOLVER_ARPACKSUPERLU_ID 312
#define MLI_SOLVER_KARCMARZ_ID      313
#define MLI_SOLVER_GMRES_ID         314
#define MLI_SOLVER_MLI_ID           315
#define MLI_SOLVER_ILU_ID           316
#define MLI_SOLVER_AMG_ID           317

/*--------------------------------------------------------------------------
 * include files 
 *--------------------------------------------------------------------------*/

#include <string.h>
#include "utilities/utilities.h"

#include "matrix/mli_matrix.h"
#include "vector/mli_vector.h"

/*--------------------------------------------------------------------------
 * MLI_Solver data structure declaration
 *--------------------------------------------------------------------------*/

class MLI_Solver
{
   char solver_name[100];

public :

   MLI_Solver(char *name);
   virtual ~MLI_Solver();

   char*   getName();
   virtual int setup(MLI_Matrix *)=0;
   virtual int solve(MLI_Vector *, MLI_Vector *)=0;
   virtual int setParams(char *paramString,int argc,char **argv);
   virtual int getParams(char *paramString,int *argc,char **argv);
};

/*--------------------------------------------------------------------------
 * MLI_Solver functions 
 *--------------------------------------------------------------------------*/

extern MLI_Solver *MLI_Solver_CreateFromName(char *);

#endif

