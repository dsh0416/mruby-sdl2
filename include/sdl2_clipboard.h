#ifndef MRUBY_SDL2_CLIPBOARD_H
#define MRUBY_SDL2_CLIPBOARD_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_clipboard.h>
#else
#include <SDL_clipboard.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_clipboard_init(mrb_state *mrb);
extern void mruby_sdl2_clipboard_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_CLIPBOARD_H */
