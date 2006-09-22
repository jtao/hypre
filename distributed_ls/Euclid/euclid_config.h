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



#ifndef EUCLID_CONF_DH
#define EUCLID_CONF_DH

#define MAX_MPI_TASKS 500

#define PETSC_EUCLID_ERR_CODE  PETSC_ERR_LIB

/* for use in printTriples functions */
#define TRIPLES_FORMAT    "%i %i %1.8e\n"
/* #define TRIPLES_FORMAT    "%i %i %1.19e\n" */

#undef PRIVATE_TIMING_DH
  /* primarily for experimental purposes; if defined, TimeLog_dh
     marks are entered in Mat_dh and Euclid_apply modules.
   */


  /* top-level error handlers. redefine to do what you want, or don't
     use it at all.  Intended usage for calling Euclid functions from
     main is:

         Euclid_dhPhoo(); ERRCHKA;
  */

#ifdef USING_MPI
#define EUCLID_EXIT MPI_Abort(comm_dh, -1)
#else
#define EUCLID_EXIT exit(-1);
#endif

#define EXIT_NOW(msg) \
      { setError_dh(msg, __FUNC__, __FILE__, __LINE__); \
        ERRCHKA; \
      } 

#define ERRCHKA   \
    if (errFlag_dh) {  \
      setError_dh("", __FUNC__, __FILE__, __LINE__); \
      if (logFile != NULL) {  \
        printErrorMsg(logFile);  \
        closeLogfile_dh();  \
      } \
      printErrorMsg(stderr);  \
      if (myid_dh == 0) { \
        Mem_dhPrint(mem_dh, stderr, false); \
      } \
      EUCLID_EXIT; \
    } 


  /* let Euclid do its thing, before handing off to PETSc;
     this is only used in main(), after a call to a function
     from the PETSc library.
   */
#define ERRCHKA_CHKERRA(ierr)   \
    if (errFlag_dh) {  \
      setError_dh("", __FUNC__, __FILE__, __LINE__); \
      if (logFile != NULL) {  \
        printErrorMsg(logFile);  \
        fprintf(logFile, "\n[%i] ierr = %i, errFlag_dh = %i\n", myid_dh, ierr, errFlag_dh); \
        closeLogfile_dh();  \
      } \
      printErrorMsg(stderr);  \
      fprintf(stderr, "\n[%i] ierr = %i, errFlag_dh = %i\n", myid_dh, ierr, errFlag_dh); \
      CHKERRA(ierr); \
    } 


#define MAX_SUBDOMAINS  20   
  /* The maximum number of subdomains into which
     the matrix may be partitioned.  Rule of thumb:
     MAX_SUBDOMAINS >= number of threads.

     Note: this is only for shared-memory.
   */


#define PIVOT_FIX_DEFAULT  1e-3

/*---------------------------------------------------------------------
 * Memory management.  These macros work with functions in Mem_dh.c;
 * Change if you want to use some memory management and reporting schemes 
 * other than that supplied with Euclid.   These depend on the global
 * object "Mem_dh mem_dh" which is defined in globalObjects.c (yuck!)
 ---------------------------------------------------------------------*/

#undef USE_PETSC_MALLOC

#ifdef USE_PETSC_MALLOC
#define MALLOC_DH(s)  PetscMalloc(s)
#define FREE_DH(p)    PetscFree(p)

#else
#define MALLOC_DH(s)  Mem_dhMalloc(mem_dh, (s))
#define FREE_DH(p)    Mem_dhFree(mem_dh, p)
#endif


  /* The actual calls used by Mem_dh objects to allocate/free memory 
   * from the heap.
   */
#define PRIVATE_MALLOC  malloc
#define PRIVATE_FREE    free

/*------------------ Memory management end -----------------------------*/

/*
Developer's note: there must be a XXX_GET_ROW defined, e.g:

  #define HYPRE_GET_ROW
  #define PETSC_GET_ROW
  #define EUCLID_GET_ROW

This is currently accomplished in the makefile system;
If you're building an interface to a solver package,
you need to write EUCLID_GET_ROW() functions: see src/getRow.c
*/

#endif
