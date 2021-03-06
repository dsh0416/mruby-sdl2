#ifndef MRUBY_SDL2_VIDEO_DISPLAY_H
#define MRUBY_SDL2_VIDEO_DISPLAY_H

#include "mruby.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void mruby_sdl2_video_display_init(mrb_state *mrb);
extern void mruby_sdl2_video_display_final(mrb_state *mrb);

#ifdef __cplusplus
}
#endif

#endif /* end of MRUBY_SDL2_VIDEO_DISPLAY_H */

