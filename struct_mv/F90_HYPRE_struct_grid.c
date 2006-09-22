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
 * HYPRE_StructGrid interface
 *
 *****************************************************************************/
#include "headers.h"
#include "fortran.h"

/*--------------------------------------------------------------------------
 * HYPRE_StructGridCreate
 *--------------------------------------------------------------------------*/

void
hypre_F90_IFACE(hypre_structgridcreate, HYPRE_STRUCTGRIDCREATE)( int      *comm,
                                         int      *dim,
                                         long int *grid,
                                         int      *ierr )
{
   *ierr = (int) ( HYPRE_StructGridCreate( (MPI_Comm)           *comm,
                                           (int)                *dim,
                                           (HYPRE_StructGrid *) grid ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_StructGridDestroy
 *--------------------------------------------------------------------------*/

void
hypre_F90_IFACE(hypre_structgriddestroy, HYPRE_STRUCTGRIDDESTROY)( long int *grid,
                                          int      *ierr )
{
   *ierr = (int) ( HYPRE_StructGridDestroy( (HYPRE_StructGrid) *grid ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_StructGridSetExtents
 *--------------------------------------------------------------------------*/

void
hypre_F90_IFACE(hypre_structgridsetextents, HYPRE_STRUCTGRIDSETEXTENTS)( long int *grid,
                                             int      *ilower,
                                             int      *iupper,
                                             int      *ierr )
{
   *ierr = (int) ( HYPRE_StructGridSetExtents( (HYPRE_StructGrid) *grid,
                                               (int *)            ilower,
                                               (int *)            iupper ) );
}

/*--------------------------------------------------------------------------
 * HYPRE_SetStructGridPeriodicity
 *--------------------------------------------------------------------------*/

void
hypre_F90_IFACE(hypre_structgridsetperiodic, HYPRE_STRUCTGRIDSETPERIODIC)( long int *grid,
                                              int      *periodic,
                                              int      *ierr)
{
   *ierr = (int) ( HYPRE_StructGridSetPeriodic( (HYPRE_StructGrid) *grid,
                                                (int *)             periodic) );
}

/*--------------------------------------------------------------------------
 * HYPRE_StructGridAssemble
 *--------------------------------------------------------------------------*/

void
hypre_F90_IFACE(hypre_structgridassemble, HYPRE_STRUCTGRIDASSEMBLE)( long int *grid,
                                           int      *ierr )
{
   *ierr = (int) ( HYPRE_StructGridAssemble( (HYPRE_StructGrid) *grid) );
}

/*--------------------------------------------------------------------------
 * HYPRE_StructGridSetNumGhost
 *--------------------------------------------------------------------------*/

void
hypre_F90_IFACE(hypre_structgridsetnumghost, HYPRE_STRUCTGRIDSETNUMGHOST)
                                         ( long int *grid,
                                           int      *num_ghost,
                                           int      *ierr )
{
   *ierr = (int) ( HYPRE_StructGridSetNumGhost( (HYPRE_StructGrid) *grid,
                                                (int *)            num_ghost) );
}
