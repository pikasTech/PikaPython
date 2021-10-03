/*-----------------------------------------------------------------------------
 * Name:    Device.h
 * Purpose: Include the correct device header file
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2011 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#ifndef __DEVICE_H
#define __DEVICE_H

#if   defined CMSDK_CM0
  #include "CMSDK_CM0.h"                         /* device specific header file */
#elif defined CMSDK_CM0plus
  #include "CMSDK_CM0plus.h"                     /* device specific header file */
#elif defined CMSDK_CM3
  #include "CMSDK_CM3.h"                         /* device specific header file */
#elif defined CMSDK_CM4
  #include "CMSDK_CM4.h"                         /* device specific header file */
#elif defined CMSDK_CM4_FP
  #include "CMSDK_CM4_FP.h"                      /* device specific header file */
#elif defined CMSDK_CM7
  #include "CMSDK_CM7.h"                         /* device specific header file */
#elif defined CMSDK_CM7_SP
  #include "CMSDK_CM7_SP.h"                      /* device specific header file */
#elif defined CMSDK_CM7_DP
  #include "CMSDK_CM7_DP.h"                      /* device specific header file */
#elif defined CMSDK_ARMv8MBL
  #include "CMSDK_ARMv8MBL.h"                    /* device specific header file */
#elif defined CMSDK_ARMv8MML
  #include "CMSDK_ARMv8MML.h"                    /* device specific header file */
#elif defined CMSDK_ARMv8MML_SP
  #include "CMSDK_ARMv8MML_SP.h"                 /* device specific header file */
#elif defined CMSDK_ARMv8MML_DP
  #include "CMSDK_ARMv8MML_DP.h"                 /* device specific header file */

#else
  #warning "no appropriate header file found!"
#endif

#endif /* __DEVICE_H */
