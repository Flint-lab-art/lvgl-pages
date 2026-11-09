/* 最小化 LVGL 配置：只覆盖本 demo 需要的项，其余交给 lv_conf_internal.h 的默认值 */
#ifndef LV_CONF_H
#define LV_CONF_H

/* 32 位色，对应 ARGB8888 帧缓冲 */
#define LV_COLOR_DEPTH 32

/* 界面对象用的内存池 */
#define LV_MEM_SIZE (1024U * 1024U)

/* 位图字体：标题、数值、正文各用一档 */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_DEFAULT &lv_font_montserrat_16

/* 默认主题需要它来判断控件样式 */
#define LV_USE_THEME_DEFAULT 1

#endif /*LV_CONF_H*/
