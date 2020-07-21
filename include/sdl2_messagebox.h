#ifndef MRUBY_SDL2_MESSAGEBOX_H
#define MRUBY_SDL2_MESSAGEBOX_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_messagebox.h>
#else
#include <SDL_stdinc.h>
#include <SDL_messagebox.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_messagebox_init(mrb_state *mrb);
extern void mruby_sdl2_messagebox_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_MESSAGEBOX_H */
