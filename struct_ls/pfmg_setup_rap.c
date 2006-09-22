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
 *
 *****************************************************************************/

#include "headers.h"
#include "pfmg.h"

/*--------------------------------------------------------------------------
 * hypre_PFMGCreateRAPOp
 *
 *   Wrapper for 2 and 3d CreateRAPOp routines which set up new coarse
 *   grid structures.
 *
 *   The parameter rap_type controls which lower level routines are
 *   used.
 *      rap_type = 0   Use optimized code for computing Galerkin operators
 *                     for special, common stencil patterns: 5 & 9 pt in
 *                     2d and 7, 19 & 27 in 3d.
 *      rap_type = 1   Use PARFLOW formula for coarse grid operator. Used
 *                     only with 5pt in 2d and 7pt in 3d.
 *      rap_type = 2   General purpose Galerkin code.
 *--------------------------------------------------------------------------*/
 
hypre_StructMatrix *
hypre_PFMGCreateRAPOp( hypre_StructMatrix *R,
                       hypre_StructMatrix *A,
                       hypre_StructMatrix *P,
                       hypre_StructGrid   *coarse_grid,
                       int                 cdir,
                       int                 rap_type    )
{
   hypre_StructMatrix    *RAP;
   hypre_StructStencil   *stencil;
   int                    P_stored_as_transpose = 0;
   int                    constant_coefficient;

   stencil = hypre_StructMatrixStencil(A);

   if (rap_type == 0)
   {
      switch (hypre_StructStencilDim(stencil)) 
      {
      case 2:
         RAP = hypre_PFMG2CreateRAPOp(R ,A, P, coarse_grid, cdir);
         break;
    
      case 3:
         RAP = hypre_PFMG3CreateRAPOp(R ,A, P, coarse_grid, cdir);
         break;
      } 
   }

   else if (rap_type == 1)
   {
      switch (hypre_StructStencilDim(stencil)) 
      {
         case 2:
         RAP =  hypre_PFMGCreateCoarseOp5(R ,A, P, coarse_grid, cdir);
         break;
    
         case 3:
         RAP =  hypre_PFMGCreateCoarseOp7(R ,A, P, coarse_grid, cdir);
         break;
      } 
   }
   else if (rap_type == 2)
   {
      RAP = hypre_SemiCreateRAPOp(R ,A, P, coarse_grid, cdir,
                                        P_stored_as_transpose);
   }


   constant_coefficient = hypre_StructMatrixConstantCoefficient(A);
   if ( constant_coefficient==2 && rap_type==0 )
   {
      /* A has variable diagonal, so, in the Galerkin case, P (and R) is
         entirely variable coefficient.  Thus RAP will be variable coefficient */
      hypre_StructMatrixSetConstantCoefficient( RAP, 0 );
   }
   else
   {
      hypre_StructMatrixSetConstantCoefficient( RAP, constant_coefficient );
   }

   return RAP;
}

/*--------------------------------------------------------------------------
 * hypre_PFMGSetupRAPOp
 *
 * Wrapper for 2 and 3d, symmetric and non-symmetric routines to calculate
 * entries in RAP. Incomplete error handling at the moment. 
 *
 *   The parameter rap_type controls which lower level routines are
 *   used.
 *      rap_type = 0   Use optimized code for computing Galerkin operators
 *                     for special, common stencil patterns: 5 & 9 pt in
 *                     2d and 7, 19 & 27 in 3d.
 *      rap_type = 1   Use PARFLOW formula for coarse grid operator. Used
 *                     only with 5pt in 2d and 7pt in 3d.
 *      rap_type = 2   General purpose Galerkin code.
 *--------------------------------------------------------------------------*/
 
int
hypre_PFMGSetupRAPOp( hypre_StructMatrix *R,
                      hypre_StructMatrix *A,
                      hypre_StructMatrix *P,
                      int                 cdir,
                      hypre_Index         cindex,
                      hypre_Index         cstride,
                      int                 rap_type,
                      hypre_StructMatrix *Ac      )
{
   int                    ierr = 0;
   int                    P_stored_as_transpose = 0;
   hypre_StructStencil   *stencil;

   stencil = hypre_StructMatrixStencil(A);

   if (rap_type == 0)
   {
      switch (hypre_StructStencilDim(stencil)) 
      {
         case 2:
         /*--------------------------------------------------------------------
          *    Set lower triangular (+ diagonal) coefficients
          *--------------------------------------------------------------------*/
         ierr = hypre_PFMG2BuildRAPSym(A, P, R, cdir, cindex, cstride, Ac);

         /*--------------------------------------------------------------------
          *    For non-symmetric A, set upper triangular coefficients as well
          *--------------------------------------------------------------------*/
         if(!hypre_StructMatrixSymmetric(A))
            ierr += hypre_PFMG2BuildRAPNoSym(A, P, R, cdir, cindex, cstride, Ac);

         break;

         case 3:

         /*--------------------------------------------------------------------
          *    Set lower triangular (+ diagonal) coefficients
          *--------------------------------------------------------------------*/
         ierr = hypre_PFMG3BuildRAPSym(A, P, R, cdir, cindex, cstride, Ac);

         /*--------------------------------------------------------------------
          *    For non-symmetric A, set upper triangular coefficients as well
          *--------------------------------------------------------------------*/
         if(!hypre_StructMatrixSymmetric(A))
            ierr += hypre_PFMG3BuildRAPNoSym(A, P, R, cdir, cindex, cstride, Ac);

         break;
      } 
   }

   else if (rap_type == 1)
   {
      switch (hypre_StructStencilDim(stencil)) 
      {
         case 2:
         ierr = hypre_PFMGBuildCoarseOp5(A, P, R, cdir, cindex, cstride, Ac);
         break;

         case 3:
         ierr = hypre_PFMGBuildCoarseOp7(A, P, R, cdir, cindex, cstride, Ac);
         break;
      } 
   }

   else if (rap_type == 2)
   {
      ierr = hypre_SemiBuildRAP(A, P, R, cdir, cindex, cstride,
                                       P_stored_as_transpose, Ac);
   }

   hypre_StructMatrixAssemble(Ac);

   return ierr;
}

