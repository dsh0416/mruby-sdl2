#ifndef MRUBY_SDL2_VIDEO_H
#define MRUBY_SDL2_VIDEO_H

#include "mruby.h"
#ifdef __APPLE__
#include <SDL2/SDL_video.h>
#else
#include <SDL_video.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern struct RClass *mod_Video;
extern struct RClass *class_Window;

extern SDL_Window *mrb_sdl2_video_window_get_ptr(mrb_state *mrb, mrb_value window);
extern mrb_value mrb_sdl2_video_window(mrb_state *mrb, SDL_Window *window);
extern mrb_value mrb_sdl2_video_associated_window(mrb_state *mrb, SDL_Window *window);

extern void mruby_sdl2_video_init(mrb_state *mrb);
extern void mruby_sdl2_video_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_VIDEO_H */

