#include "../ui.h"
#include <math.h>
#include <limits.h>

extern const uint8_t face_start[] asm("_binary_info1_face_rgb565_start");
LV_FONT_DECLARE(info1_digits_96);
static const lv_img_dsc_t face = {
    .header = {.cf = LV_IMG_CF_TRUE_COLOR, .w = 512, .h = 512},
    .data_size = 512 * 512 * 2, .data = face_start
};
lv_obj_t *ui_Screen1, *ui_rpm_bg, *ui_rpm_arc;
lv_obj_t *ui_label_mph_value, *ui_label_mph, *ui_label_gear;
lv_obj_t *ui_label_gear_value, *ui_label_odometer_value;
static lv_obj_t *readouts[3];
static int last_values[3] = {INT_MIN, INT_MIN, INT_MIN};

static lv_obj_t *black_panel(int x, int y, int width, int height)
{
    lv_obj_t *panel = lv_obj_create(ui_Screen1);
    lv_obj_remove_style_all(panel);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_size(panel, width, height);
    lv_obj_set_style_bg_color(panel, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return panel;
}

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_black(), 0);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    ui_rpm_bg = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_rpm_bg, &face);
    lv_obj_center(ui_rpm_bg);
    lv_img_set_zoom(ui_rpm_bg, 425);
    /* Reserved header: remove INFO1 only; retain the flanking red strokes. */
    black_panel(262, 65, 204, 54);
    static const lv_area_t boxes[3] = {
        {394, 162, 585, 243},
        {414, 291, 583, 372},
        {392, 420, 585, 504}
    };
    for(unsigned i = 0; i < 3; ++i) {
        const lv_area_t *box = &boxes[i];
        lv_obj_t *panel = black_panel(box->x1, box->y1,
            box->x2 - box->x1 + 1, box->y2 - box->y1 + 1);
        readouts[i] = lv_label_create(panel);
        lv_obj_remove_style_all(readouts[i]);
        lv_obj_set_style_text_font(readouts[i], &info1_digits_96, 0);
        lv_obj_set_style_text_color(readouts[i], lv_color_white(), 0);
        lv_label_set_text(readouts[i], "--");
        lv_obj_center(readouts[i]);
        last_values[i] = INT_MIN;
    }
    /* Hidden compatibility targets for inherited, unused gauge callbacks. */
    lv_obj_t **legacy[] = {&ui_label_mph_value, &ui_label_mph, &ui_label_gear,
        &ui_label_gear_value, &ui_label_odometer_value};
    for(unsigned i = 0; i < sizeof(legacy) / sizeof(legacy[0]); ++i) {
        *legacy[i] = lv_label_create(ui_Screen1);
        lv_obj_add_flag(*legacy[i], LV_OBJ_FLAG_HIDDEN);
    }
    ui_rpm_arc = NULL;
}

void ui_info1_set_values(float coolant_f, float oil_psi, float transmission_f)
{
    const float values[3] = {coolant_f, oil_psi, transmission_f};
    for(unsigned i = 0; i < 3; ++i) {
        if(!readouts[i]) continue;
        /* Keep unavailable/out-of-range inputs distinct from valid zero. */
        bool valid = isfinite(values[i]) && values[i] >= -99 && values[i] <= 999;
        if(i == 1 && values[i] < 0) valid = false;
        int value = valid ? (int)lroundf(values[i]) : INT_MIN;
        if(value == last_values[i]) continue;
        last_values[i] = value;
        if(valid) lv_label_set_text_fmt(readouts[i], "%d", value);
        else lv_label_set_text(readouts[i], "--");
        lv_obj_center(readouts[i]);
    }
}

void ui_tach_set_rpm(int rpm) { (void)rpm; }
void ui_tach_set_gear(int gear) { (void)gear; }

void ui_Screen1_screen_destroy(void)
{
    if(ui_Screen1) lv_obj_del(ui_Screen1);
    ui_Screen1 = ui_rpm_bg = ui_rpm_arc = NULL;
    ui_label_mph_value = ui_label_mph = ui_label_gear = NULL;
    ui_label_gear_value = ui_label_odometer_value = NULL;
    for(unsigned i = 0; i < 3; ++i) {
        readouts[i] = NULL;
        last_values[i] = INT_MIN;
    }
}
