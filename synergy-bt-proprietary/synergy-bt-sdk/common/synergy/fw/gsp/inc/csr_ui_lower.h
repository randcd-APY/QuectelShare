#ifndef CSR_UI_LOWER_H__
#define CSR_UI_LOWER_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************************/
/********* Input *********/
/*************************/

/* These functions are implemented in CSR_UI and can be called by some
   external entity, from a context that allows calling of CsrPmemAlloc and
   CsrSchedMessagePut. */

/* Call to signal a touch on a particular pixel within the extends of the
   display. */
void CsrUiTouchEvent(CsrInt16 x, CsrInt16 y);

/* Call to signal activation of a particular key. Valid parameter values are
   any of the CSR_UI_KEY_ values defined in csr_ui_keycode.h */
void CsrUiKeyEvent(CsrUint16 key);


/******************************************/
/********* Touch Key Registration *********/
/******************************************/

/* Call to map a key to an area of the touch screen. The key is registered as a
   rectangle with the given coordinates. Valid parameter values for key are any
   of the CSR_UI_KEY_ values defined in csr_ui_keycode.h. This function can be
   called from any context that allows calling of CsrPmemAlloc. */
void CsrUiRegisterKey(CsrInt16 left, CsrInt16 top, CsrInt16 right, CsrInt16 bottom, CsrUint16 key);


/*************************************/
/********* Graphics Renderer *********/
/*************************************/

typedef struct
{
    /* Set to TRUE to activate the Graphics Renderer */
    CsrBool active;
    /* Top edge of screen (in pixel coordinates) */
    CsrUint16 top;
    /* Left edge of screen (in pixel coordinates) */
    CsrUint16 left;
    /* Called by CSR_UI to paint a particular pixel with a specified colour */
    void (*setPixel)(CsrInt16 x, CsrInt16 y, CsrUint32 colour);
    /* Called by CSR_UI to paint a range of pixels with the specified colours */
    void (*drawBox)(CsrInt16 left, CsrInt16 top, CsrInt16 right, CsrInt16 bottom, CsrUint32 outlineColor, CsrUint32 fillColor, CsrBool fill);
    /* Called by CSR_UI before a redraw of the display is started */
    void (*lock)(void);
    /* Called by CSR_UI after a redraw of the display completes */
    void (*unlock)(void);
} CsrUiGraphicsRenderer;

/* Called by CSR_UI during initialisation to retrieve information about the
   Graphics Renderer interface. This function is not implemented by CSR_UI, and
   shall be provided by some external entity. If graphics rendering is desired,
   the fields of the renderer struct must be filled with appropriate values. If
   no graphics rendering is desired, the active field is set to FALSE, and the
   remaining fields can be left unassigned. */
void CsrUiGraphicsRendererConfigurationGet(CsrUiGraphicsRenderer *renderer);

/*  Colour is encoded as an unsigned 32 bit integer:

    0xAABBGGRR

    Where:
    AA: 0-0xFF, Alpha - optional
    BB: 0-0xFF, Blue
    GG: 0-0xFF, Green
    RR: 0-0xFF, Red */


/*********************************/
/********* Text Renderer *********/
/*********************************/

#define CSR_UI_TEXT_STATUS_CHARACTERS     80
#define CSR_UI_TEXT_CANVAS_LINES          30
#define CSR_UI_TEXT_CANVAS_CHARACTERS     80
#define CSR_UI_TEXT_SOFTKEY1_CHARACTERS   30
#define CSR_UI_TEXT_SOFTKEY2_CHARACTERS   30

typedef struct
{
    /* Set to TRUE to activate the Text Renderer. */
    CsrBool active;
    /* Called by CSR_UI to update the text display with new strings */
    void (*setText)(CsrUint16 *status, CsrUint16 *canvas, CsrUint16 *softkey1, CsrUint16 *softkey2);
} CsrUiTextRenderer;

/* Called by CSR_UI during initialisation to retrieve information about the Text
   Renderer interface. This function is not implemented by CSR_UI, and shall be
   provided by some external entity. If text rendering is desired, the fields of
   the renderer struct must be filled with appropriate values. If no text
   rendering is desired, the active field is set to FALSE, and the remaining
   fields can be left unassigned. */
void CsrUiTextRendererConfigurationGet(CsrUiTextRenderer *renderer);

#ifdef __cplusplus
}
#endif

#endif
