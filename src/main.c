#include <lvgl.h>
#include <lv_demo_truck/lv_demo_truck.h>
#include <stdint.h>
#include <unistd.h>

static lv_display_t * init_drm(void);
static lv_display_t * init_sdl(void);
static lv_display_t * init_wayland(void);
static lv_result_t init_evdev(lv_display_t * disp);
static void discovery_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data);

typedef lv_display_t * (*init_backend_t)(void);

typedef struct {
    const char * name;
    init_backend_t init;
} backend_t;

static const backend_t backends[] = {
    {.name = "drm",     .init = init_drm},
    {.name = "wayland", .init = init_wayland},
    {.name = "sdl",     .init = init_sdl},
};

int main(int argc, const char ** argv)
{
    const char * ui_assets_path = "ui/assets";
    if(argc < 2) {
        LV_LOG_WARN("Assets Path not set, assuming `ui/assets`");
    }
    else {
        ui_assets_path = argv[1];
    }
    LV_LOG_USER("Assets path is %s", ui_assets_path);

    lv_init();
    const size_t backend_count =  LV_ARRAYLEN(backends);
    lv_display_t * display;
    for(size_t i  = 0; i < backend_count; ++i) {
        display = backends[i].init();
        if(!display) {
            LV_LOG_WARN("Failed to init '%s' backend", backends[i].name);
            continue;
        }
        break;
    }
    if(!display) {
        LV_LOG_ERROR("Failed to create a LVGL display");
        return 1;
    }

    lv_demo_truck(ui_assets_path);

    while(1) {
        uint32_t ms = lv_timer_handler();
        if(ms == LV_NO_TIMER_READY) {
            ms = LV_DEF_REFR_PERIOD;
        }
        usleep(ms * 1000);
    }
    lv_deinit();
}


static lv_display_t * init_sdl(void)
{
#if !LV_USE_SDL
    return NULL;
#else
    lv_display_t * disp = lv_sdl_window_create(1920, 1080);
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    return disp;
#endif
}
static lv_display_t * init_drm(void)
{
    char * device = lv_linux_drm_find_device_path();
    lv_display_t * disp = lv_linux_drm_create();

    if(disp == NULL) {
        LV_LOG_WARN("lv_linux_drm_create failed");
        return NULL;
    }

    lv_result_t res = lv_linux_drm_set_file(disp, device, -1);
    lv_free(device);
    if(res != LV_RESULT_OK) {
        lv_display_delete(disp);
        LV_LOG_WARN("lv_linux_drm_set_file failed");
        return NULL;
    }
    res = init_evdev(disp);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Failed to initialize evdev");
    }
    LV_LOG_USER("DRM initialized");
    return disp;
}

static lv_display_t * init_wayland(void)
{
    lv_display_t * disp = lv_wayland_window_create(1280, 720, "Renesas Oven 3D Demo", NULL);
    if(!disp) {
        LV_LOG_WARN("lv_wayland_window_create failed");
    }
    lv_wayland_window_set_fullscreen(disp, true);
    LV_LOG_USER("Wayland initialized");
    return disp;
}

static lv_result_t init_evdev(lv_display_t * disp)
{
    return lv_evdev_discovery_start(discovery_cb, disp);
}

static void discovery_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data)
{
    LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL ? "REL" :
                type == LV_EVDEV_TYPE_ABS ? "ABS" :
                type == LV_EVDEV_TYPE_KEY ? "KEY" :
                "unknown");

    lv_display_t * disp = user_data;
    lv_indev_set_display(indev, disp);
}
