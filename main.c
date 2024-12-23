#include <unistd.h>
#include <xcb/xcb.h>

typedef uint32_t u32;

static xcb_connection_t *c;
static xcb_window_t w;

int
main(int argc, char *argv[])
{
    xcb_screen_iterator_t i;
    xcb_screen_t *s;

    c = xcb_connect(NULL, NULL);
    const xcb_setup_t *setup = xcb_get_setup(c);
    i = xcb_setup_roots_iterator(setup);
    s = i.data;

    w = xcb_generate_id(c);
    xcb_create_window(c,
                    XCB_COPY_FROM_PARENT,
                    w,
                    s->root,
                    0, 0,
                    150, 150,
                    10,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    s->root_visual,
                    XCB_CW_BACK_PIXEL,(u32[]){s->white_pixel});
    xcb_map_window(c, w);
    xcb_flush(c);
    xcb_create_gc(c, xcb_generate_id(c), w, XCB_GC_FOREGROUND,(u32[]){s->white_pixel});
    xcb_flush(c);

    pause();
    xcb_disconnect(c);

    return 0;
}