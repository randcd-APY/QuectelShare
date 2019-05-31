/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#ifdef BSP_SUPPORT_SDL
#include <SDL.h>
#endif

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_unicode.h"
#include "csr_macro.h"

#include "csr_ui_lower.h"
#include "csr_ui_keycode.h"

#include "csr_sched_init.h"

#ifdef BSP_SUPPORT_SDL
#include "csr_phone_skin.h"

#define DISPLAY_HEIGHT                             640
#define DISPLAY_WIDTH                              480

/* The offset of the phone */
#define PHONE_X_OFFSET                             (DISPLAY_WIDTH / 2 - PHONE_SKIN_WIDTH / 2)
#define PHONE_Y_OFFSET                             (0)

/* The size of the screen in the phone */
#define PHONE_SCREEN_WIDTH                         (256)
#define PHONE_SCREEN_HEIGHT                        (320)

/* The offset of the screen */
#define PHONE_SCREEN_X_OFFSET                      (PHONE_X_OFFSET + 32)
#define PHONE_SCREEN_Y_OFFSET                      (PHONE_Y_OFFSET + 57)

/* The SK1, SK2, BACK and DEL keys */
#define PHONE_SK_X_OFFSET                          (PHONE_SCREEN_X_OFFSET + 1)
#define PHONE_SK_Y_OFFSET                          (PHONE_SCREEN_HEIGHT + PHONE_SCREEN_Y_OFFSET + 17)

#define PHONE_SK_HEIGHT                            (26)
#define PHONE_SK_WIDTH                             (82)
#define PHONE_SK_SPACE_BETWEEN_COLS                (94)
#define PHONE_SK_SPACE_BETWEEN_ROWS                (12)

#define PHONE_SK_ROW_ONE_TOP                       (PHONE_SK_Y_OFFSET)
#define PHONE_SK_ROW_ONE_BOTTOM                    (PHONE_SK_Y_OFFSET + PHONE_SK_HEIGHT)
#define PHONE_SK_ROW_TWO_TOP                       (PHONE_SK_Y_OFFSET + PHONE_SK_HEIGHT + PHONE_SK_SPACE_BETWEEN_ROWS)
#define PHONE_SK_ROW_TWO_BOTTOM                    (PHONE_SK_Y_OFFSET + PHONE_SK_HEIGHT * 2 + PHONE_SK_SPACE_BETWEEN_ROWS)

#define PHONE_SK_COL_ONE_LEFT                      (PHONE_SK_X_OFFSET)
#define PHONE_SK_COL_ONE_RIGHT                     (PHONE_SK_X_OFFSET + PHONE_SK_WIDTH)
#define PHONE_SK_COL_TWO_LEFT                      (PHONE_SK_X_OFFSET + PHONE_SK_WIDTH + PHONE_SK_SPACE_BETWEEN_COLS)
#define PHONE_SK_COL_TWO_RIGHT                     (PHONE_SK_X_OFFSET + PHONE_SK_WIDTH * 2 + PHONE_SK_SPACE_BETWEEN_COLS)

/* The navigation keys */
#define PHONE_NAVIGATION_LEFT_KEY_TOP_X_OFFSET     (PHONE_SK_COL_ONE_RIGHT)
#define PHONE_NAVIGATION_LEFT_KEY_TOP_Y_OFFSET     (PHONE_SK_ROW_ONE_TOP - 3)
#define PHONE_NAVIGATION_LEFT_KEY_BOTTOM_X_OFFSET  (PHONE_NAVIGATION_LEFT_KEY_TOP_X_OFFSET + 28)
#define PHONE_NAVIGATION_LEFT_KEY_BOTTOM_Y_OFFSET  (PHONE_SK_ROW_TWO_BOTTOM + 8)

#define PHONE_NAVIGATION_UP_KEY_TOP_X_OFFSET       (PHONE_NAVIGATION_LEFT_KEY_BOTTOM_X_OFFSET)
#define PHONE_NAVIGATION_UP_KEY_TOP_Y_OFFSET       (PHONE_NAVIGATION_LEFT_KEY_TOP_Y_OFFSET)
#define PHONE_NAVIGATION_UP_KEY_BOTTOM_X_OFFSET    (PHONE_NAVIGATION_UP_KEY_TOP_X_OFFSET + 36)
#define PHONE_NAVIGATION_UP_KEY_BOTTOM_Y_OFFSET    (PHONE_NAVIGATION_UP_KEY_TOP_Y_OFFSET + 37)

