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




      subroutine equate(m, n, 
     &                  matrix_old, matrix_new)
c
c -----------------------------------------------------------------------
c This is an example of a F77 code illustrating the indentation used
c for CASC codes. This code does not illustrate issues related to
c documentation or error handling.
c 
c The most important item here is consistent indentation of the following
c structures:
c    - do loops
c    - if-then-else-endif statements
c    - do-while statements
c In each of the above, we recommend indentation by 3 spaces - 2 is sometimes
c difficult to detect, and 4 can be too much in deeply nested statements.
c 
c The following are left to the discretion of a project. The only constraint
c is that the standard be applied consistently to all F77/F90 codes within
c a project.
c    - upper/lower case for codes
c    - upper/lower/mixed case for comments
c    - whether a code has blank lines or blank comments (a line with
c      c in the first column)
c    - where blank lines should be added to improve the modularity of the 
c      code
c 
c Note that this code does something nonsensical - it is mainly for the
c illustration of indentation.
c
c -----------------------------------------------------------------------
c 

      implicit none

      integer m, n
      integer i, j

      real*8 matrix_new(m,n), matrix_old(m,n)

      if (m .gt. n) then 
c
         do j = 1, n
            do i = 1, m
               matrix_new(i,j) = matrix_old(i,j)
            end do
         end do
c 
      else 
c
         do j = 1, n
            do i = 1, m
               matrix_new(i,j) = - matrix_old(i,j)
            end do
         end do
c
      end if

      return
      end
