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
 * Header info for the hypre_StructSolver structures
 *
 *****************************************************************************/

#ifndef hypre_PETScSolverParILUT_HEADER
#define hypre_PETScSolverParILUT_HEADER

#include <HYPRE_config.h>

#include "general.h"
#include "utilities.h"
#ifdef HYPRE_DEBUG
#include <gmalloc.h>
#endif

/* Include Petsc linear solver headers */
#include "sles.h"

#include "HYPRE.h"

/* type definitions from this directory */
#include "HYPRE_PETScSolverParILUT_types.h"

/* type and prototype declarations for object used in this implementation */
#include "HYPRE_PETScMatPilutSolver_types.h"
#include "HYPRE_PETScMatPilutSolver_protos.h"


/* type definition for member SlesOwner in ParILUTData structure */
#define ParILUTLibrary 47
#define ParILUTUser    98

/*--------------------------------------------------------------------------
 * hypre_PETScSolverParILUT
 *--------------------------------------------------------------------------*/

typedef struct
{

  MPI_Comm        comm;

  /* Linear solver structure from Petsc */
  SLES            Sles;
  int             SlesOwner; /* Keeps track of whether library or user allocated
                                SLES for freeing purposes */

  /* Petsc Matrix that defines the system to be solved */
  Mat             SystemMatrix;

  /* Petsc Matrix from which to build the preconditioner */
  Mat             PreconditionerMatrix;

  /* Preconditioner is Parallel ILUT through the HYPRE_PETScMatPilutSolver */
  HYPRE_PETScMatPilutSolver PETScMatPilutSolver;

  /* Diagnostic information */
  int             number_of_iterations;

} hypre_PETScSolverParILUT;

/*--------------------------------------------------------------------------
 * Accessor macros: hypre_PETScSolverParILUT
 *--------------------------------------------------------------------------*/

#define hypre_PETScSolverParILUTComm(parilut_data)        ((parilut_data) -> comm)
#define hypre_PETScSolverParILUTSles(parilut_data)        ((parilut_data) -> Sles)
#define hypre_PETScSolverParILUTSlesOwner(parilut_data)       ((parilut_data) -> SlesOwner)
#define hypre_PETScSolverParILUTPreconditionerMatrix(parilut_data)\
                                         ((parilut_data) -> PreconditionerMatrix)
#define hypre_PETScSolverParILUTSystemMatrix(parilut_data)\
                                         ((parilut_data) -> SystemMatrix)
#define hypre_PETScSolverParILUTPETScMatPilutSolver(parilut_data)\
                                         ((parilut_data) -> PETScMatPilutSolver)
#define hypre_PETScSolverParILUTNumIts(parilut_data)\
                                         ((parilut_data) -> number_of_iterations)

/* Include internal prototypes */
#include "./hypre_protos.h"
#include "./internal_protos.h"


#endif
