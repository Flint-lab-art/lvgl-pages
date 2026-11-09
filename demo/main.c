/* LVGL 离屏渲染演示：不开任何窗口系统，直接把界面画进内存并导出 raw 帧 */
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

#define W 480
#define H 800

/* 两种编译器都能识别的 8 字节对齐 */
#if defined(_MSC_VER)
static __declspec(align(8)) uint8_t render_buf[W * H * 4];
#else
static uint8_t render_buf[W * H * 4] __attribute__((aligned(8)));
#endif
static uint32_t frame[W * H];
static lv_obj_t * g_slider;

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    for(int32_t y = 0; y < h; y++) {
        memcpy(&frame[(area->y1 + y) * W + area->x1], px_map + (size_t)y * w * 4, (size_t)w * 4);
    }
    lv_display_flush_ready(disp);
}

/* ---------- 工具样式 ---------- */
static void card_style(lv_obj_t * obj)
{
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x171B26), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 18, 0);
    lv_obj_set_style_pad_all(obj, 16, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xE8EBF2), 0);
}

static lv_obj_t * mk_label(lv_obj_t * parent, const char * txt, const lv_font_t * font, uint32_t color)
{
    lv_obj_t * l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
    return l;
}

/* ---------- 界面 ---------- */
static void build_ui(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B0E14), 0);
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x131826), 0);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(scr, 16, 0);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(scr, 12, 0);

    /* 状态栏 */
    lv_obj_t * bar = lv_obj_create(scr);
    lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(bar, lv_pct(100), 40);
    lv_obj_set_style_bg_opa(bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_pad_all(bar, 0, 0);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    mk_label(bar, "09:41", &lv_font_montserrat_20, 0xE8EBF2);
    lv_obj_t * st = lv_label_create(bar);
    lv_label_set_text(st, LV_SYMBOL_WIFI "  " LV_SYMBOL_VOLUME_MAX "  " LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_font(st, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(st, lv_color_hex(0x8B93A7), 0);

    /* 主卡片：温控 */
    lv_obj_t * main_card = lv_obj_create(scr);
    lv_obj_remove_flag(main_card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(main_card, lv_pct(100), 300);
    card_style(main_card);

    mk_label(main_card, "Living Room", &lv_font_montserrat_16, 0x8B93A7);

    lv_obj_t * arc = lv_arc_create(main_card);
    lv_obj_set_size(arc, 196, 196);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 10);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_range(arc, 10, 32);
    lv_arc_set_value(arc, 22);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x2A3040), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x4C8DF6), LV_PART_INDICATOR);

    lv_obj_t * val = mk_label(main_card, "22.5", &lv_font_montserrat_32, 0xFFFFFF);
    lv_obj_align(val, LV_ALIGN_CENTER, 0, -2);
    lv_obj_t * unit = mk_label(main_card, "\xc2\xb0" "C", &lv_font_montserrat_16, 0x8B93A7);
    lv_obj_align(unit, LV_ALIGN_CENTER, 22, 34);
    lv_obj_t * hint = mk_label(main_card, "Heating to 24.0", &lv_font_montserrat_14, 0x4C8DF6);
    lv_obj_align(hint, LV_ALIGN_CENTER, 0, 82);

    /* 图表卡片 */
    lv_obj_t * chart_card = lv_obj_create(scr);
    lv_obj_remove_flag(chart_card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(chart_card, lv_pct(100), 178);
    card_style(chart_card);

    lv_obj_t * row = lv_obj_create(chart_card);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(row, lv_pct(100), 28);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    mk_label(row, "Temperature  /  last 12 h", &lv_font_montserrat_14, 0x8B93A7);
    mk_label(row, "+1.2", &lv_font_montserrat_14, 0x39D98A);

    lv_obj_t * chart = lv_chart_create(chart_card);
    lv_obj_set_size(chart, lv_pct(100), 118);
    lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 12);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 15, 30);
    lv_chart_set_div_line_count(chart, 4, 6);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_set_style_line_color(chart, lv_color_hex(0x2A3040), LV_PART_MAIN);
    lv_obj_set_style_line_width(chart, 1, LV_PART_MAIN);
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);

    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_color_hex(0x4C8DF6), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t pts[12] = {18, 19, 19, 20, 21, 21, 22, 22, 23, 22, 24, 22};
    lv_chart_set_series_values(chart, ser, pts, 12);

    /* 控制区 */
    lv_obj_t * ctrl = lv_obj_create(scr);
    lv_obj_remove_flag(ctrl, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(ctrl, lv_pct(100), 132);
    card_style(ctrl);
    lv_obj_set_flex_flow(ctrl, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(ctrl, 14, 0);

    lv_obj_t * l1 = lv_obj_create(ctrl);
    lv_obj_remove_flag(l1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(l1, lv_pct(100), 34);
    lv_obj_set_style_bg_opa(l1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(l1, 0, 0);
    lv_obj_set_style_pad_all(l1, 0, 0);
    lv_obj_set_flex_flow(l1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(l1, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    mk_label(l1, "Fan speed", &lv_font_montserrat_16, 0xE8EBF2);
    mk_label(l1, "60%", &lv_font_montserrat_16, 0x4C8DF6);

    lv_obj_t * slider = lv_slider_create(ctrl);
    g_slider = slider;
    lv_obj_set_size(slider, lv_pct(100), 12);
    lv_slider_set_value(slider, 60, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x2A3040), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x4C8DF6), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 4, LV_PART_KNOB);

    lv_obj_t * l2 = lv_obj_create(ctrl);
    lv_obj_remove_flag(l2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(l2, lv_pct(100), 42);
    lv_obj_set_style_bg_opa(l2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(l2, 0, 0);
    lv_obj_set_style_pad_all(l2, 0, 0);
    lv_obj_set_flex_flow(l2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(l2, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    mk_label(l2, "Auto mode", &lv_font_montserrat_16, 0xE8EBF2);
    lv_obj_t * sw = lv_switch_create(l2);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x4C8DF6), LV_PART_INDICATOR | LV_STATE_CHECKED);

    /* 底部按钮 */
    lv_obj_t * btns = lv_obj_create(scr);
    lv_obj_remove_flag(btns, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(btns, lv_pct(100), 76);
    lv_obj_set_style_bg_opa(btns, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btns, 0, 0);
    lv_obj_set_style_pad_all(btns, 0, 0);
    lv_obj_set_flex_flow(btns, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btns, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * b1 = lv_button_create(btns);
    lv_obj_set_size(b1, 110, 60);
    lv_obj_set_style_bg_color(b1, lv_color_hex(0x1E2536), 0);
    lv_obj_set_style_radius(b1, 16, 0);
    lv_obj_set_style_shadow_width(b1, 0, 0);
    mk_label(b1, LV_SYMBOL_MINUS, &lv_font_montserrat_20, 0xC8D0E0);
    lv_obj_center(lv_obj_get_child(b1, 0));

    lv_obj_t * b2 = lv_button_create(btns);
    lv_obj_set_size(b2, 200, 60);
    lv_obj_set_style_bg_color(b2, lv_color_hex(0x4C8DF6), 0);
    lv_obj_set_style_bg_grad_color(b2, lv_color_hex(0x2F6FE0), 0);
    lv_obj_set_style_bg_grad_dir(b2, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_radius(b2, 16, 0);
    lv_obj_set_style_shadow_color(b2, lv_color_hex(0x4C8DF6), 0);
    lv_obj_set_style_shadow_width(b2, 24, 0);
    lv_obj_set_style_shadow_opa(b2, LV_OPA_40, 0);
    mk_label(b2, "Boost  " LV_SYMBOL_PLAY, &lv_font_montserrat_16, 0xFFFFFF);
    lv_obj_center(lv_obj_get_child(b2, 0));

    lv_obj_t * b3 = lv_button_create(btns);
    lv_obj_set_size(b3, 110, 60);
    lv_obj_set_style_bg_color(b3, lv_color_hex(0x1E2536), 0);
    lv_obj_set_style_radius(b3, 16, 0);
    lv_obj_set_style_shadow_width(b3, 0, 0);
    mk_label(b3, LV_SYMBOL_PLUS, &lv_font_montserrat_20, 0xC8D0E0);
    lv_obj_center(lv_obj_get_child(b3, 0));
}

int main(void)
{
    lv_init();

    lv_display_t * disp = lv_display_create(W, H);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, render_buf, NULL, sizeof(render_buf), LV_DISPLAY_RENDER_MODE_FULL);

    build_ui();

    lv_slider_set_value(g_slider, 100, LV_ANIM_ON);

    for(int i = 0; i < 90; i++) {
        lv_tick_inc(5);
        lv_timer_handler();
        if(i % 6 == 0 && i < 60) {
            char path[64];
            snprintf(path, sizeof(path), "frames/%03d.raw", i / 6);
            FILE * ff = fopen(path, "wb");
            if(ff) { fwrite(frame, 4, W * H, ff); fclose(ff); }
        }
    }

    FILE * f = fopen("frame.raw", "wb");
    if(!f) {
        printf("open failed\n");
        return 1;
    }
    fwrite(frame, 4, W * H, f);
    fclose(f);
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("dumped %dx%d\n", W, H);
    printf("mem: used=%d total=%d max_used=%d frag=%d%%\n", (int)mon.total_size - (int)mon.free_size, (int)mon.total_size, (int)mon.max_used, (int)mon.frag_pct);
    return 0;
}
