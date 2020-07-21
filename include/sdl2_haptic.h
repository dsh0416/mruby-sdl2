#ifndef MRUBY_SDL2_HAPTIC_H
#define MRUBY_SDL2_HAPTIC_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_haptic.h>
#else
#include <SDL_haptic.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_haptic_init(mrb_state *mrb);
extern void mruby_sdl2_haptic_final(mrb_state *mrb);


#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_HAPTIC_H */
