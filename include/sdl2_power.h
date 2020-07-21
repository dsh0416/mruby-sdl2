#ifndef MRUBY_SDL2_POWER_H
#define MRUBY_SDL2_POWER_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_power.h>
#else
#include <SDL_power.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_power_init(mrb_state *mrb);
extern void mruby_sdl2_power_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_POWER_H */
