#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <assert.h>
#include <unistd.h>

enum{
	WIDTH	= 128,
	HEIGHT	= 64, 
	POS_X	= 24,
	POS_Y	= 24,
	GREEN_PIXEL	= 0x00009a22,
};

static xcb_gcontext_t gc;

static void
setfont(xcb_connection_t* c,
              xcb_screen_t* s,
              xcb_window_t w,
              char* name)
{
    uint32_t v[3], mask;
    xcb_void_cookie_t cf;   // cookie font
    xcb_void_cookie_t cg;   // cookie gc
    xcb_font_t font;

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
    setfont(c, s, w, "10x20");

	while(1){
		char buf[64];
   		time_t t = time(NULL);
		struct tm *lt = localtime(&t);
		lt = localtime(&t);
		int n = snprintf(buf, sizeof(buf), "%02u:%02u:%02u", lt->tm_hour, lt->tm_min, lt->tm_sec);
		assert(xcb_request_check(c, xcb_image_text_8_checked(c, n, w, gc, POS_X, POS_Y, buf)) == 0);
		sleep(1);
    }
}
