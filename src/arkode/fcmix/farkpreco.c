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
 * The C functions FARKPSet and FARKPSol are to interface between
 * the ARKSPILS module and the user-supplied preconditioner
 * setup/solve routines FARKPSET and FARKPSOL. Note the use of
 * the generic names FARK_PSET and FARK_PSOL in the code below.
 *--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "farkode.h"
#include "arkode_impl.h"
#include <arkode/arkode_spils.h>

/*=============================================================*/

/* Prototype of the Fortran routines */

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

  extern void FARK_PSET(realtype *T, realtype *Y, realtype *FY,
			booleantype *JOK, booleantype *JCUR,
			realtype *GAMMA, realtype *H,
			long int *IPAR, realtype *RPAR, int *IER);
  extern void FARK_PSOL(realtype *T, realtype *Y, realtype *FY,
			realtype *R, realtype *Z,
			realtype *GAMMA, realtype *DELTA,
			int *LR, long int *IPAR, realtype *RPAR,
                        int *IER);

#ifdef __cplusplus
}
#endif

/*=============================================================*/

/* Fortran interface to C routine ARKSpilsSetPreconditioner; see
   farkode.h for further details */
void FARK_SPILSSETPREC(int *flag, int *ier)
{
  if (*flag == 0) {
    *ier = ARKSpilsSetPreconditioner(ARK_arkodemem, NULL, NULL);
  } else {
    *ier = ARKSpilsSetPreconditioner(ARK_arkodemem,
				     FARKPSet, FARKPSol);
  }
  return;
}

/*=============================================================*/

/* C interface to user-supplied Fortran routine FARKPSET; see
   farkode.h for further details */
int FARKPSet(realtype t, N_Vector y, N_Vector fy,
	     booleantype jok, booleantype *jcurPtr,
	     realtype gamma, void *user_data)
{
  int ier = 0;
  realtype *ydata, *fydata;
  realtype h;
  FARKUserData ARK_userdata;

  ARKodeGetLastStep(ARK_arkodemem, &h);
  ydata  = N_VGetArrayPointer(y);
  fydata = N_VGetArrayPointer(fy);
  ARK_userdata = (FARKUserData) user_data;

  FARK_PSET(&t, ydata, fydata, &jok, jcurPtr, &gamma, &h,
	    ARK_userdata->ipar, ARK_userdata->rpar, &ier);
  return(ier);
}


/*=============================================================*/

/* C interface to user-supplied Fortran routine FARKPSOL; see
   farkode.h for further details */
int FARKPSol(realtype t, N_Vector y, N_Vector fy, N_Vector r,
	     N_Vector z, realtype gamma, realtype delta,
	     int lr, void *user_data)
{
  int ier = 0;
  realtype *ydata, *fydata, *rdata, *zdata;
  FARKUserData ARK_userdata;

  ydata  = N_VGetArrayPointer(y);
  fydata = N_VGetArrayPointer(fy);
  rdata  = N_VGetArrayPointer(r);
  zdata  = N_VGetArrayPointer(z);
  ARK_userdata = (FARKUserData) user_data;

  FARK_PSOL(&t, ydata, fydata, rdata, zdata, &gamma, &delta, &lr,
	    ARK_userdata->ipar, ARK_userdata->rpar, &ier);
  return(ier);
}

/*===============================================================
   EOF
===============================================================*/
