#ifndef MRUBY_SDL2_CPUINFO_H
#define MRUBY_SDL2_CPUINFO_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_cpuinfo.h>
#else
#include <SDL_clipboard.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_cpuinfo_init(mrb_state *mrb);
extern void mruby_sdl2_cpuinfo_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_CPUINFO_H */
