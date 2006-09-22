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



/* ******************************************************************** */
/* See the file COPYRIGHT for a complete copyright notice, contact      */
/* person and disclaimer.                                               */        
/* ******************************************************************** */

/* ******************************************************************** */
/* Declaration of the ML structure                                      */
/* ******************************************************************** */
/* Author : Charles Tong (LLNL) and Raymond Tuminaro (SNL)              */
/* Date   : February, 1999                                              */
/* ******************************************************************** */

#ifndef _MLSOLVER_
#define _MLSOLVER_

/* #include <stdlib.h> */
#include "ml_common.h"
#include "ml_defs.h"

#ifdef DSUPERLU
#include "superlu_ddefs.h"
#endif

typedef struct ML_Lugrid_Struct 
{
#ifdef DSUPERLU
  gridinfo_t grid;
#endif
  int count;
} ML_Lugrid;

typedef struct ML_Solver_Struct 
{
   int          ML_id;               /* ID for the Solver structure     */
   int          reuse_flag;          /* flag for internal use           */
   void         (*func)(void);       /* function to perform the solve   */
   void         *Mat1;               /* primary matrix for the solver   */
   void         *Mat2;               /* L matrix (for direct solver)    */
   void         *Mat3;               /* U matrix (for direct solver)    */
   int          int_data;            /* integer data for the solver     */
   int          int_data2;           /* integer data for the solver     */
   double       dble_data;           /* double data for the solver      */
   int          *int_params1;        /* integer array for the solver    */
   int          *int_params2;        /* integer array for the solver    */
   double       *dble_params1;       /* double array for the solver     */
   double       *dble_params2;       /* double array for the solver     */
   void         *void_params1;       /* other data for the solver       */
   void         *void_params2;       /* other data for the solver       */
   void         *LUspl;              /* for direct solver               */
   void         *PERMspl;            /* for direct solver               */
   int          ML_subgroup;
   ML_Lugrid    *gridtiles;
} ML_Solver;

/* Changed void ML_subcomm to int ML_subgroup */

#ifdef __cplusplus
extern "C" {
#endif

int ML_Solver_Create( ML_Solver **sol );
int ML_Solver_Destroy( ML_Solver **sol );
int ML_Solver_Check( ML_Solver *sol );

#ifdef __cplusplus
}
#endif
#endif
