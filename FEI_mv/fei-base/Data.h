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



#ifndef _Data_h_
#define _Data_h_

#include <string.h>
#include <stdlib.h>

/**
  This is a very simple class for passing stuff around
  in a void pointer. It has the ability to store and query
  a type name, so at least there can be user-enforced
  type safety.

  When setTypeName is called, a char* is created and a copy
  of the input argument is taken. This char* is later destroyed
  by the Data destructor. The void* dataPtr_ member is not
  destroyed, it is just a copy of a pointer.
*/

class Data {
 public:
  /** Default constructor. */
   Data() {typeName_ = NULL; dataPtr_ = NULL;};

   /** Default destructor. */
   virtual ~Data() {if (typeName_) delete [] typeName_;};

   /** Set a string representing the type of the object stored in
       'getDataPtr()'. */
   void setTypeName(const char* name) {if (typeName_) delete [] typeName_;
                                 int len = strlen(name);
                                 typeName_ = new char[len+1];
                                 strcpy(typeName_, name);
                                 typeName_[len] = '\0';};

   /** Query the string representing the type of the object stored in
       'getDataPtr()'. */
   char* getTypeName() const {return(typeName_);};

   /** Set the contents of the data pointer. */
   void setDataPtr(void* ptr) {dataPtr_ = ptr;};

  /** Retrieve the contents of the data pointer. */
   void* getDataPtr() const {return(dataPtr_);};

 private:
   char* typeName_;
   void* dataPtr_;
};

#endif

