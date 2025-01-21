#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *obj0;
    lv_obj_t *mode;
    lv_obj_t *battery;
    lv_obj_t *title;
    lv_obj_t *tune;
    lv_obj_t *sound;
    lv_obj_t *sound__obj1;
    lv_obj_t *sound__play;
    lv_obj_t *sound__name;
    lv_obj_t *pitcher;
    lv_obj_t *pitcher__obj0;
    lv_obj_t *pitcher__cents;
    lv_obj_t *pitcher__freq;
    lv_obj_t *a4freq;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void create_user_widget_pitcher(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_pitcher(int startWidgetIndex);

void create_user_widget_sound(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_sound(int startWidgetIndex);

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/