#define PHONE_NAVIGATION_RIGHT_KEY_TOP_X_OFFSET    (PHONE_NAVIGATION_UP_KEY_BOTTOM_X_OFFSET)
#define PHONE_NAVIGATION_RIGHT_KEY_TOP_Y_OFFSET    (PHONE_SK_ROW_ONE_TOP - 3)
#define PHONE_NAVIGATION_RIGHT_KEY_BOTTOM_X_OFFSET (PHONE_NAVIGATION_RIGHT_KEY_TOP_X_OFFSET + 28)
#define PHONE_NAVIGATION_RIGHT_KEY_BOTTOM_Y_OFFSET (PHONE_SK_ROW_TWO_BOTTOM + 8)

#define PHONE_NAVIGATION_DOWN_KEY_TOP_X_OFFSET     (PHONE_NAVIGATION_LEFT_KEY_TOP_X_OFFSET + 28)
#define PHONE_NAVIGATION_DOWN_KEY_TOP_Y_OFFSET     (PHONE_NAVIGATION_UP_KEY_BOTTOM_Y_OFFSET)
#define PHONE_NAVIGATION_DOWN_KEY_BOTTOM_X_OFFSET  (PHONE_NAVIGATION_RIGHT_KEY_TOP_X_OFFSET)
#define PHONE_NAVIGATION_DOWN_KEY_BOTTOM_Y_OFFSET  (PHONE_NAVIGATION_RIGHT_KEY_BOTTOM_Y_OFFSET)

/* The rest of the keys */
#define PHONE_KEYS_X_OFFSET                        (PHONE_SK_X_OFFSET - 1)
#define PHONE_KEYS_Y_OFFSET                        (PHONE_SK_ROW_TWO_BOTTOM + 5)

#define PHONE_KEYS_HEIGHT                          (36)
#define PHONE_KEYS_WIDTH                           (86)
#define PHONE_KEYS_SPACE_BETWEEN_ROWS              (1)
#define PHONE_KEYS_SPACE_BETWEEN_COLS              (1)

#define PHONE_KEYS_ROW_ONE_TOP                     (PHONE_KEYS_Y_OFFSET)
#define PHONE_KEYS_ROW_TWO_TOP                     (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT + PHONE_KEYS_SPACE_BETWEEN_ROWS)
#define PHONE_KEYS_ROW_THREE_TOP                   (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT * 2 + PHONE_KEYS_SPACE_BETWEEN_ROWS * 2)
#define PHONE_KEYS_ROW_FOUR_TOP                    (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT * 3 + PHONE_KEYS_SPACE_BETWEEN_ROWS * 3)

#define PHONE_KEYS_ROW_ONE_BOTTOM                  (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT)
#define PHONE_KEYS_ROW_TWO_BOTTOM                  (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT * 2 + PHONE_KEYS_SPACE_BETWEEN_ROWS)
#define PHONE_KEYS_ROW_THREE_BOTTOM                (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT * 3 + PHONE_KEYS_SPACE_BETWEEN_ROWS * 2)
#define PHONE_KEYS_ROW_FOUR_BOTTOM                 (PHONE_KEYS_Y_OFFSET + PHONE_KEYS_HEIGHT * 4 + PHONE_KEYS_SPACE_BETWEEN_ROWS * 3)

#define PHONE_KEYS_COL_ONE_LEFT                    (PHONE_KEYS_X_OFFSET)
#define PHONE_KEYS_COL_TWO_LEFT                    (PHONE_KEYS_X_OFFSET + PHONE_KEYS_WIDTH + PHONE_KEYS_SPACE_BETWEEN_COLS)
#define PHONE_KEYS_COL_THREE_LEFT                  (PHONE_KEYS_X_OFFSET + PHONE_KEYS_WIDTH * 2 + PHONE_KEYS_SPACE_BETWEEN_COLS * 2)

