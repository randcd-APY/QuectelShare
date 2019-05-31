/*
* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

/*! @brief Method to create a display device for a given display id even if
    display was already created. This method forces the display to be re-created
    after destroying the one which was previously created, if so.

    @details Client shall use this method to force create each of the display id.
    display_id must be valid to create display.

    @param[in] display_id \link int \endlink
    @return \link DisplayError \endlink

    @sa DestroyDisplay
*/
int ReconfigureDisplay(int display_id);

/*! @brief Method to compose layers associated with given frame.

    @details Client shall send all layers associated with a frame
    targeted for current display using this method and check the layers
    which can be handled completely in display manager.

    This method can be called multiple times but only last call prevails. This method must be
    followed by Commit().

    @param[in] display_id \link int \endlink
    @param[in] drm_output \link struct drm_output \endlink

    @return \link DisplayError \endlink

    @sa Commit
*/
int Prepare(int display_id, struct drm_output *output);

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
int Commit(int display_id, struct drm_output *output);

/*! @brief Method to obtain display property for a display_id requested.
    @details Client shall use this method to display properties of requested
    display id.

    @param[in] display_id \link int \endlink
    @param[in] display_config \link struct DisplayConfigInfo \endlink

    @return \link DisplayError \endlink

    @sa
*/
bool GetDisplayConfiguration(int display_id, struct DisplayConfigInfo *display_config);

/*! @brief Method to obtain display's HDR information parameters for requested display_id.
    @details Client shall use this method to obtain display's HDR capability parameters
    for requested display_id.

    @param[in] display_id \link int \endlink
    @param[in] display_config \link struct DisplayHdrInfo \endlink

    @return \link DisplayError \endlink

    @sa
*/
bool GetDisplayHdrInfo(int display_id, struct DisplayHdrInfo *display_hdr_info);

/*! @brief Method to obtain display's HDCP information parameters for requested display_id.
    @details Client shall use this method to obtain display's HDCP capability parameters
    for requested display_id.

    @param[in] display_id \link int \endlink
    @param[in] display_config \link struct DisplayHdcpProtocol \endlink

    @return \link bool \endlink

    @sa
*/
bool GetDisplayHdcpProtocol(int display_id, struct DisplayHdcpProtocol *display_hdcp_protocol);

/*! @brief Method to register callbacks: VBlank Handler function to be called on
    enabling VBlank (VSync), and hotplug handler function to be called on hotplug
    uevent. SDM shall trigger a call back through this interface function.

    @param[in] display_id \link int \endlink
    @param[in] cbs \link sdm_cbs_t \endlink

    @return \link DisplayError \endlink

    @sa
*/

int RegisterCbs(int display_id, sdm_cbs_t *cbs);

/*! @brief Method to turn on power of display

    @details Client shall use this method to turn on display. DisplayError
    must have been created previously.
    to be called by hardware composer.

    @param[in] display_id \link int \endlink
    @param[in] power_mode \link int \endlink

    @return \link int \endlink

    @sa
*/
bool SetDisplayState(int display_id, int power_mode);

/*! @brief Method to enable VSync State, i.e. whether to generate callback
    on next frame.

    @details Client shall use this method for enable VSync (VBlank) callback.

    @param[in] display_id \link int \endlink
    @param[in] enable \link bool \endlink
    @param[in] output \link (struct drm_output *) \endlink

    @return \link int \endlink

    @sa
*/
int SetVSyncState(int display_id, bool enable, struct drm_output *output);

/*! @brief Method for obtaining master fd.

    @details client to obtaining master fd.

    @return \link int \endlink

    @sa
*/
int get_drm_master_fd();

/*! @brief Method for enable PLL update function.

    @details enable or disable PLL update function.

    @param[in] display_id \link int \endlink

    @param[in] enable \link int \endlink

    @return \link int \endlink

    @sa
*/
int EnablePllUpdate(int display_id, int enable);

/*! @brief Method for update display PLL.

    @details update display PLL by ppm.

    @param[in] display_id \link int \endlink

    @param[in] ppm \link int \endlink

    @return \link int \endlink

    @sa
*/
int UpdateDisplayPll(int display_id, int ppm);

#ifdef __cplusplus
}
#endif

#endif // SDM_DISPLAY_CONNECT_H
