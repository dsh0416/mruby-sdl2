#ifndef MRUBY_SDL2_RENDER_H
#define MRUBY_SDL2_RENDER_H

#include "sdl2.h"
#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#else
#include <SDL_render.h>
#include <SDL_rect.h>
#endif

typedef struct pixelbuf_data_t {
  SDL_Rect rect;
  void    *pixels;
  int      pitch;
} pixelbuf_data_t;

extern SDL_Renderer *mrb_sdl2_video_renderer_get_ptr(mrb_state *mrb, mrb_value renderer);

extern mrb_value mrb_sdl2_video_renderer(mrb_state *mrb, SDL_Renderer *renderer);
extern mrb_value mrb_sdl2_video_texture(mrb_state *mrb, SDL_Texture *texture);

extern SDL_Texture *mrb_sdl2_video_texture_get_ptr(mrb_state *mrb, mrb_value texture);

extern void mruby_sdl2_video_renderer_init(mrb_state *mrb, struct RClass *mod_Video);
extern void mruby_sdl2_video_renderer_final(mrb_state *mrb, struct RClass *mod_Video);


#endif /* end of MRUBY_SDL2_RENDER_H */