#define PHONE_KEYS_COL_ONE_RIGHT                   (PHONE_KEYS_X_OFFSET + PHONE_KEYS_WIDTH)
#define PHONE_KEYS_COL_TWO_RIGHT                   (PHONE_KEYS_X_OFFSET + PHONE_KEYS_WIDTH * 2 + PHONE_KEYS_SPACE_BETWEEN_COLS)
#define PHONE_KEYS_COL_THREE_RIGHT                 (PHONE_KEYS_X_OFFSET + PHONE_KEYS_WIDTH * 3 + PHONE_KEYS_SPACE_BETWEEN_COLS * 2)

static SDL_Surface *surface;

static CsrBool localGuiEnable = FALSE;
static CsrBool localTuiEnable = FALSE;
static CsrBool localTuiNoCls;
#else
static CsrBool localTuiEnable = TRUE;
static CsrBool localTuiNoCls;
#endif

#ifdef BSP_SUPPORT_SDL
static CsrUint16 handleKeyEvent(SDLKey sym, CsrUint8 key)
{
    if (key == 0)
    {
        switch (sym)
        {
            case SDLK_UP:
                return CSR_UI_KEY_UP;
            case SDLK_DOWN:
                return CSR_UI_KEY_DOWN;
            case SDLK_RIGHT:
                return CSR_UI_KEY_RIGHT;
            case SDLK_LEFT:
                return CSR_UI_KEY_LEFT;
            case SDLK_HOME:
                return CSR_UI_KEY_SK1;
            case SDLK_END:
                return CSR_UI_KEY_BACK;
            case SDLK_PAGEUP:
                return CSR_UI_KEY_SK2;
            case SDLK_PAGEDOWN:
                return CSR_UI_KEY_DEL;
            default:
                break;
        }
    }
    else
    {
        switch (key)
        {
            case 0x03:
#ifdef ENABLE_SHUTDOWN
                CsrSchedStop();
                return 0;
#else
                exit(0);
#endif
            case 0x08:
                return CSR_UI_KEY_BACK;
            case 0x7F:
                return CSR_UI_KEY_DEL;
            case 0x0D:
                return CSR_UI_KEY_SK1;
            default:
                if ((key >= 0x20) && (key <= 0x7E))
                {
                    return CSR_UI_KEY_ASCII(key);
                }
                break;
        }
    }
    return 0;
}

void CsrUiLowerEventLoop(void)
{
    SDL_Event event;
    do
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
#ifdef ENABLE_SHUTDOWN
                CsrSchedStop();
#else
                exit(0);
#endif
                break;
            case SDL_KEYDOWN:
            {
                CsrUint16 csrUiKey;
                csrUiKey = handleKeyEvent(event.key.keysym.sym, event.key.keysym.unicode & 0xFF);
                if (csrUiKey != 0)
                {
                    CsrUiKeyEvent(csrUiKey);
                }
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                CsrUiTouchEvent(event.button.x, event.button.y);
            }
            default:
                break;
        }
    } while (event.type != SDL_USEREVENT);
}

