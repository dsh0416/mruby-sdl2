#ifndef MRUBY_SDL2_gl_H
#define MRUBY_SDL2_gl_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#else
#include <SDL_render.h>
#include <SDL_video.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_gl_init(mrb_state *mrb);
extern void mruby_sdl2_gl_final(mrb_state *mrb);


#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_gl_H */
