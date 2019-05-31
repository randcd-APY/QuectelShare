/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "utils.h"
#include "view.h"
#ifdef ANDROID
bool init_surface(draw_control_t * draw) {

    ALOGI("%s: start init surface", __FUNCTION__);
    char value[PROPERTY_VALUE_MAX] = { 0 };
    if(draw == NULL) {
        ALOGI("%s: wrong param fail!", __FUNCTION__);
        return false;
    }

    draw->client = new SurfaceComposerClient();
    if(draw->client == NULL) {
        ALOGI("%s: get surface composer client fail!", __FUNCTION__);
        return false;
    }
    sp < IBinder > dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    status_t status = SurfaceComposerClient::getDisplayInfo(dtoken, &draw->dinfo);

    if(status) {
        ALOGI("%s: get display info fail!", __FUNCTION__);
        return false;
    }

    property_get("persist.panel.orientation", value, "0");
    int orient = atoi(value) / 90;

    if(orient == eOrientation90 || orient == eOrientation270) {
        int temp = draw->dinfo.h;

        draw->dinfo.h = draw->dinfo.w;
        draw->dinfo.w = temp;
    }

    draw->pixels_percent_x = draw->dinfo.w / PERCENT_MAX;
    draw->pixels_percent_y = draw->dinfo.h / PERCENT_MAX;

    Rect destRect(draw->dinfo.w, draw->dinfo.h);

    draw->client->setDisplayProjection(dtoken, orient, destRect, destRect);

    // create the native surface
    draw->control =
        draw->client->createSurface(String8(draw->name), draw->dinfo.w, draw->dinfo.h, PIXEL_FORMAT_RGBA_8888);

    if(draw->control == NULL) {
        ALOGI("%s: get native surface fail!", __FUNCTION__);
        return false;
    }

    draw->surface = draw->control->getSurface();

    SurfaceComposerClient::openGlobalTransaction();
    draw->control->setLayer(draw->layer);
    draw->control->show();
    SurfaceComposerClient::closeGlobalTransaction();

    ALOGI("Get display info(%d,%d), name:%s, layer:%d.  control:%p", draw->dinfo.w, draw->dinfo.h, draw->name,
          draw->layer, draw->control.get());
    return true;
}

bool create_surface(draw_control_t * draw) {

    if(draw == NULL || draw->client == NULL) {
        ALOGE("%s: Invalid param!", __FUNCTION__);
        return false;
    }

    ALOGI("Draw ( name:%s, layer:%d.  control:%p,surface(%d,%d) \n", draw->name, draw->layer, draw->control.get(),
          draw->surface_w, draw->surface_h);
    draw->control =
        draw->client->createSurface(String8(draw->name), draw->surface_w, draw->surface_h, PIXEL_FORMAT_RGBA_8888);

    if((draw->control == NULL) || (!draw->control->isValid())) {
        ALOGE("createSurface failed\n");
        return false;
    }
    draw->surface = draw->control->getSurface();

    SurfaceComposerClient::openGlobalTransaction();
    draw->control->setLayer(draw->layer);
    SurfaceComposerClient::closeGlobalTransaction();

    return true;
}

void remove_surface(draw_control_t * draw) {
    SurfaceComposerClient::openGlobalTransaction();
    draw->control->clear();
    SurfaceComposerClient::closeGlobalTransaction();
}

void draw_bitmap(SkBitmap * bitmap, draw_control_t * pdraw) {
    if(bitmap == NULL || pdraw == NULL || pdraw->surface == NULL) {
        ALOGE("NULL point  failed\n");
        return;
    }

    ANativeWindow_Buffer outBuffer;

    pdraw->surface->lock(&outBuffer, NULL);
    SkBitmap surfaceBitmap;
    ssize_t bpr = outBuffer.stride * bytesPerPixel(outBuffer.format);

    surfaceBitmap.installPixels(SkImageInfo::MakeN32Premul(outBuffer.width, outBuffer.height), outBuffer.bits, bpr);
    SkCanvas surfaceCanvas(surfaceBitmap);

        /**Initial background to Black*/
    surfaceCanvas.clear(SK_ColorBLACK);

    surfaceCanvas.drawBitmap(*bitmap, 0, 0, &pdraw->paint);
    pdraw->surface->unlockAndPost();

}
#endif

bool is_point_in_rect(int x, int y, rect_t * rect) {
    if(rect == NULL)
        return false;

    return (x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h);
}

bool is_point_in_rect(point_t point, rect_t * rect) {
    if(rect == NULL)
        return false;

    return (point.x >= rect->x && point.x <= rect->x + rect->w && point.y >= rect->y && point.y <= rect->y + rect->h);
}