static void registerKeys(void)
{
    /* The SK1, SK2, BACK and DEL keys */
    CsrUiRegisterKey(PHONE_SK_COL_ONE_LEFT, PHONE_SK_ROW_ONE_TOP, PHONE_SK_COL_ONE_RIGHT, PHONE_SK_ROW_ONE_BOTTOM, CSR_UI_KEY_SK1);
    CsrUiRegisterKey(PHONE_SK_COL_TWO_LEFT, PHONE_SK_ROW_ONE_TOP, PHONE_SK_COL_TWO_RIGHT, PHONE_SK_ROW_ONE_BOTTOM, CSR_UI_KEY_SK2);
    CsrUiRegisterKey(PHONE_SK_COL_ONE_LEFT, PHONE_SK_ROW_TWO_TOP, PHONE_SK_COL_ONE_RIGHT, PHONE_SK_ROW_TWO_BOTTOM, CSR_UI_KEY_BACK);
    CsrUiRegisterKey(PHONE_SK_COL_TWO_LEFT, PHONE_SK_ROW_TWO_TOP, PHONE_SK_COL_TWO_RIGHT, PHONE_SK_ROW_TWO_BOTTOM, CSR_UI_KEY_DEL);

    /* The numeric key */
    CsrUiRegisterKey(PHONE_KEYS_COL_ONE_LEFT, PHONE_KEYS_ROW_ONE_TOP, PHONE_KEYS_COL_ONE_RIGHT, PHONE_KEYS_ROW_ONE_BOTTOM, CSR_UI_KEY_N1);
    CsrUiRegisterKey(PHONE_KEYS_COL_TWO_LEFT, PHONE_KEYS_ROW_ONE_TOP, PHONE_KEYS_COL_TWO_RIGHT, PHONE_KEYS_ROW_ONE_BOTTOM, CSR_UI_KEY_N2);
    CsrUiRegisterKey(PHONE_KEYS_COL_THREE_LEFT, PHONE_KEYS_ROW_ONE_TOP, PHONE_KEYS_COL_THREE_RIGHT, PHONE_KEYS_ROW_ONE_BOTTOM, CSR_UI_KEY_N3);
    CsrUiRegisterKey(PHONE_KEYS_COL_ONE_LEFT, PHONE_KEYS_ROW_TWO_TOP, PHONE_KEYS_COL_ONE_RIGHT, PHONE_KEYS_ROW_TWO_BOTTOM, CSR_UI_KEY_N4);
    CsrUiRegisterKey(PHONE_KEYS_COL_TWO_LEFT, PHONE_KEYS_ROW_TWO_TOP, PHONE_KEYS_COL_TWO_RIGHT, PHONE_KEYS_ROW_TWO_BOTTOM, CSR_UI_KEY_N5);
    CsrUiRegisterKey(PHONE_KEYS_COL_THREE_LEFT, PHONE_KEYS_ROW_TWO_TOP, PHONE_KEYS_COL_THREE_RIGHT, PHONE_KEYS_ROW_TWO_BOTTOM, CSR_UI_KEY_N6);
    CsrUiRegisterKey(PHONE_KEYS_COL_ONE_LEFT, PHONE_KEYS_ROW_THREE_TOP, PHONE_KEYS_COL_ONE_RIGHT, PHONE_KEYS_ROW_THREE_BOTTOM, CSR_UI_KEY_N7);
    CsrUiRegisterKey(PHONE_KEYS_COL_TWO_LEFT, PHONE_KEYS_ROW_THREE_TOP, PHONE_KEYS_COL_TWO_RIGHT, PHONE_KEYS_ROW_THREE_BOTTOM, CSR_UI_KEY_N8);
    CsrUiRegisterKey(PHONE_KEYS_COL_THREE_LEFT, PHONE_KEYS_ROW_THREE_TOP, PHONE_KEYS_COL_THREE_RIGHT, PHONE_KEYS_ROW_THREE_BOTTOM, CSR_UI_KEY_N9);
    CsrUiRegisterKey(PHONE_KEYS_COL_ONE_LEFT, PHONE_KEYS_ROW_FOUR_TOP, PHONE_KEYS_COL_ONE_RIGHT, PHONE_KEYS_ROW_FOUR_BOTTOM, CSR_UI_KEY_STAR);
    CsrUiRegisterKey(PHONE_KEYS_COL_TWO_LEFT, PHONE_KEYS_ROW_FOUR_TOP, PHONE_KEYS_COL_TWO_RIGHT, PHONE_KEYS_ROW_FOUR_BOTTOM, CSR_UI_KEY_N0);
    CsrUiRegisterKey(PHONE_KEYS_COL_THREE_LEFT, PHONE_KEYS_ROW_FOUR_TOP, PHONE_KEYS_COL_THREE_RIGHT, PHONE_KEYS_ROW_FOUR_BOTTOM, CSR_UI_KEY_HASH);

    /* The navigation keys */
    CsrUiRegisterKey(PHONE_NAVIGATION_LEFT_KEY_TOP_X_OFFSET, PHONE_NAVIGATION_LEFT_KEY_TOP_Y_OFFSET, PHONE_NAVIGATION_LEFT_KEY_BOTTOM_X_OFFSET,
        PHONE_NAVIGATION_LEFT_KEY_BOTTOM_Y_OFFSET, CSR_UI_KEY_LEFT);

    CsrUiRegisterKey(PHONE_NAVIGATION_RIGHT_KEY_TOP_X_OFFSET, PHONE_NAVIGATION_RIGHT_KEY_TOP_Y_OFFSET, PHONE_NAVIGATION_RIGHT_KEY_BOTTOM_X_OFFSET,
        PHONE_NAVIGATION_RIGHT_KEY_BOTTOM_Y_OFFSET, CSR_UI_KEY_RIGHT);

    CsrUiRegisterKey(PHONE_NAVIGATION_UP_KEY_TOP_X_OFFSET, PHONE_NAVIGATION_UP_KEY_TOP_Y_OFFSET, PHONE_NAVIGATION_UP_KEY_BOTTOM_X_OFFSET,
        PHONE_NAVIGATION_UP_KEY_BOTTOM_Y_OFFSET, CSR_UI_KEY_UP);

    CsrUiRegisterKey(PHONE_NAVIGATION_DOWN_KEY_TOP_X_OFFSET, PHONE_NAVIGATION_DOWN_KEY_TOP_Y_OFFSET, PHONE_NAVIGATION_DOWN_KEY_BOTTOM_X_OFFSET,
        PHONE_NAVIGATION_DOWN_KEY_BOTTOM_Y_OFFSET, CSR_UI_KEY_DOWN);
}

