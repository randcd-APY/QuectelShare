/*
 * Copyright (c) 2014-2015, Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "common.h"
#include "module.h"
#include "utils.h"
#include "draw.h"
#include "input.h"
#include "mmi.h"

static sem_t g_local_module_sem;
static pthread_t g_module_tid;
static bool touch_completed = false;
static draw_control_t lcd_draw;

static key_map_t key_map[] = {
    {KEY_STR_HOME, NULL, KEY_HOME, false, false},
    {KEY_STR_MENU, NULL, KEY_MENU, false, false},
    {KEY_STR_BACK, NULL, KEY_BACK, false, false},
    {KEY_STR_VOLUMEDOWN, NULL, KEY_VOLUMEDOWN, false, false},
    {KEY_STR_VOLUMEUP, NULL, KEY_VOLUMEUP, false, false},
    {KEY_STR_POWER, NULL, KEY_POWER, false, false},
    {KEY_STR_SNAPSHOT, NULL, KEY_CAMERA_SNAPSHOT, false, false},
    {KEY_STR_HEADPHONE, NULL, SW_HEADPHONE_INSERT, false, false},
    {KEY_STR_MICROPHONE, NULL, SW_MICROPHONE_INSERT, false, false},
    {KEY_STR_HANGUP, NULL, KEY_MEDIA, false, false},
};

/**Change the operation permission for PASS,FAIL button:
*  enable = 1: enable operation for the buttons(both pass and fail button).
*  enable = 0: disable operation for the buttons(both pass and fail button).
**/
static void btn_enable(layout * lay, bool enable) {

    if(lay == NULL) {
        ALOGE("%s:No layout\n", __FUNCTION__);
        return;
    }

    button *pass_btn = lay->find_button_by_name(KEY_PASS);
    button *fail_btn = lay->find_button_by_name(KEY_FAIL);

    if(pass_btn != NULL) {
        if(enable)
            pass_btn->set_disabled(false);
        else
            pass_btn->set_disabled(true);
    }

    if(fail_btn != NULL) {
        if(enable)
            fail_btn->set_disabled(false);
        else
            fail_btn->set_disabled(true);
    }
}

/**Change the visibility for PASS,FAIL,DISPLAY button:
*  visible = 1: dispaly button visible,pass and fail button invisible.
*  visible = 0: pass and fail button visible, display button invisible.
**/
static void change_visibility(layout * lay, bool visible) {

    if(lay == NULL) {
        ALOGE("%s:No layout\n", __FUNCTION__);
        return;
    }

    button *pass_btn = lay->find_button_by_name(KEY_PASS);
    button *fail_btn = lay->find_button_by_name(KEY_FAIL);
    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(pass_btn != NULL) {
        pass_btn->set_visibility(!visible);
    }

    if(fail_btn != NULL) {
        fail_btn->set_visibility(!visible);
    }

    if(display_btn != NULL) {
        display_btn->set_visibility(visible);
    }
}
static void *process_auto_exit(void *m) {
    ALOGI("[%s] start,", __FUNCTION__);
    process_exit(m, SUCCESS);
    return NULL;
}

void do_auto_exit(void *m) {
    ALOGI("[%s] start,", __FUNCTION__);
    int ret = -1;
    pthread_t pid;

    ret = pthread_create(&pid, NULL, process_auto_exit, m);
    if(ret < 0) {
        ALOGE("%s:Can't create msg handle pthread: %s\n", __FUNCTION__, strerror(errno));
    }
}

/**========================================*/
/**LCD test*/
void do_next(void *m) {
    sem_post(&g_local_module_sem);
}


void launch_lcd_show_image(module_info * mod) {

    unordered_map < string, string > params;
    ALOGI("[%s] start,", __FUNCTION__);
    if(mod == NULL)
        return;

    parse_parameter(mod->config_list["parameter"], params);
    ALOGI("show image:%s", params["filename"].c_str());
    SkBitmap *bitmap = load_image(params["filename"], false);

    if(bitmap == NULL) {
        ALOGE("[%s] No image file specified,", __FUNCTION__);
        return;
    }

    strlcpy(lcd_draw.name, "LCD", sizeof(lcd_draw.name));
    lcd_draw.layer = 0x71000000;
    init_surface(&lcd_draw);

    draw_bitmap(bitmap, &lcd_draw);
}

void stop_lcd_show_image() {
    ALOGI("[%s] start,", __FUNCTION__);
    remove_surface(&lcd_draw);
}

