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
 * Relaxation scheme
 *
 *****************************************************************************/

#include "headers.h"


/*--------------------------------------------------------------------------
 * hypre_AMGRelax
 *--------------------------------------------------------------------------*/

int  hypre_AMGRelax( hypre_CSRMatrix *A,
                         hypre_Vector    *f,
                         int            *cf_marker,
                         int             relax_type,
                         int             relax_points,
                         double          relax_weight,
                         hypre_Vector    *u,
                         hypre_Vector    *Vtemp )
{
   double         *A_data  = hypre_CSRMatrixData(A);
   int            *A_i     = hypre_CSRMatrixI(A);
   int            *A_j     = hypre_CSRMatrixJ(A);

   int             n       = hypre_CSRMatrixNumRows(A);
   
   double         *u_data  = hypre_VectorData(u);
   double         *f_data  = hypre_VectorData(f);

   double         *Vtemp_data = hypre_VectorData(Vtemp);
   
   double          res;
	          
   int             i, ii;
   int             jj;
   int             column;
   int             relax_error = 0;

   double         *A_mat;
   double         *b_vec;

   double          zero = 0.0;
   double 	   one_minus_weight = 1.0 -relax_weight;
 
   /*-----------------------------------------------------------------------
    * Switch statement to direct control based on relax_type:
    *     relax_type = 0 -> Jacobi
    *     relax_type = 1 -> Gauss-Seidel
    *     relax_type = 2 -> symm. Gauss-Seidel
    *     relax_type = 9 -> Direct Solve
    *-----------------------------------------------------------------------*/
   
   switch (relax_type)
   {
      case 0: /* Weighted Jacobi */
      {

         /*-----------------------------------------------------------------
          * Copy current approximation into temporary vector.
          *-----------------------------------------------------------------*/
         
         for (i = 0; i < n; i++)
         {
            Vtemp_data[i] = u_data[i];
         }

         /*-----------------------------------------------------------------
          * Relax all points.
          *-----------------------------------------------------------------*/

         if (relax_points == 0)
         {
            for (i = 0; i < n; i++)
            {

               /*-----------------------------------------------------------
                * If diagonal is nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * Vtemp_data[ii];
                  }
                  u_data[i] *= one_minus_weight;
                  u_data[i] += relax_weight * res / A_data[A_i[i]];
               }
            }
         }

         /*-----------------------------------------------------------------
          * Relax only C or F points as determined by relax_points.
          *-----------------------------------------------------------------*/

         else
         {
            for (i = 0; i < n; i++)
            {

               /*-----------------------------------------------------------
                * If i is of the right type ( C or F ) and diagonal is
                * nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (cf_marker[i] == relax_points && A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * Vtemp_data[ii];
                  }
                  u_data[i] *= one_minus_weight;
                  u_data[i] += relax_weight * res / A_data[A_i[i]];
               }
            }     
         }
         
      }
      break;
      
      case 1: /* Gauss-Seidel */
      {

         /*-----------------------------------------------------------------
          * Relax all points.
          *-----------------------------------------------------------------*/

         if (relax_points == 0)
         {
            for (i = 0; i < n; i++)
            {

               /*-----------------------------------------------------------
                * If diagonal is nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * u_data[ii];
                  }
                  u_data[i] = res / A_data[A_i[i]];
               }
            }
         }

         /*-----------------------------------------------------------------
          * Relax only C or F points as determined by relax_points.
          *-----------------------------------------------------------------*/

         else
         {
            for (i = 0; i < n; i++)
            {

               /*-----------------------------------------------------------
                * If i is of the right type ( C or F ) and diagonal is
                * nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (cf_marker[i] == relax_points && A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * u_data[ii];
                  }
                  u_data[i] = res / A_data[A_i[i]];
               }
            }     
         }
         
      }
      break;

      case 2: /* symm. Gauss-Seidel */
      {

         /*-----------------------------------------------------------------
          * Relax all points.
          *-----------------------------------------------------------------*/

         if (relax_points == 0)
         {
            for (i = 0; i < n; i++)
            {

               /*-----------------------------------------------------------
                * If diagonal is nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * u_data[ii];
                  }
                  u_data[i] = res / A_data[A_i[i]];
               }
            }
            for (i = n-1; i > -1; i--)
            {

               /*-----------------------------------------------------------
                * If diagonal is nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * u_data[ii];
                  }
                  u_data[i] = res / A_data[A_i[i]];
               }
            }
         }

         /*-----------------------------------------------------------------
          * Relax only C or F points as determined by relax_points.
          *-----------------------------------------------------------------*/

         else
         {
            for (i = 0; i < n; i++)
            {

               /*-----------------------------------------------------------
                * If i is of the right type ( C or F ) and diagonal is
                * nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (cf_marker[i] == relax_points && A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * u_data[ii];
                  }
                  u_data[i] = res / A_data[A_i[i]];
               }
            }     
            for (i = n-1; i > -1; i--)
            {

               /*-----------------------------------------------------------
                * If i is of the right type ( C or F ) and diagonal is
                * nonzero, relax point i; otherwise, skip it.
                *-----------------------------------------------------------*/
             
               if (cf_marker[i] == relax_points && A_data[A_i[i]] != zero)
               {
                  res = f_data[i];
                  for (jj = A_i[i]+1; jj < A_i[i+1]; jj++)
                  {
                     ii = A_j[jj];
                     res -= A_data[jj] * u_data[ii];
                  }
                  u_data[i] = res / A_data[A_i[i]];
               }
            }     
         }
         
      }
      break;

      case 9: /* Direct solve: use gaussian elimination */
      {

         A_mat = hypre_CTAlloc(double, n*n);
         b_vec = hypre_CTAlloc(double, n);    

         /*-----------------------------------------------------------------
          *  Load CSR matrix into A_mat.
          *-----------------------------------------------------------------*/

         for (i = 0; i < n; i++)
         {
            for (jj = A_i[i]; jj < A_i[i+1]; jj++)
            {
               column = A_j[jj];
               A_mat[i*n+column] = A_data[jj];
            }
            b_vec[i] = f_data[i];
         }

         relax_error = gselim(A_mat,b_vec,n);

         for (i = 0; i < n; i++)
         {
            u_data[i] = b_vec[i];
         }

         hypre_TFree(A_mat); 
         hypre_TFree(b_vec);
         
      }
      break;   
   }

   return(relax_error); 
}

/*-------------------------------------------------------------------------
 *
 *                      Gaussian Elimination
 *
 *------------------------------------------------------------------------ */

int gselim(A,x,n)
double *A;
double *x;
int n;
{
   int    err_flag = 0;
   int    j,k,m;
   double factor;
   
   if (n==1)                           /* A is 1x1 */  
   {
      if (A[0] != 0.0)
      {
         x[0] = x[0]/A[0];
         return(err_flag);
      }
      else
      {
         err_flag = 1;
         return(err_flag);
      }
   }
   else                               /* A is nxn.  Forward elimination */ 
   {
      for (k = 0; k < n-1; k++)
      {
          if (A[k*n+k] != 0.0)
          {          
             for (j = k+1; j < n; j++)
             {
                 if (A[j*n+k] != 0.0)
                 {
                    factor = A[j*n+k]/A[k*n+k];
                    for (m = k+1; m < n; m++)
                    {
                        A[j*n+m]  -= factor * A[k*n+m];
                    }
                                     /* Elimination step for rhs */ 
                    x[j] -= factor * x[k];              
                 }
             }
          }
       }
                                    /* Back Substitution  */
       for (k = n-1; k > 0; --k)
       {
           x[k] /= A[k*n+k];
           for (j = 0; j < k; j++)
           {
               if (A[j*n+k] != 0.0)
               {
                  x[j] -= x[k] * A[j*n+k];
               }
           }
       }
       x[0] /= A[0];
       return(err_flag);
    }
}
 

         


      