static void drawPhone(void)
{
    CsrUint32 xc, yc, index;

    /* Clear entire surface */
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));

    /* Draw phone image */
    index = 0;
    for (yc = PHONE_SKIN_HEIGHT; yc > 0; yc--)
    {
        for (xc = 0; xc < PHONE_SKIN_WIDTH; xc++)
        {
            SDL_Rect rect = {PHONE_X_OFFSET + xc, PHONE_Y_OFFSET + yc - 1, 1, 1};
            Uint32 colour = SDL_MapRGB(surface->format, phoneSkinImageData[index],
                phoneSkinImageData[index + 1],
                phoneSkinImageData[index + 2]);
            SDL_FillRect(surface, &rect, colour);
            index += 3;
        }
    }
}

#endif

CsrBool CsrUiLowerInitialise(CsrBool guiEnable, CsrBool tuiEnable, CsrBool tuiNoCls)
{
#ifdef BSP_SUPPORT_SDL
    localGuiEnable = guiEnable;
    localTuiEnable = tuiEnable;
#else
    CSR_UNUSED(guiEnable);
    CSR_UNUSED(tuiEnable); /* Always enable TUI when SDL not supported */
#endif
    localTuiNoCls = tuiNoCls;

#ifdef BSP_SUPPORT_SDL
    if (localGuiEnable)
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_EnableUNICODE(1);
            surface = SDL_SetVideoMode(DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
            if (surface != NULL)
            {
                /* Draw the picture */
                drawPhone();

                /* Register the keys on the phone */
                registerKeys();
            }
            else
            {
                SDL_Quit();
                localGuiEnable = FALSE;
                localTuiEnable = TRUE;
            }
        }
        else
        {
            localGuiEnable = FALSE;
            localTuiEnable = TRUE;
        }
    }
    return localGuiEnable;
#else
    return FALSE;
#endif
}

#ifdef BSP_SUPPORT_SDL
/* Called by CSR_UI before a redraw of the display is started */
static void lock(void)
{
    SDL_LockSurface(surface);
}

/* Called by CSR_UI after a redraw of the display completes */
static void unlock(void)
{
    SDL_UnlockSurface(surface);
    SDL_Flip(surface);
}

/* Called by CSR_UI to paint a particular pixel with a specified colour */
static void setPixel(CsrInt16 x, CsrInt16 y, CsrUint32 colour)
{
    SDL_Rect rect = {x, y, 1, 1};
    colour = SDL_MapRGB(surface->format,
        (colour & 0x0000FF),
        (colour & 0x00FF00) >> 8,
        (colour & 0xFF0000) >> 16);
    SDL_FillRect(surface, &rect, colour);
}