void launch_lcd_show_rgb(module_info * mod) {

    unordered_map < string, string > params;
    int color = 0x00000000;

    /*Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }
    /**switch layout to pcba and initial the module*/
    switch_cur_layout_locked(lay, mod);

    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(display_btn == NULL)
        return;

    parse_parameter(mod->config_list["parameter"], params);
    if(!strcmp(params["color"].c_str(), "red")) {
        color = COLOR_RED;
    } else if(!strcmp(params["color"].c_str(), "blue")) {
        color = COLOR_BLUE;
    } else if(!strcmp(params["color"].c_str(), "green")) {
        color = COLOR_GREEN;
    } else if(!strcmp(params["color"].c_str(), "black")) {
        color = COLOR_BLACK;
    }

    change_visibility(lay, true);

    display_btn->set_color(color);
    invalidate();
}

static void launch_local_lcd(module_info * mod) {
    /*Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }

    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(display_btn == NULL)
        return;

    change_visibility(lay, true);

    display_btn->set_color(COLOR_RED);
    invalidate();
    sem_wait(&g_local_module_sem);

    display_btn->set_color(COLOR_GREEN);
    invalidate();
    sem_wait(&g_local_module_sem);

    display_btn->set_color(COLOR_BLUE);
    invalidate();
    sem_wait(&g_local_module_sem);

    display_btn->set_color(COLOR_WHITE);
    invalidate();
    sem_wait(&g_local_module_sem);

    display_btn->set_color(COLOR_BLACK);
    invalidate();
    sem_wait(&g_local_module_sem);

    change_visibility(lay, false);

    invalidate();
}

/**KEY test*/
void key_down_cb(void *modev) {
    unsigned int i = 0;
    bool ret = true;

    if(modev == NULL)
        return;

    mod_ev_t *pmodev = (mod_ev_t *) modev;

    ALOGI("%s: Key[%d] callback invoke", __FUNCTION__, pmodev->ev->code);

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(pmodev->ev->code == key_map[i].key_code) {
            key_map[i].tested = true;
        }
        ALOGI("%s: Last status Key[%s,%d, exist:%d,tested:%d] callback invoke", __FUNCTION__, key_map[i].key_name,
              key_map[i].key_code, key_map[i].exist, key_map[i].tested);
    }

   /**Check if completed*/
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist)
            ret &= key_map[i].tested;
    }

    if(ret)
        do_auto_exit(pmodev->mod);

}

static void launch_local_key(module_info * mod) {
    unsigned int i = 0;

    unordered_map < string, string > params;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }

    parse_parameter(mod->config_list["parameter"], params);
    /**Initial status*/
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(strstr(params["keys"].c_str(), key_map[i].key_name) != NULL) {
            key_map[i].exist = true;
            key_map[i].tested = false;
        } else {
            key_map[i].exist = false;
            key_map[i].tested = false;
        }
    }

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist) {
            lay->set_button_color_by_name(key_map[i].key_name, COLOR_KEY_DEFAULT);
        } else {
            key_map[i].key_btn = lay->find_button_by_name(key_map[i].key_name);
            if(key_map[i].key_btn != NULL)
                key_map[i].key_btn->set_visibility(false);
        }
    }

    invalidate();

    /**Register call back */
    input_listener *lis = new input_listener_key(mod, lay, key_down_cb);

    if(lis != NULL)
        register_input_listener(lis);
}

/**TOUCH test*/
void launch_touch_show_rgb(module_info * mod) {
    /*Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }

    /**switch layout to pcba and initial the module*/
    switch_cur_layout_locked(lay, mod);

    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(display_btn == NULL)
        return;

    change_visibility(lay, true);

    display_btn->set_color(COLOR_RED);
    invalidate();
    sem_wait(&g_local_module_sem);

    display_btn->set_color(COLOR_GREEN);
    invalidate();
    sem_wait(&g_local_module_sem);

    display_btn->set_color(COLOR_BLUE);
    invalidate();
}


void touch_complete_cb(void *m) {

    if(!touch_completed) {
        ALOGI("Touch complete callback invoke sleep some time for mis-click action");
        sleep(1);
        do_auto_exit(m);
        touch_completed = true;
    }
}

