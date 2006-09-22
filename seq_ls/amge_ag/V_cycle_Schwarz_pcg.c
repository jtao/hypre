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





#include "headers.h"

/*****************************************************************************
 *
 * PCG method with one V-cycle iteration; x = 0 -- initial iterate;
 *                 as preconditioner;
 *
 * symmetric multiplicative Schwarz smoother;
 *
 ****************************************************************************/


int hypre_VcycleSchwarzpcg(double *x, double *rhs, 
			   double **v_cycle, double **w_cycle,

			   double *reduction_factor_pointer,

			   hypre_CSRMatrix **Matrix,

			   int **i_domain_dof,
			   int **j_domain_dof,
			   double **domain_matrixinverse,
			   int *Nelements,
			     
			   hypre_CSRMatrix **P,

			   double *v_fine, double *w_fine, double *aux_fine,
			   double *v, double *w, double *d, 

			   int max_iter, 
		       
			   double *v_coarse, double *w_coarse, 
			   double *d_coarse, 

			   int nu, 
			   int level, int coarse_level, 

			   int *Ndofs)

{
  int ierr=0, i, j, l;
  float delta0, delta_old, delta, asfac, arfac, eps = 1.e-12;
  
  int iter=0;
  float tau, alpha, beta;
  float delta_x;

  int num_dofs = Ndofs[0];
  int *i_dof_dof = hypre_CSRMatrixI(Matrix[0]);
  int *j_dof_dof = hypre_CSRMatrixJ(Matrix[0]);
  double *sparse_matrix = hypre_CSRMatrixData(Matrix[0]);


  if (max_iter) max_iter = 1000;

  delta0 = 0.e0;
  for (i=0; i<num_dofs; i++)
    delta0+= rhs[i] * rhs[i];

  if (delta0 < eps*eps)
    {

      l = 0;
      ierr = hypre_SchwarzSolve(x, rhs,  aux_fine, 

				
				i_dof_dof[l], 
				j_dof_dof[l],
				sparse_matrix[l],

				i_domain_dof[l], 
				j_domain_dof[l], 
				/* Nelements[l+1], */
				Nelements[l],

				domain_matrixinverse[l],
			    
				Ndofs[l]);
      return ierr;
    }



  for (i=0; i<num_dofs; i++)
    {
      v_cycle[0][i] = 0.e0; 
      w_cycle[0][i] = rhs[i];
    }

  ierr = hypre_VcycleSchwarzsmoothing(v_cycle, w_cycle,

				      Matrix,

				      i_domain_dof,
				      j_domain_dof,
				      domain_matrixinverse,
				      Nelements,


				      P,
				      v_fine, w_fine, aux_fine,
				      v_coarse, w_coarse, d_coarse, 

				      nu, 
				      level, coarse_level, 
				      Ndofs);



  for (i=0; i < num_dofs; i++)
    x[i] = v_cycle[0][i];



  /* sparse-matrix vector product: --------------------------*/

  ierr = sparse_matrix_vector_product(v,
				      sparse_matrix, 
				      x, 
				      i_dof_dof, j_dof_dof,
				      num_dofs);

  /* compute residual: w <-- rhs - A *x = rhs - v;          */
  for (i=0; i < num_dofs; i++)
    w[i] = rhs[i] - v[i];


  delta0 = 0.e0;
  for (i=0; i<num_dofs; i++)
    delta0+= w[i] * w[i];

  if (delta0 < eps*eps)
    return ierr;



  for (i=0; i<num_dofs; i++)
    {
      v_cycle[0][i] = 0.e0; 
      w_cycle[0][i] = w[i];
    }

  ierr = hypre_VcycleSchwarzsmoothing(v_cycle, w_cycle,

				      Matrix,

				      i_domain_dof,
				      j_domain_dof,
				      domain_matrixinverse,
				      Nelements,


				      P,
				      v_fine, w_fine, aux_fine,
				      v_coarse, w_coarse, d_coarse, 

				      nu, 
				      level, coarse_level, 
				      Ndofs);



  for (i=0; i < num_dofs; i++)
    d[i] = v_cycle[0][i];

  delta0 = 0.e0;
  for (i=0; i<num_dofs; i++)
    delta0+= w[i] * d[i];

  if (max_iter > 999)
    printf("hypre_VcycleSchwarzpcg: delta0: %e\n", delta0); 

  delta_old = delta0;

  /* for (i=0; i < num_dofs; i++)
     d[i] = w[i]; */

loop:

  /* sparse-matrix vector product: --------------------------*/

  ierr = sparse_matrix_vector_product(v,
				      sparse_matrix, 
				      d, 
				      i_dof_dof, j_dof_dof,
				      num_dofs);

  tau = 0.e0;
  for (i=0; i<num_dofs; i++)
    tau += d[i] * v[i];

      if (tau <= 0.e0) 
	{
	  printf("indefinite matrix: %e\n", tau);
	  /*	  return -1;                               */
	}

  alpha = delta_old/tau;
  for (i=0; i<num_dofs; i++)
    x[i] += alpha * d[i];

  for (i=0; i<num_dofs; i++)
    w[i] -= alpha * v[i];



  for (i=0; i<num_dofs; i++)
    {
      v_cycle[0][i] = 0.e0; 
      w_cycle[0][i] = w[i];
    }


  ierr = hypre_VcycleSchwarzsmoothing(v_cycle, w_cycle,

				      Matrix,

				      i_domain_dof,
				      j_domain_dof,
				      domain_matrixinverse,
				      Nelements,


				      P,
				      v_fine, w_fine, aux_fine,
				      v_coarse, w_coarse, d_coarse, 

				      nu, 
				      level, coarse_level, 
				      Ndofs);



  for (i=0; i < num_dofs; i++)
    v[i] = v_cycle[0][i];


  delta = 0.e0;
  for (i=0; i<num_dofs; i++)
    delta += v[i] * w[i];

  beta = delta /delta_old;
  iter++;

  if (max_iter > 999)
    printf("              hypre_VcycleSchwarzpcg_iteration: %d;  residual_delta: %e,   arfac: %e\n", iter, sqrt(delta), sqrt(beta));	 

  delta_old = delta;

  for (i=0; i<num_dofs; i++)
    d[i] = v[i] + beta * d[i];

  if (delta > eps * delta0 && iter < max_iter) goto loop;

  asfac = sqrt(beta);
  arfac = exp(log(delta/delta0)/(2*iter));

  if (max_iter > 999)
    {
      /*==================================================================*/
      printf("hypre_VcycleSchwarzpcg: delta0: %e; delta: %e\n", delta0, delta);
      printf("hypre_VcycleSchwarzpcg: iterations: %d; reduction factors: %e, %e\n", iter, asfac, arfac);
      /*==================================================================*/
    }
 
  *reduction_factor_pointer =  arfac;

  return ierr;
}
