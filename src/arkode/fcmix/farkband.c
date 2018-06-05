/*---------------------------------------------------------------
 * Programmer(s): Daniel R. Reynolds @ SMU
 *---------------------------------------------------------------
 * LLNS/SMU Copyright Start
 * Copyright (c) 2017, Southern Methodist University and
 * Lawrence Livermore National Security
 *
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Southern Methodist University and Lawrence Livermore
 * National Laboratory under Contract DE-AC52-07NA27344.
 * Produced at Southern Methodist University and the Lawrence
 * Livermore National Laboratory.
 *
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS/SMU Copyright End
 *---------------------------------------------------------------
 * Fortran/C interface routines for ARKODE/ARKDLS, for the case
 * of a user-supplied Jacobian approximation routine.
 *--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "farkode.h"
#include "arkode_impl.h"
#include <arkode/arkode_direct.h>
#include <sunmatrix/sunmatrix_band.h>


/*=============================================================*/

/* Prototype of the Fortran routine */

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

  extern void FARK_BJAC(long int *N, long int *MU,
                        long int *ML, long int *EBAND,
                        realtype *T, realtype *Y, realtype *FY,
  			realtype *BJAC, realtype *H,
  			long int *IPAR, realtype *RPAR,
  			realtype *V1, realtype *V2,
  			realtype *V3, int *IER);

#ifdef __cplusplus
}
#endif

/*=============================================================*/

/* Fortran interface routine to ARKDlsSetJacFn; see farkode.h
   for further details */
void FARK_BANDSETJAC(int *flag, int *ier)
{
  if (*flag == 0) {
    *ier = ARKDlsSetJacFn(ARK_arkodemem, NULL);
  } else {
    *ier = ARKDlsSetJacFn(ARK_arkodemem, FARKBandJac);
  }
  return;
}

/*=============================================================*/

/* C interface to user-supplied Fortran subroutine FARKBJAC; see
   farkode.h for further details */
int FARKBandJac(realtype t, N_Vector y, N_Vector fy, SUNMatrix J,
                void *user_data, N_Vector vtemp1, N_Vector vtemp2,
		N_Vector vtemp3)
{
  realtype *ydata, *fydata, *jacdata, *v1data, *v2data, *v3data;
  realtype h;
  long int N, mupper, mlower, smu, eband;
  FARKUserData ARK_userdata;
  int ier = 0;

  ARKodeGetLastStep(ARK_arkodemem, &h);
  ydata   = N_VGetArrayPointer(y);
  fydata  = N_VGetArrayPointer(fy);
  v1data  = N_VGetArrayPointer(vtemp1);
  v2data  = N_VGetArrayPointer(vtemp2);
  v3data  = N_VGetArrayPointer(vtemp3);
  N = SUNBandMatrix_Columns(J);
  mupper = SUNBandMatrix_UpperBandwidth(J);
  mlower = SUNBandMatrix_LowerBandwidth(J);
  smu = SUNBandMatrix_StoredUpperBandwidth(J);
  eband   = smu + mlower + 1;
  jacdata = SUNBandMatrix_Column(J,0) - mupper;
  ARK_userdata = (FARKUserData) user_data;

  FARK_BJAC(&N, &mupper, &mlower, &eband, &t, ydata, fydata,
	    jacdata, &h, ARK_userdata->ipar, ARK_userdata->rpar,
	    v1data, v2data, v3data, &ier);
  return(ier);
}

/*===============================================================
   EOF
===============================================================*/
