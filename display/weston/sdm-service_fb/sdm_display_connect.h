/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted
* provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright notice, this list of
*      conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright notice, this list of
*      conditions and the following disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its contributors may be used to
*      endorse or promote products derived from this software without specific prior written
*      permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SDM_DISPLAY_CONNECT_H
#define SDM_DISPLAY_CONNECT_H
#include "sdm_display_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Method to create and internally store the handle to display core interface.

    @details This method is the entry point into the display core. Client can create
    and operate on different display devices only after valid interface handle is
    internally stored after invoking this method. Successfully obtaining valid interface
    handle and storing internally is indicated by return code kErrorNone. An object of
    display core is created and handle to this object returned internally by the display
    core interface via output parameter is internally stored. This interface shall be
    called only once. Parameters used in this method internally and not exposed to Client
    are noted below for information purposes only.

    @param[in] void \endlink
    @return \link DisplayError \endlink

    @sa DestroyCore
*/
int CreateCore();

/*! @brief Method to release internally stored handle to display core interface.

    @details The object of corresponding display core is destroyed when this method is
    invoked and internally stored handle is released. Client must explicitly destroy
    all created display device objects associated with this handle before invoking this
    method even though this method makes a modest effort to destroy all existing and
    created display device objects.

    @param[in] interface (internally used by this method) \link CoreInterface \endlink

    @return \link DisplayError \endlink

    @sa CreateCore
*/
int DestroyCore();


/*! @brief Method to get characteristics of the first display.

    @details Client shall use this method to determine type of the first display.

    @param[in] display_id that this method will fill up with info.

    @return \link DisplayError \endlink

*/
int GetFirstDisplayType(int *display_id);


/*! @brief Method to create a display device for a given display id.

    @details Client shall use this method to create each of the display id.
    display_id must be valid to create display.

    @param[in] display_id \link int \endlink
    @return \link DisplayError \endlink

    @sa DestroyDisplay
*/
int CreateDisplay(int display_id);

/*! @brief Method to destroy a display device.

    @details Client shall use this method to destroy each of the created
    display device objects.

    @param[in] display_id \link int \endlink

    @return \link DisplayError \endlink

    @sa CreateDisplay
*/
int DestroyDisplay(int display_id);

/*! @brief Method to commit layers of a frame submitted in a former call to Prepare().

    @details Client shall call this method to submit layers for final composition.
    The composed output shall be displayed on the panel or written in output buffer.

    This method shall be called only once for each frame.

    In the event of an error as well, this call will cause any fences returned in the
    previous call to Commit() to eventually become signaled, so the client's wait on
    fences can be released to prevent deadlocks.

    @param[in] display_id \link int \endlink
    @param[inout] drm_output \link struct drm_output \endlink

    @return \link DisplayError \endlink

    @sa Prepare
*/
int Commit(int display_id,int fd);

void SetLineLength(int line_length);



#ifdef __cplusplus
}
#endif

#endif // SDM_DISPLAY_CONNECT_H
