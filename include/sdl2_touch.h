#ifndef MRUBY_SDL2_TOUCH_H
#define MRUBY_SDL2_TOUCH_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_touch.h>
#else
#include <SDL_touch.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_touch_init(mrb_state *mrb);
extern void mruby_sdl2_touch_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_TOUCH_H */
