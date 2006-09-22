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
 *****************************************************************************/

#include "headers.h"

/*==========================================================================*/
/*==========================================================================*/
/**
  Augments measures by some random value between 0 and 1.

  {\bf Input files:}
  headers.h

  @return Error code.

  @param S [IN]
  parent graph matrix in CSR format
  @param measure_array [IN/OUT]
  measures assigned to each node of the parent graph

  @see hypre_AMGIndepSet */
/*--------------------------------------------------------------------------*/

int
hypre_BoomerAMGIndepSetInit( hypre_ParCSRMatrix *S,
                          double             *measure_array ,
                          int   seq_rand)
{
   hypre_CSRMatrix *S_diag = hypre_ParCSRMatrixDiag(S);
   MPI_Comm         comm = hypre_ParCSRMatrixComm(S);
   int              S_num_nodes = hypre_CSRMatrixNumRows(S_diag);
   int              i, my_id;
   int              ierr = 0;

   MPI_Comm_rank(comm,&my_id);
   i = 2747+my_id;
   if (seq_rand) i = 2747;
   hypre_SeedRand(i);
   if (seq_rand)
   {
      for (i = 0; i < hypre_ParCSRMatrixFirstRowIndex(S); i++)
	hypre_Rand(); 
   }
   for (i = 0; i < S_num_nodes; i++)
   {
      measure_array[i] += hypre_Rand();
   }

   return (ierr);
}

/*==========================================================================*/
/*==========================================================================*/
/**
  Select an independent set from a graph.  This graph is actually a
  subgraph of some parent graph.  The parent graph is described as a
  matrix in compressed sparse row format, where edges in the graph are
  represented by nonzero matrix coefficients (zero coefficients are
  ignored).  A positive measure is given for each node in the
  subgraph, and this is used to pick the independent set.  A measure
  of zero must be given for all other nodes in the parent graph.  The
  subgraph is a collection of nodes in the parent graph.

  Positive entries in the `IS\_marker' array indicate nodes in the
  independent set.  All other entries are zero.

  The algorithm proceeds by first setting all nodes in `graph\_array'
  to be in the independent set.  Nodes are then removed from the
  independent set by simply comparing the measures of adjacent nodes.

  {\bf Input files:}
  headers.h

  @return Error code.

  @param S [IN]
  parent graph matrix in CSR format
  @param measure_array [IN]
  measures assigned to each node of the parent graph
  @param graph_array [IN]
  node numbers in the subgraph to be partitioned
  @param graph_array_size [IN]
  number of nodes in the subgraph to be partitioned
  @param IS_marker [IN/OUT]
  marker array for independent set

  @see hypre_InitAMGIndepSet */
/*--------------------------------------------------------------------------*/

int
hypre_BoomerAMGIndepSet( hypre_ParCSRMatrix *S,
                      double             *measure_array,
                      int                *graph_array,
                      int                 graph_array_size,
                      int                *graph_array_offd,
                      int                 graph_array_offd_size,
                      int                *IS_marker,
                      int                *IS_marker_offd     )
{
   hypre_CSRMatrix *S_diag      = hypre_ParCSRMatrixDiag(S);
   int             *S_diag_i    = hypre_CSRMatrixI(S_diag);
   int             *S_diag_j    = hypre_CSRMatrixJ(S_diag);
   hypre_CSRMatrix *S_offd      = hypre_ParCSRMatrixOffd(S);
   int             *S_offd_i    = hypre_CSRMatrixI(S_offd);
   int             *S_offd_j;

   int		    local_num_vars = hypre_CSRMatrixNumRows(S_diag);
   int              i, j, ig, jS, jj;
                   
   int              ierr = 0;

   /*-------------------------------------------------------
    * Initialize IS_marker by putting all nodes in
    * the independent set.
    *-------------------------------------------------------*/

   if (hypre_CSRMatrixNumCols(S_offd))
   {
	S_offd_j = hypre_CSRMatrixJ(S_offd);
   }

   for (ig = 0; ig < graph_array_size; ig++)
   {
      i = graph_array[ig];
      if (measure_array[i] > 1)
      {
         IS_marker[i] = 1;
      }
   }
   for (ig = 0; ig < graph_array_offd_size; ig++)
   {
      i = graph_array_offd[ig];
      if (measure_array[i+local_num_vars] > 1)
      {
         IS_marker_offd[i] = 1;
      }
   }

   /*-------------------------------------------------------
    * Remove nodes from the initial independent set
    *-------------------------------------------------------*/

   for (ig = 0; ig < graph_array_size; ig++)
   {
      i = graph_array[ig];
      if (measure_array[i] > 1)
      {
         for (jS = S_diag_i[i]; jS < S_diag_i[i+1]; jS++)
         {
            j = S_diag_j[jS];
	    if (j < 0) j = -j-1;
            
            /* only consider valid graph edges */
            /* if ( (measure_array[j] > 1) && (S_diag_data[jS]) ) */
            if (measure_array[j] > 1) 
            {
               if (measure_array[i] > measure_array[j])
               {
                  IS_marker[j] = 0;
               }
               else if (measure_array[j] > measure_array[i])
               {
                  IS_marker[i] = 0;
               }
            }
         }
         for (jS = S_offd_i[i]; jS < S_offd_i[i+1]; jS++)
         {
            jj = S_offd_j[jS];
            if (jj < 0) jj = -jj-1;
            j = local_num_vars+jj;
            
            /* only consider valid graph edges */
            /* if ( (measure_array[j] > 1) && (S_offd_data[jS]) ) */
            if (measure_array[j] > 1) 
            {
               if (measure_array[i] > measure_array[j])
               {
                  IS_marker_offd[jj] = 0;
               }
               else if (measure_array[j] > measure_array[i])
               {
                  IS_marker[i] = 0;
               }
            }
         }
      }
   }
            
   return (ierr);
}
         
