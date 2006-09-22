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
 
/*--------------------------------------------------------------------------
 * hypre_GenerateLaplacian
 *--------------------------------------------------------------------------*/

HYPRE_ParCSRMatrix 
GenerateLaplacian( MPI_Comm comm,
                   int      nx,
                   int      ny,
                   int      nz, 
                   int      P,
                   int      Q,
                   int      R,
                   int      p,
                   int      q,
                   int      r,
                   double  *value )
{
   hypre_ParCSRMatrix *A;
   hypre_CSRMatrix *diag;
   hypre_CSRMatrix *offd;

   int    *diag_i;
   int    *diag_j;
   double *diag_data;

   int    *offd_i;
   int    *offd_j;
   double *offd_data;

   int *global_part;
   int ix, iy, iz;
   int cnt, o_cnt;
   int local_num_rows; 
   int *col_map_offd;
   int row_index;
   int i,j;

   int nx_local, ny_local, nz_local;
   int nx_size, ny_size, nz_size;
   int num_cols_offd;
   int grid_size;

   int *nx_part;
   int *ny_part;
   int *nz_part;

   int num_procs, my_id;
   int P_busy, Q_busy, R_busy;

   MPI_Comm_size(comm,&num_procs);
   MPI_Comm_rank(comm,&my_id);

   grid_size = nx*ny*nz;


   hypre_GeneratePartitioning(nx,P,&nx_part);
   hypre_GeneratePartitioning(ny,Q,&ny_part);
   hypre_GeneratePartitioning(nz,R,&nz_part);

   global_part = hypre_CTAlloc(int,P*Q*R+1);

   global_part[0] = 0;
   cnt = 1;
   for (iz = 0; iz < R; iz++)
   {
      nz_size = nz_part[iz+1]-nz_part[iz];
      for (iy = 0; iy < Q; iy++)
      {
         ny_size = ny_part[iy+1]-ny_part[iy];
         for (ix = 0; ix < P; ix++)
         {
            nx_size = nx_part[ix+1] - nx_part[ix];
            global_part[cnt] = global_part[cnt-1];
            global_part[cnt++] += nx_size*ny_size*nz_size;
         }
      }
   }

   nx_local = nx_part[p+1] - nx_part[p];
   ny_local = ny_part[q+1] - ny_part[q];
   nz_local = nz_part[r+1] - nz_part[r];

   my_id = r*(P*Q) + q*P + p;
   num_procs = P*Q*R;

   local_num_rows = nx_local*ny_local*nz_local;
   diag_i = hypre_CTAlloc(int, local_num_rows+1);
   offd_i = hypre_CTAlloc(int, local_num_rows+1);

   P_busy = hypre_min(nx,P);
   Q_busy = hypre_min(ny,Q);
   R_busy = hypre_min(nz,R);

   num_cols_offd = 0;
   if (p) num_cols_offd += ny_local*nz_local;
   if (p < P_busy-1) num_cols_offd += ny_local*nz_local;
   if (q) num_cols_offd += nx_local*nz_local;
   if (q < Q_busy-1) num_cols_offd += nx_local*nz_local;
   if (r) num_cols_offd += nx_local*ny_local;
   if (r < R_busy-1) num_cols_offd += nx_local*ny_local;

   if (!local_num_rows) num_cols_offd = 0;

   col_map_offd = hypre_CTAlloc(int, num_cols_offd);

   cnt = 1;
   o_cnt = 1;
   diag_i[0] = 0;
   offd_i[0] = 0;
   for (iz = nz_part[r]; iz < nz_part[r+1]; iz++)
   {
      for (iy = ny_part[q];  iy < ny_part[q+1]; iy++)
      {
         for (ix = nx_part[p]; ix < nx_part[p+1]; ix++)
         {
            diag_i[cnt] = diag_i[cnt-1];
            offd_i[o_cnt] = offd_i[o_cnt-1];
            diag_i[cnt]++;
            if (iz > nz_part[r]) 
               diag_i[cnt]++;
            else
            {
               if (iz) 
               {
                  offd_i[o_cnt]++;
               }
            }
            if (iy > ny_part[q]) 
               diag_i[cnt]++;
            else
            {
               if (iy) 
               {
                  offd_i[o_cnt]++;
               }
            }
            if (ix > nx_part[p]) 
               diag_i[cnt]++;
            else
            {
               if (ix) 
               {
                  offd_i[o_cnt]++; 
               }
            }
            if (ix+1 < nx_part[p+1]) 
               diag_i[cnt]++;
            else
            {
               if (ix+1 < nx) 
               {
                  offd_i[o_cnt]++; 
               }
            }
            if (iy+1 < ny_part[q+1]) 
               diag_i[cnt]++;
            else
            {
               if (iy+1 < ny) 
               {
                  offd_i[o_cnt]++;
               }
            }
            if (iz+1 < nz_part[r+1]) 
               diag_i[cnt]++;
            else
            {
               if (iz+1 < nz) 
               {
                  offd_i[o_cnt]++;
               }
            }
            cnt++;
            o_cnt++;
         }
      }
   }

   diag_j = hypre_CTAlloc(int, diag_i[local_num_rows]);
   diag_data = hypre_CTAlloc(double, diag_i[local_num_rows]);

   if (num_procs > 1)
   {
      offd_j = hypre_CTAlloc(int, offd_i[local_num_rows]);
      offd_data = hypre_CTAlloc(double, offd_i[local_num_rows]);
   }

   row_index = 0;
   cnt = 0;
   o_cnt = 0;
   for (iz = nz_part[r]; iz < nz_part[r+1]; iz++)
   {
      for (iy = ny_part[q];  iy < ny_part[q+1]; iy++)
      {
         for (ix = nx_part[p]; ix < nx_part[p+1]; ix++)
         {
            diag_j[cnt] = row_index;
            diag_data[cnt++] = value[0];
            if (iz > nz_part[r]) 
            {
               diag_j[cnt] = row_index-nx_local*ny_local;
               diag_data[cnt++] = value[3];
            }
            else
            {
               if (iz) 
               {
                  offd_j[o_cnt] = hypre_map(ix,iy,iz-1,p,q,r-1,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  offd_data[o_cnt++] = value[3];
               }
            }
            if (iy > ny_part[q]) 
            {
               diag_j[cnt] = row_index-nx_local;
               diag_data[cnt++] = value[2];
            }
            else
            {
               if (iy) 
               {
                  offd_j[o_cnt] = hypre_map(ix,iy-1,iz,p,q-1,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  offd_data[o_cnt++] = value[2];
               }
            }
            if (ix > nx_part[p]) 
            {
               diag_j[cnt] = row_index-1;
               diag_data[cnt++] = value[1];
            }
            else
            {
               if (ix) 
               {
                  offd_j[o_cnt] = hypre_map(ix-1,iy,iz,p-1,q,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  offd_data[o_cnt++] = value[1];
               }
            }
            if (ix+1 < nx_part[p+1]) 
            {
               diag_j[cnt] = row_index+1;
               diag_data[cnt++] = value[1];
            }
            else
            {
               if (ix+1 < nx) 
               {
                  offd_j[o_cnt] = hypre_map(ix+1,iy,iz,p+1,q,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  offd_data[o_cnt++] = value[1];
               }
            }
            if (iy+1 < ny_part[q+1]) 
            {
               diag_j[cnt] = row_index+nx_local;
               diag_data[cnt++] = value[2];
            }
            else
            {
               if (iy+1 < ny) 
               {
                  offd_j[o_cnt] = hypre_map(ix,iy+1,iz,p,q+1,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  offd_data[o_cnt++] = value[2];
               }
            }
            if (iz+1 < nz_part[r+1]) 
            {
               diag_j[cnt] = row_index+nx_local*ny_local;
               diag_data[cnt++] = value[3];
            }
            else
            {
               if (iz+1 < nz) 
               {
                  offd_j[o_cnt] = hypre_map(ix,iy,iz+1,p,q,r+1,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  offd_data[o_cnt++] = value[3];
               }
            }
            row_index++;
         }
      }
   }

   if (num_procs > 1)
   {
      for (i=0; i < num_cols_offd; i++)
         col_map_offd[i] = offd_j[i];
   	
      qsort0(col_map_offd, 0, num_cols_offd-1);

      for (i=0; i < num_cols_offd; i++)
         for (j=0; j < num_cols_offd; j++)
            if (offd_j[i] == col_map_offd[j])
            {
               offd_j[i] = j;
               break;
            }
   }


#ifdef HYPRE_NO_GLOBAL_PARTITION
/* ideally we would use less storage earlier in this function, but this is fine
   for testing */
   {
      int tmp1, tmp2;
      tmp1 = global_part[my_id];
      tmp2 = global_part[my_id + 1];
      hypre_TFree(global_part);
      global_part = hypre_CTAlloc(int, 2);
      global_part[0] = tmp1;
      global_part[1] = tmp2;
   }
#endif



   A = hypre_ParCSRMatrixCreate(comm, grid_size, grid_size,
                                global_part, global_part, num_cols_offd,
                                diag_i[local_num_rows],
                                offd_i[local_num_rows]);

   hypre_ParCSRMatrixColMapOffd(A) = col_map_offd;

   diag = hypre_ParCSRMatrixDiag(A);
   hypre_CSRMatrixI(diag) = diag_i;
   hypre_CSRMatrixJ(diag) = diag_j;
   hypre_CSRMatrixData(diag) = diag_data;

   offd = hypre_ParCSRMatrixOffd(A);
   hypre_CSRMatrixI(offd) = offd_i;
   if (num_cols_offd)
   {
      hypre_CSRMatrixJ(offd) = offd_j;
      hypre_CSRMatrixData(offd) = offd_data;
   }

   hypre_TFree(nx_part);
   hypre_TFree(ny_part);
   hypre_TFree(nz_part);

   return (HYPRE_ParCSRMatrix) A;
}

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

int
hypre_map( int  ix,
     int  iy,
     int  iz,
     int  p,
     int  q,
     int  r,
     int  P,
     int  Q,
     int  R, 
     int *nx_part,
     int *ny_part,
     int *nz_part,
     int *global_part )
{
   int nx_local;
   int ny_local;
   int ix_local;
   int iy_local;
   int iz_local;
   int global_index;
   int proc_num;
 
   proc_num = r*P*Q + q*P + p;
   nx_local = nx_part[p+1] - nx_part[p];
   ny_local = ny_part[q+1] - ny_part[q];
   ix_local = ix - nx_part[p];
   iy_local = iy - ny_part[q];
   iz_local = iz - nz_part[r];
   global_index = global_part[proc_num] 
      + (iz_local*ny_local+iy_local)*nx_local + ix_local;

   return global_index;
}

/*--------------------------------------------------------------------------
 * hypre_GenerateVectorLaplacian - this is the systems version
 *--------------------------------------------------------------------------*/

HYPRE_ParCSRMatrix 
GenerateSysLaplacian( MPI_Comm comm,
                      int      nx,
                      int      ny,
                      int      nz, 
                      int      P,
                      int      Q,
                      int      R,
                      int      p,
                      int      q,
                      int      r,
                      int      num_fun,
                      double  *mtrx,
                      double  *value )
{
   hypre_ParCSRMatrix *A;
   hypre_CSRMatrix *diag;
   hypre_CSRMatrix *offd;

   int    *diag_i;
   int    *diag_j;
   double *diag_data;

   int    *offd_i;
   int    *offd_j;
   double *offd_data;

   int *global_part;
   int ix, iy, iz;
   int cnt, o_cnt;
   int local_num_rows; 
   int *col_map_offd;
   int row_index, row, col;
   int index, diag_index;
   int i,j;

   int nx_local, ny_local, nz_local;
   int nx_size, ny_size, nz_size;
   int num_cols_offd;
   int grid_size;
   int local_grid_size;
   int first_j, j_ind;
   int num_coeffs, num_offd_coeffs;

   int *nx_part;
   int *ny_part;
   int *nz_part;

   int num_procs, my_id;
   int P_busy, Q_busy, R_busy;
   double val;

   MPI_Comm_size(comm,&num_procs);
   MPI_Comm_rank(comm,&my_id);

   grid_size = nx*ny*nz;

   hypre_GeneratePartitioning(nx,P,&nx_part);
   hypre_GeneratePartitioning(ny,Q,&ny_part);
   hypre_GeneratePartitioning(nz,R,&nz_part);

   global_part = hypre_CTAlloc(int,P*Q*R+1);

   global_part[0] = 0;
   cnt = 1;
   for (iz = 0; iz < R; iz++)
   {
      nz_size = nz_part[iz+1]-nz_part[iz];
      for (iy = 0; iy < Q; iy++)
      {
         ny_size = ny_part[iy+1]-ny_part[iy];
         for (ix = 0; ix < P; ix++)
         {
            nx_size = nx_part[ix+1] - nx_part[ix];
            global_part[cnt] = global_part[cnt-1];
            global_part[cnt++] += nx_size*ny_size*nz_size;
         }
      }
   }

   nx_local = nx_part[p+1] - nx_part[p];
   ny_local = ny_part[q+1] - ny_part[q];
   nz_local = nz_part[r+1] - nz_part[r];

   my_id = r*(P*Q) + q*P + p;
   num_procs = P*Q*R;

   local_grid_size = nx_local*ny_local*nz_local;
   local_num_rows = num_fun*local_grid_size;
   diag_i = hypre_CTAlloc(int, local_num_rows+1);
   offd_i = hypre_CTAlloc(int, local_num_rows+1);

   P_busy = hypre_min(nx,P);
   Q_busy = hypre_min(ny,Q);
   R_busy = hypre_min(nz,R);

   num_cols_offd = 0;
   if (p) num_cols_offd += ny_local*nz_local;
   if (p < P_busy-1) num_cols_offd += ny_local*nz_local;
   if (q) num_cols_offd += nx_local*nz_local;
   if (q < Q_busy-1) num_cols_offd += nx_local*nz_local;
   if (r) num_cols_offd += nx_local*ny_local;
   if (r < R_busy-1) num_cols_offd += nx_local*ny_local;
   num_cols_offd *= num_fun;

   if (!local_num_rows) num_cols_offd = 0;

   col_map_offd = hypre_CTAlloc(int, num_cols_offd);

   cnt = 1;
   diag_i[0] = 0;
   offd_i[0] = 0;
   for (iz = nz_part[r]; iz < nz_part[r+1]; iz++)
   {
      for (iy = ny_part[q];  iy < ny_part[q+1]; iy++)
      {
         for (ix = nx_part[p]; ix < nx_part[p+1]; ix++)
         {
            diag_i[cnt] = diag_i[cnt-1];
            offd_i[cnt] = offd_i[cnt-1];
            diag_i[cnt] += num_fun;
            if (iz > nz_part[r]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iz) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
            if (iy > ny_part[q]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iy) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
            if (ix > nx_part[p]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (ix) 
               {
                  offd_i[cnt] += num_fun; 
               }
            }
            if (ix+1 < nx_part[p+1]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (ix+1 < nx) 
               {
                  offd_i[cnt] += num_fun; 
               }
            }
            if (iy+1 < ny_part[q+1]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iy+1 < ny) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
            if (iz+1 < nz_part[r+1]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iz+1 < nz) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
	    num_coeffs = diag_i[cnt]-diag_i[cnt-1];
	    num_offd_coeffs = offd_i[cnt]-offd_i[cnt-1];
            cnt++;
	    for (i=1; i < num_fun; i++)
            {
	       diag_i[cnt] = diag_i[cnt-1]+num_coeffs;
	       offd_i[cnt] = offd_i[cnt-1]+num_offd_coeffs;
               cnt++;
            }
         }
      }
   }

   diag_j = hypre_CTAlloc(int, diag_i[local_num_rows]);
   diag_data = hypre_CTAlloc(double, diag_i[local_num_rows]);

   if (num_procs > 1)
   {
      offd_j = hypre_CTAlloc(int, offd_i[local_num_rows]);
      offd_data = hypre_CTAlloc(double, offd_i[local_num_rows]);
   }

   row_index = 0;
   for (iz = nz_part[r]; iz < nz_part[r+1]; iz++)
   {
      for (iy = ny_part[q];  iy < ny_part[q+1]; iy++)
      {
         for (ix = nx_part[p]; ix < nx_part[p+1]; ix++)
         {
            cnt = diag_i[row_index];;
            o_cnt = offd_i[row_index];;
	    num_coeffs = diag_i[row_index+1]-diag_i[row_index];
	    num_offd_coeffs = offd_i[row_index+1]-offd_i[row_index];
            first_j = row_index;
            for (i=0; i < num_fun; i++)
	    {
               for (j=0; j < num_fun; j++)
	       {
                  j_ind = cnt+i*num_coeffs+j;
                  diag_j[j_ind] = first_j+j;
                  diag_data[j_ind] = value[0]*mtrx[i*num_fun+j];
	       }
	    }
            cnt += num_fun;
            if (iz > nz_part[r]) 
            {
               first_j = row_index-nx_local*ny_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[3]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iz) 
               {
                  first_j = num_fun*hypre_map(ix,iy,iz-1,p,q,r-1,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[3]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (iy > ny_part[q]) 
            {
               first_j = row_index-nx_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[2]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iy) 
               {
                  first_j = num_fun*hypre_map(ix,iy-1,iz,p,q-1,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[2]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (ix > nx_part[p]) 
            {
               first_j = row_index-num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[1]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (ix) 
               {
                  first_j = num_fun*hypre_map(ix-1,iy,iz,p-1,q,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[1]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (ix+1 < nx_part[p+1]) 
            {
               first_j = row_index+num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[1]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (ix+1 < nx) 
               {
                  first_j = num_fun*hypre_map(ix+1,iy,iz,p+1,q,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[1]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (iy+1 < ny_part[q+1]) 
            {
               first_j = row_index+nx_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[2]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iy+1 < ny) 
               {
                  first_j = num_fun*hypre_map(ix,iy+1,iz,p,q+1,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[2]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (iz+1 < nz_part[r+1]) 
            {
               first_j = row_index+nx_local*ny_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[3]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iz+1 < nz) 
               {
                  first_j = num_fun*hypre_map(ix,iy,iz+1,p,q,r+1,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[3]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            row_index += num_fun;
         }
      }
   }

   if (num_procs > 1)
   {
         cnt = 0;
         for (i=0; i < local_num_rows; i+=num_fun)
 	 {
	    for (j=offd_i[i]; j < offd_i[i+1]; j++)
            {
               col_map_offd[cnt++] = offd_j[j];
	    }
	 }  
   	
      qsort0(col_map_offd, 0, num_cols_offd-1);

      for (i=0; i < num_fun*num_cols_offd; i++)
         for (j=hypre_min(0,abs(i-num_fun)); j < num_cols_offd; j++)
            if (offd_j[i] == col_map_offd[j])
            {
               offd_j[i] = j;
               break;
            }
   }

   for (i=0; i < num_procs+1; i++)
      global_part[i] *= num_fun;

   for (j=1; j< num_fun; j++)
   {
      for (i=0; i<local_grid_size; i++)
      {
	  row = i*num_fun+j;
	  diag_index = diag_i[row];
	  index = diag_index+j;
	  val = diag_data[diag_index];
	  col = diag_j[diag_index];
	  diag_data[diag_index] = diag_data[index];
	  diag_j[diag_index] = diag_j[index];
	  diag_data[index] = val;
	  diag_j[index] = col;
      }
   }


#ifdef HYPRE_NO_GLOBAL_PARTITION
/* ideally we would use less storage earlier in this function, but this is fine
   for testing */
   {
      int tmp1, tmp2;
      tmp1 = global_part[my_id];
      tmp2 = global_part[my_id + 1];
      hypre_TFree(global_part);
      global_part = hypre_CTAlloc(int, 2);
      global_part[0] = tmp1;
      global_part[1] = tmp2;
   }
#endif

   A = hypre_ParCSRMatrixCreate(comm, num_fun*grid_size, num_fun*grid_size,
                                global_part, global_part, num_cols_offd,
                                diag_i[local_num_rows],
                                offd_i[local_num_rows]);

   hypre_ParCSRMatrixColMapOffd(A) = col_map_offd;

   diag = hypre_ParCSRMatrixDiag(A);
   hypre_CSRMatrixI(diag) = diag_i;
   hypre_CSRMatrixJ(diag) = diag_j;
   hypre_CSRMatrixData(diag) = diag_data;

   offd = hypre_ParCSRMatrixOffd(A);
   hypre_CSRMatrixI(offd) = offd_i;
   if (num_cols_offd)
   {
      hypre_CSRMatrixJ(offd) = offd_j;
      hypre_CSRMatrixData(offd) = offd_data;
   }

   hypre_TFree(nx_part);
   hypre_TFree(ny_part);
   hypre_TFree(nz_part);

   return (HYPRE_ParCSRMatrix) A;
}


/*--------------------------------------------------------------------------
 * hypre_GenerateVectorLaplacian - this is also a systems version and we can
 * vary the diffusion coefficients in each block  
 *--------------------------------------------------------------------------*/

HYPRE_ParCSRMatrix 
GenerateSysLaplacianVCoef( MPI_Comm comm,
                      int      nx,
                      int      ny,
                      int      nz, 
                      int      P,
                      int      Q,
                      int      R,
                      int      p,
                      int      q,
                      int      r,
                      int      num_fun,
                      double  *mtrx,
                      double  *value )
{
   hypre_ParCSRMatrix *A;
   hypre_CSRMatrix *diag;
   hypre_CSRMatrix *offd;

   int    *diag_i;
   int    *diag_j;
   double *diag_data;

   int    *offd_i;
   int    *offd_j;
   double *offd_data;

   int *global_part;
   int ix, iy, iz;
   int cnt, o_cnt;
   int local_num_rows; 
   int *col_map_offd;
   int row_index, row, col;
   int index, diag_index;
   int i,j;

   int nx_local, ny_local, nz_local;
   int nx_size, ny_size, nz_size;
   int num_cols_offd;
   int grid_size;
   int local_grid_size;
   int first_j, j_ind;
   int num_coeffs, num_offd_coeffs;

   int *nx_part;
   int *ny_part;
   int *nz_part;

   int num_procs, my_id;
   int P_busy, Q_busy, R_busy;
   double val;

  /* for indexing in values */
   int sz = num_fun*num_fun;

   MPI_Comm_size(comm,&num_procs);
   MPI_Comm_rank(comm,&my_id);

   grid_size = nx*ny*nz;

   hypre_GeneratePartitioning(nx,P,&nx_part);
   hypre_GeneratePartitioning(ny,Q,&ny_part);
   hypre_GeneratePartitioning(nz,R,&nz_part);

   global_part = hypre_CTAlloc(int,P*Q*R+1);

   global_part[0] = 0;
   cnt = 1;
   for (iz = 0; iz < R; iz++)
   {
      nz_size = nz_part[iz+1]-nz_part[iz];
      for (iy = 0; iy < Q; iy++)
      {
         ny_size = ny_part[iy+1]-ny_part[iy];
         for (ix = 0; ix < P; ix++)
         {
            nx_size = nx_part[ix+1] - nx_part[ix];
            global_part[cnt] = global_part[cnt-1];
            global_part[cnt++] += nx_size*ny_size*nz_size;
         }
      }
   }

   nx_local = nx_part[p+1] - nx_part[p];
   ny_local = ny_part[q+1] - ny_part[q];
   nz_local = nz_part[r+1] - nz_part[r];

   my_id = r*(P*Q) + q*P + p;
   num_procs = P*Q*R;

   local_grid_size = nx_local*ny_local*nz_local;
   local_num_rows = num_fun*local_grid_size;
   diag_i = hypre_CTAlloc(int, local_num_rows+1);
   offd_i = hypre_CTAlloc(int, local_num_rows+1);

   P_busy = hypre_min(nx,P);
   Q_busy = hypre_min(ny,Q);
   R_busy = hypre_min(nz,R);

   num_cols_offd = 0;
   if (p) num_cols_offd += ny_local*nz_local;
   if (p < P_busy-1) num_cols_offd += ny_local*nz_local;
   if (q) num_cols_offd += nx_local*nz_local;
   if (q < Q_busy-1) num_cols_offd += nx_local*nz_local;
   if (r) num_cols_offd += nx_local*ny_local;
   if (r < R_busy-1) num_cols_offd += nx_local*ny_local;
   num_cols_offd *= num_fun;

   if (!local_num_rows) num_cols_offd = 0;

   col_map_offd = hypre_CTAlloc(int, num_cols_offd);

   cnt = 1;
   diag_i[0] = 0;
   offd_i[0] = 0;
   for (iz = nz_part[r]; iz < nz_part[r+1]; iz++)
   {
      for (iy = ny_part[q];  iy < ny_part[q+1]; iy++)
      {
         for (ix = nx_part[p]; ix < nx_part[p+1]; ix++)
         {
            diag_i[cnt] = diag_i[cnt-1];
            offd_i[cnt] = offd_i[cnt-1];
            diag_i[cnt] += num_fun;
            if (iz > nz_part[r]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iz) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
            if (iy > ny_part[q]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iy) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
            if (ix > nx_part[p]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (ix) 
               {
                  offd_i[cnt] += num_fun; 
               }
            }
            if (ix+1 < nx_part[p+1]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (ix+1 < nx) 
               {
                  offd_i[cnt] += num_fun; 
               }
            }
            if (iy+1 < ny_part[q+1]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iy+1 < ny) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
            if (iz+1 < nz_part[r+1]) 
               diag_i[cnt] += num_fun;
            else
            {
               if (iz+1 < nz) 
               {
                  offd_i[cnt] += num_fun;
               }
            }
	    num_coeffs = diag_i[cnt]-diag_i[cnt-1];
	    num_offd_coeffs = offd_i[cnt]-offd_i[cnt-1];
            cnt++;
	    for (i=1; i < num_fun; i++)
            {
	       diag_i[cnt] = diag_i[cnt-1]+num_coeffs;
	       offd_i[cnt] = offd_i[cnt-1]+num_offd_coeffs;
               cnt++;
            }
         }
      }
   }

   diag_j = hypre_CTAlloc(int, diag_i[local_num_rows]);
   diag_data = hypre_CTAlloc(double, diag_i[local_num_rows]);

   if (num_procs > 1)
   {
      offd_j = hypre_CTAlloc(int, offd_i[local_num_rows]);
      offd_data = hypre_CTAlloc(double, offd_i[local_num_rows]);
   }

   row_index = 0;
   for (iz = nz_part[r]; iz < nz_part[r+1]; iz++)
   {
      for (iy = ny_part[q];  iy < ny_part[q+1]; iy++)
      {
         for (ix = nx_part[p]; ix < nx_part[p+1]; ix++)
         {
            cnt = diag_i[row_index];;
            o_cnt = offd_i[row_index];;
	    num_coeffs = diag_i[row_index+1]-diag_i[row_index];
	    num_offd_coeffs = offd_i[row_index+1]-offd_i[row_index];
            first_j = row_index;
            for (i=0; i < num_fun; i++)
	    {
               for (j=0; j < num_fun; j++)
	       {
                  j_ind = cnt+i*num_coeffs+j;
                  diag_j[j_ind] = first_j+j;
                  diag_data[j_ind] = value[0*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	       }
	    }
            cnt += num_fun;
            if (iz > nz_part[r]) 
            {
               first_j = row_index-nx_local*ny_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[3*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iz) 
               {
                  first_j = num_fun*hypre_map(ix,iy,iz-1,p,q,r-1,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[3*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (iy > ny_part[q]) 
            {
               first_j = row_index-nx_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[2*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iy) 
               {
                  first_j = num_fun*hypre_map(ix,iy-1,iz,p,q-1,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[2*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (ix > nx_part[p]) 
            {
               first_j = row_index-num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[1*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (ix) 
               {
                  first_j = num_fun*hypre_map(ix-1,iy,iz,p-1,q,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[1*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (ix+1 < nx_part[p+1]) 
            {
               first_j = row_index+num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[1*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (ix+1 < nx) 
               {
                  first_j = num_fun*hypre_map(ix+1,iy,iz,p+1,q,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[1*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (iy+1 < ny_part[q+1]) 
            {
               first_j = row_index+nx_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[2*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iy+1 < ny) 
               {
                  first_j = num_fun*hypre_map(ix,iy+1,iz,p,q+1,r,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[2*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            if (iz+1 < nz_part[r+1]) 
            {
               first_j = row_index+nx_local*ny_local*num_fun;
               for (i=0; i < num_fun; i++)
	       {
                  for (j=0; j < num_fun; j++)
	          {
                     j_ind = cnt+i*num_coeffs+j;
                     diag_j[j_ind] = first_j+j;
                     diag_data[j_ind] = value[3*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	          }
	       }
               cnt += num_fun;
            }
            else
            {
               if (iz+1 < nz) 
               {
                  first_j = num_fun*hypre_map(ix,iy,iz+1,p,q,r+1,P,Q,R,
                                      nx_part,ny_part,nz_part,global_part);
                  for (i=0; i < num_fun; i++)
	          {
                     for (j=0; j < num_fun; j++)
	             {
                        j_ind = o_cnt+i*num_offd_coeffs+j;
                        offd_j[j_ind] = first_j+j;
                        offd_data[j_ind] = value[3*sz + i*num_fun+j]*mtrx[i*num_fun+j];
	             }
	          }
                  o_cnt += num_fun;
               }
            }
            row_index += num_fun;
         }
      }
   }

   if (num_procs > 1)
   {
         cnt = 0;
         for (i=0; i < local_num_rows; i+=num_fun)
 	 {
	    for (j=offd_i[i]; j < offd_i[i+1]; j++)
            {
               col_map_offd[cnt++] = offd_j[j];
	    }
	 }  
   	
      qsort0(col_map_offd, 0, num_cols_offd-1);

      for (i=0; i < num_fun*num_cols_offd; i++)
         for (j=hypre_min(0,abs(i-num_fun)); j < num_cols_offd; j++)
            if (offd_j[i] == col_map_offd[j])
            {
               offd_j[i] = j;
               break;
            }
   }

   for (i=0; i < num_procs+1; i++)
      global_part[i] *= num_fun;

   for (j=1; j< num_fun; j++)
   {
      for (i=0; i<local_grid_size; i++)
      {
	  row = i*num_fun+j;
	  diag_index = diag_i[row];
	  index = diag_index+j;
	  val = diag_data[diag_index];
	  col = diag_j[diag_index];
	  diag_data[diag_index] = diag_data[index];
	  diag_j[diag_index] = diag_j[index];
	  diag_data[index] = val;
	  diag_j[index] = col;
      }
   }


#ifdef HYPRE_NO_GLOBAL_PARTITION
/* ideally we would use less storage earlier in this function, but this is fine
   for testing */
   {
      int tmp1, tmp2;
      tmp1 = global_part[my_id];
      tmp2 = global_part[my_id + 1];
      hypre_TFree(global_part);
      global_part = hypre_CTAlloc(int, 2);
      global_part[0] = tmp1;
      global_part[1] = tmp2;
   }
#endif

   A = hypre_ParCSRMatrixCreate(comm, num_fun*grid_size, num_fun*grid_size,
                                global_part, global_part, num_cols_offd,
                                diag_i[local_num_rows],
                                offd_i[local_num_rows]);

   hypre_ParCSRMatrixColMapOffd(A) = col_map_offd;

   diag = hypre_ParCSRMatrixDiag(A);
   hypre_CSRMatrixI(diag) = diag_i;
   hypre_CSRMatrixJ(diag) = diag_j;
   hypre_CSRMatrixData(diag) = diag_data;

   offd = hypre_ParCSRMatrixOffd(A);
   hypre_CSRMatrixI(offd) = offd_i;
   if (num_cols_offd)
   {
      hypre_CSRMatrixJ(offd) = offd_j;
      hypre_CSRMatrixData(offd) = offd_data;
   }

   hypre_TFree(nx_part);
   hypre_TFree(ny_part);
   hypre_TFree(nz_part);

   return (HYPRE_ParCSRMatrix) A;
}
