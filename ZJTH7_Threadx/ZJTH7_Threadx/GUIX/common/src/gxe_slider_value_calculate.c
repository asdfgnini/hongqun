/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Component                                                        */
/**                                                                       */
/**   Slider Management (Slider)                                          */
/**                                                                       */
/**************************************************************************/

#define GX_SOURCE_CODE

/* Include necessary system files.  */

#include "gx_api.h"
#include "gx_slider.h"

/* Bring in externs for caller checking code.  */
GX_CALLER_CHECKING_EXTERNS

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_slider_value_calculate                         PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in slider value calculate call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    slider                                Slider widget control block   */
/*    info                                  Pointer to slider info        */
/*    new_position                          New slider position           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  _gx_slider_value_calculate              Actual slider value calculate */
/*                                            call                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Kenneth Maxwell          Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
UINT  _gxe_slider_value_calculate(GX_SLIDER *slider, GX_SLIDER_INFO *info, INT new_position)
{
UINT status;

    /* Check for invalid caller.  */
    GX_INIT_AND_THREADS_CALLER_CHECKING

    /* Check for invalid pointer.  */
    if ((slider == GX_NULL) || (info == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    /* Check for invalid widget.  */
    if (slider -> gx_widget_type == 0)
    {
        return(GX_INVALID_WIDGET);
    }

    /* Check for invalid slider info.  */
    if ((info -> gx_slider_info_current_val < info -> gx_slider_info_min_val) ||
        (info -> gx_slider_info_current_val > info -> gx_slider_info_max_val) ||
        (info -> gx_slider_info_min_val >= info -> gx_slider_info_max_val))
    {
        return(GX_INVALID_VALUE);
    }

    /* Call actual slider value caculate function.  */
    status = _gx_slider_value_calculate(slider, info, new_position);

    return(status);
}

