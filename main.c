#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <assert.h>

#define elem(x) (sizeof(x)/sizeof(x[0]))
#define WIDTH 1024
#define HEIGHT 1024
#define GREEN_PIXEL 0x00009a22
typedef unsigned int rune;

rune charset[120+63];

static xcb_gc_t
getfont(xcb_connection_t* c,
              xcb_screen_t* s,
              xcb_window_t w,
              char* name)
{
    uint32_t v[3], mask;
    xcb_void_cookie_t cf;   // cookie font
    xcb_void_cookie_t cg;   // cookie gc
    xcb_font_t font;
    xcb_gcontext_t gc;

    font = xcb_generate_id(c);
    cf = xcb_open_font_checked(c, font, strlen(name), name);
    assert(xcb_request_check(c, cf) == 0);
    gc = xcb_generate_id(c);
    mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    v[0] = GREEN_PIXEL;
    v[1] = s->black_pixel;
    v[2] = font;
    cg = xcb_create_gc_checked(c, gc, w, mask, v);
    assert(xcb_request_check(c, cg) == 0);
    cf = xcb_close_font_checked(c, font);
    assert(xcb_request_check(c, cf) == 0);
    return gc;
}

static void
draw(xcb_connection_t* c,
            xcb_screen_t* s,
            xcb_window_t w,
            int16_t x1,
            int16_t y1,
            rune *str)
{
    xcb_void_cookie_t cg, ct; // cookie gc, text
    xcb_gcontext_t gc;

    gc = getfont(c, s, w, "kana14");
    ct = xcb_image_text_8_checked(c, strlen(str), w, gc, x1, y1, str);
    assert(xcb_request_check(c, ct) == 0);
    cg = xcb_free_gc(c, gc);
    assert(xcb_request_check(c, cg) == 0);
}

static void
init(void)
{
    int i,j;

   for(i = 0; i < 120; ++i)
        charset[i] = i+7;
    for(j = 0; j < 63; ++j)
        charset[i+j] = j + 0xa1;
}

int
main(int argc, char* argv[])
{
    uint32_t mask, v[2];
    int conidx;
    const xcb_setup_t* setup;
    xcb_screen_iterator_t i;
    xcb_connection_t* c;
    xcb_screen_t* s = 0;
    xcb_window_t w;
    xcb_void_cookie_t cw, cm;    // cookie map,window

    init();
    assert((c = xcb_connect(NULL, &conidx)) != 0);
    setup = xcb_get_setup(c);
    i = xcb_setup_roots_iterator(setup);
    for(; i.rem != 0; --conidx, xcb_screen_next(&i))
        if(conidx == 0)
            goto found;
    assert(0);

found:
    assert((s = i.data) != 0); 
    w = xcb_generate_id(c);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    v[0] = s->black_pixel;
    v[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION;
    cw = xcb_create_window_checked(c,
                                    s->root_depth,
                                    w,
                                    s->root,
                                    20,
                                    200,
                                    WIDTH,
                                    HEIGHT,
                                    0,
                                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                    s->root_visual,
                                    mask,
                                    v);
    cm = xcb_map_window_checked(c, w);
    assert(xcb_request_check(c, cw) == 0);
    assert(xcb_request_check(c, cm) == 0);
    xcb_flush(c);

    while(1){
        draw(c, s, w, rand()%WIDTH, rand()%HEIGHT, &charset[rand()%elem(charset)]);
    }
    return 0;
}