static void drawHorizontalLine(CsrInt16 left, CsrInt16 top, CsrInt16 width, CsrUint32 color)
{
    SDL_Rect rect = {left, top, width, 1};
    color = SDL_MapRGB(surface->format,
        (color & 0x0000FF),
        (color & 0x00FF00) >> 8,
        (color & 0xFF0000) >> 16);
    SDL_FillRect(surface, &rect, color);
}

static void drawVerticalLine(CsrInt16 left, CsrInt16 top, CsrInt16 height, CsrUint32 color)
{
    SDL_Rect rect = {left, top, 1, height};
    color = SDL_MapRGB(surface->format,
        (color & 0x0000FF),
        (color & 0x00FF00) >> 8,
        (color & 0xFF0000) >> 16);
    SDL_FillRect(surface, &rect, color);
}

static void drawBox(CsrInt16 left, CsrInt16 top, CsrInt16 right, CsrInt16 bottom, CsrUint32 outlineColor, CsrUint32 fillColor, CsrBool fill)
{
    Uint32 color = SDL_MapRGB(surface->format,
        (outlineColor & 0x0000FF),
        (outlineColor & 0x00FF00) >> 8,
        (outlineColor & 0xFF0000) >> 16);

    /* Vertical line sub-case */
    if (left == right)
    {
        drawVerticalLine(left, top, bottom - top + 1, outlineColor);
        return;
    }

    /* Horizontal line sub-case */
    if (top == bottom)
    {
        drawHorizontalLine(left, top, right - left + 1, outlineColor);
        return;
    }

    drawHorizontalLine(left, top, right - left + 1, outlineColor);
    drawHorizontalLine(left, bottom, right - left + 1, outlineColor);
    if ((bottom - top) > 1)
    {
        drawVerticalLine(left, top + 1, bottom - top - 1, outlineColor);
        drawVerticalLine(right, top + 1, bottom - top - 1, outlineColor);

        if (fill && ((right - left) > 1))
        {
            SDL_Rect rect = {left + 1, top + 1, right - left - 1, bottom - top - 1};
            color = SDL_MapRGB(surface->format,
                (fillColor & 0x0000FF),
                (fillColor & 0x00FF00) >> 8,
                (fillColor & 0xFF0000) >> 16);
            SDL_FillRect(surface, &rect, color);
        }
    }
}

#endif

void CsrUiGraphicsRendererConfigurationGet(CsrUiGraphicsRenderer *renderer)
{
#ifdef BSP_SUPPORT_SDL
    renderer->active = localGuiEnable;
    renderer->top = PHONE_SCREEN_Y_OFFSET;
    renderer->left = PHONE_SCREEN_X_OFFSET;
    renderer->setPixel = setPixel;
    renderer->drawBox = drawBox;
    renderer->lock = lock;
    renderer->unlock = unlock;
#else
    renderer->active = FALSE;
#endif
}

static void setText(CsrUint16 *status, CsrUint16 *canvas, CsrUint16 *softkey1, CsrUint16 *softkey2)
{
    CsrUtf8String *asciiStatus = CsrUtf16String2Utf8(status);
    CsrUtf8String *asciiCanvas = CsrUtf16String2Utf8(canvas);
    CsrUtf8String *asciiSoftkey1 = CsrUtf16String2Utf8(softkey1);
    CsrUtf8String *asciiSoftkey2 = CsrUtf16String2Utf8(softkey2);

    if (!localTuiNoCls)
    {
        int res = system("clear");
        CSR_UNUSED(res);
    }

    printf("\n\n********************************************************************************\n\n");
    if (status != NULL)
    {
        printf("%s\n\n", asciiStatus);
        CsrPmemFree(asciiStatus);
    }
    if (canvas != NULL)
    {
        printf("%s\n\n", asciiCanvas);
        CsrPmemFree(asciiCanvas);
    }

    if (softkey1 != NULL)
    {
        printf("%-38s || ", asciiSoftkey1);
        CsrPmemFree(asciiSoftkey1);
    }
    if (softkey2 != NULL)
    {
        printf("%38s", asciiSoftkey2);
        CsrPmemFree(asciiSoftkey2);
    }
    printf("\n\n********************************************************************************");
}

void CsrUiTextRendererConfigurationGet(CsrUiTextRenderer *renderer)
{
    renderer->active = localTuiEnable;
    renderer->setText = setText;
}