static void touch_mode_edges(layout * lay) {
    int w = get_fb_width();
    int h = get_fb_height();
    int row = 20 - 1;
    int col = 10 - 1;
    rect_t rect;
    button *btn;

    for(int i = 0; i < row; i += 2) {
        rect.x = 0;
        rect.y = i * h / row;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
    for(int i = 0; i < row; i += 2) {
        rect.x = 8 * w / col;
        rect.y = i * h / row;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
    for(int i = 0; i < col; i += 2) {
        rect.x = i * w / col;
        rect.y = 0;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
    for(int i = 0; i < col; i += 2) {
        rect.x = i * w / col;
        rect.y = 18 * h / row;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
}

static void set_btn(layout * lay, int w, int h, int x, int y) {
    int row = 20 - 1;
    int col = 10 - 1;

    rect_t rect;
    button *btn;

    if((x == 0) || (y == 0) || (x == w) || (y == h)) {
        if((x == 0) && (y == 0)) {
            rect.x = 0;
            rect.y = 0;
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
        if((x == 0) && (y == h)) {
            rect.x = 0;
            rect.y = h - (h / row);
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
        if((x == w) && (y == 0)) {
            rect.x = w - (w / col);
            rect.y = 0;
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);

        }
        if((x == w) && (y == h)) {
            rect.x = w - (w / col);
            rect.y = h - (h / row);
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
    } else {
        rect.x = x - (w / (2 * col));
        rect.y = y - (h / (2 * row));
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
}

static void touch_mode_diagonal(layout * lay) {
    int w = get_fb_width();
    int h = get_fb_height();

    for(int i = 0; i < 9; i++) {
        set_btn(lay, w, h, w * i / 8, h * i / 8);
        set_btn(lay, w, h, w * i / 8, h * (8 - i) / 8);
    }
}


static void touch_mode_full(layout * lay) {
    int w = get_fb_width();
    int h = get_fb_width();
    int row = 20 - 1;
    int col = 10 - 1;
    rect_t rect;
    button *btn;

    for(int i = 0; i < row; i += 2) {
        for(int j = 0; j < col; j += 2) {
            rect.x = j * w / col;
            rect.y = i * h / row;
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);
            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
    }
}

static void launch_local_touch(module_info * mod) {

    if(mod == NULL) {
        ALOGE("touch test mode not set ");
        return;
    }
    unordered_map < string, string > params;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }

    change_visibility(lay, true);

    /**Register call back */
    touch_completed = false;
    input_listener *lis = new input_listener_touch(mod, lay, touch_complete_cb);

    if(lis != NULL)
        register_input_listener(lis);

   /**build touch mode*/
    ALOGE("[%s] mod:%s\n", mod->module, mod->config_list["parameter"].c_str());

    parse_parameter(mod->config_list["parameter"], params);
    if(!strcmp(params["mode"].c_str(), "edges")) {
        touch_mode_edges(lay);
    } else if(!strcmp(params["mode"].c_str(), "full")) {
        touch_mode_full(lay);
    } else if(!strcmp(params["mode"].c_str(), "diagonal")) {
        touch_mode_edges(lay);
        touch_mode_diagonal(lay);
    }
    invalidate();
}

static void launch_local_headset(module_info * mod) {
    unsigned int i = 0;

    unordered_map < string, string > params;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }

    parse_parameter(mod->config_list["parameter"], params);
    /**Initial status*/
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(strstr(params["keys"].c_str(), key_map[i].key_name) != NULL) {
            key_map[i].exist = true;
            key_map[i].tested = false;
        } else {
            key_map[i].exist = false;
            key_map[i].tested = false;
        }
    }

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist) {
            lay->set_button_color_by_name(key_map[i].key_name, COLOR_KEY_DEFAULT);
        } else {
            key_map[i].key_btn = lay->find_button_by_name(key_map[i].key_name);
            if(key_map[i].key_btn != NULL)
                key_map[i].key_btn->set_visibility(false);
        }
    }

    invalidate();

      /**Register call back */
    input_listener *lis = new input_listener_key(mod, lay, key_down_cb);

    if(lis != NULL)
        register_input_listener(lis);
}

/**Handle local modules*/
void launch_local_module(module_info * mod) {
    /**Init sem*/
    sem_init(&g_local_module_sem, 0, 0);

    if(is_lcd_module(mod)) {
        launch_local_lcd(mod);
    } else if(is_key_module(mod)) {
        launch_local_key(mod);
    } else if(is_touch_module(mod)) {
        launch_local_touch(mod);
    } else if(is_headset_key_module(mod)) {
        launch_local_headset(mod);
    }
    sem_close(&g_local_module_sem);
}
