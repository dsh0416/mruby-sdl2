#include "sdl2_video.h"
#include "sdl2_rect.h"
#include "sdl2_render.h"
#include "sdl2_surface.h"
#include "sdl2_video_gl.h"
#include "sdl2_video_display.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/array.h"
#ifdef __APPLE__
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_version.h>
#else
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_version.h>
#endif

struct RClass *mod_Video    = NULL;
struct RClass *class_Window = NULL;

typedef struct mrb_sdl2_video_window_data_t {
  bool        is_associated;
  SDL_Window *window;
} mrb_sdl2_video_window_data_t;

static void
mrb_sdl2_video_window_data_free(mrb_state *mrb, void *p)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)p;
  if (NULL != data) {
    if ((!data->is_associated) && (NULL != data->window)) {
      SDL_DestroyWindow(data->window);
    }
    mrb_free(mrb, data);
  }
}

static struct mrb_data_type const mrb_sdl2_video_window_data_type = {
  "Window", mrb_sdl2_video_window_data_free
};

SDL_Window *
mrb_sdl2_video_window_get_ptr(mrb_state *mrb, mrb_value window)
{
  mrb_sdl2_video_window_data_t *data;
  if (mrb_nil_p(window)) {
    return NULL;
  }
  data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, window, &mrb_sdl2_video_window_data_type);
  return data->window;
}

mrb_value
mrb_sdl2_video_window(mrb_state *mrb, SDL_Window *window)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_video_window_data_t));
  if (NULL != data) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
  }
  data->is_associated = false;
  data->window = window;
  return mrb_obj_value(Data_Wrap_Struct(mrb, class_Window, &mrb_sdl2_video_window_data_type, data));
}

mrb_value
mrb_sdl2_video_associated_window(mrb_state *mrb, SDL_Window *window)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_video_window_data_t));
  if (NULL != data) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
  }
  data->is_associated = true;
  data->window = window;
  return mrb_obj_value(Data_Wrap_Struct(mrb, class_Window, &mrb_sdl2_video_window_data_type, data));
}

/***************************************************************************
*
* module SDL2::Video
*
***************************************************************************/

/*
 * SDL2::Video::init
 */
static mrb_value
mrb_sdl2_video_init(mrb_state *mrb, mrb_value self)
{
  mrb_value name;
  int ret = 0;
  int const argc = mrb_get_args(mrb, "|o", &name);
  if (0 == argc) {
    ret = SDL_VideoInit(NULL);
  } else if (mrb_string_p(name)) {
    ret = SDL_VideoInit(RSTRING_PTR(name));
  } else if (mrb_respond_to(mrb, name, mrb_intern(mrb, "to_s", 4))) {
    name = mrb_funcall(mrb, name, "to_s", 0);
    ret = SDL_VideoInit(RSTRING_PTR(name));
  } else {
    mrb_raise(mrb, E_TYPE_ERROR, "supplied argument cannot be converted to string.");
  }
  if (0 != ret) {
    mruby_sdl2_raise_error(mrb);
  }
  return mrb_nil_value();
}

/*
 * SDL2::Video::quit
 */
static mrb_value
mrb_sdl2_video_quit(mrb_state *mrb, mrb_value self)
{
  SDL_VideoQuit();
  return mrb_nil_value();
}

/*
 * SDL2::Video::screen_saver?
 */
static mrb_value
mrb_sdl2_video_get_screen_saver_enabled(mrb_state *mrb, mrb_value self)
{
  return SDL_IsScreenSaverEnabled() == SDL_FALSE ? mrb_false_value() : mrb_true_value();
}

/*
 * SDL2::Video::screen_saver=
 */
static mrb_value
mrb_sdl2_video_set_screen_saver_enabled(mrb_state *mrb, mrb_value self)
{
  mrb_bool enabled;
  mrb_get_args(mrb, "b", &enabled);
  if (enabled) {
    SDL_EnableScreenSaver();
  } else {
    SDL_DisableScreenSaver();
  }
  return self;
}

/*
 * SDL2::Video::video_drivers
 */
static mrb_value
mrb_sdl2_video_get_video_drivers(mrb_state *mrb, mrb_value self)
{
  int i;
  mrb_value array;
  int const n = SDL_GetNumVideoDrivers();
  if (n < 0) {
    mruby_sdl2_raise_error(mrb);
  }
  array = mrb_ary_new_capa(mrb, n);
  for (i = 0; i < n; ++i) {
    mrb_value const str = mrb_str_new_cstr(mrb, SDL_GetVideoDriver(i));
    mrb_ary_push(mrb, array, str);
  }
  return array;
}

/*
 * SDL2::Video::displays
 */
static mrb_value
mrb_sdl2_video_get_displays(mrb_state *mrb, mrb_value self)
{
  int const n = SDL_GetNumVideoDisplays();
  if (0 > n) {
    mruby_sdl2_raise_error(mrb);
  }
  return mrb_fixnum_value(n);
}

/*
 * SDL2::Video::current_driver
 */
static mrb_value
mrb_sdl2_video_get_current_video_driver(mrb_state *mrb, mrb_value self)
{
  const char* name = SDL_GetCurrentVideoDriver();
  if (NULL == name) {
    return mrb_nil_value();
  }
  return mrb_str_new_cstr(mrb, name);
}


/***************************************************************************
*
* class SDL2::Video::Window
*
***************************************************************************/

/*
 * SDL2::Video::Window.initialize
 */
static mrb_value
mrb_sdl2_video_window_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value title;
  mrb_int x, y, w, h;
  uint32_t flags;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)DATA_PTR(self);
  int const argc = mrb_get_args(mrb, "|Siiiii", &title, &x, &y, &w, &h, &flags);
  if ((0 != argc) && (6 != argc)) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments.");
  }
  if (NULL == data) {
    data = (mrb_sdl2_video_window_data_t*)mrb_malloc(mrb, sizeof(mrb_sdl2_video_window_data_t));
    if (NULL == data) {
      mrb_raise(mrb, E_RUNTIME_ERROR, "insufficient memory.");
    }
  }
  if (6 == argc) {
    data->is_associated = false;
    data->window = SDL_CreateWindow(RSTRING_PTR(title), x, y, w, h, flags);
    if (NULL == data->window) {
      mruby_sdl2_raise_error(mrb);
    }
  } else {
    data->is_associated = false;
    data->window = NULL;
  }
  DATA_PTR(self) = data;
  DATA_TYPE(self) = &mrb_sdl2_video_window_data_type;
  return self;
}

/*
 * SDL2::Video::Window.create
 */
static mrb_value
mrb_sdl2_video_window_create(mrb_state *mrb, mrb_value self)
{
  mrb_value title;
  mrb_int x, y, w, h;
  uint32_t flags;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL != data->window) {
    mrb_raise(mrb, class_SDL2Error, "window has already been created.");
  }
  mrb_get_args(mrb, "Siiiii", &title, &x, &y, &w, &h, &flags);
  data->window = SDL_CreateWindow(RSTRING_PTR(title), x, y, w, h, flags);
  if (NULL == data->window) {
    mruby_sdl2_raise_error(mrb);
  }
  return self;
}

static mrb_value
mrb_sdl2_video_window_create_with_renderer(mrb_state *mrb, mrb_value self)
{
  int ret;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  mrb_int w, h;
  uint32_t flags;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL != data->window) {
    mrb_raise(mrb, class_SDL2Error, "window has already been created.");
  }
  mrb_get_args(mrb, "iii", &w, &h, &flags);
  ret = SDL_CreateWindowAndRenderer(w, h, flags, &window, &renderer);
  if (0 != ret) {
    mruby_sdl2_raise_error(mrb);
  }
  data->window = window;
  if (NULL == renderer) {
    return mrb_nil_value();
  }
  return mrb_sdl2_video_renderer(mrb, renderer);
}

/*
 * SDL2::Window.destroy
 */
static mrb_value
mrb_sdl2_video_window_destroy(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL != data->window) {
    SDL_DestroyWindow(data->window);
    data->window = NULL;
  }
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_video_window_width(mrb_state *mrb, mrb_value self)
{
   int w,h;
   mrb_sdl2_video_window_data_t *data =
     (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
   if (NULL == data->window) {
     return mrb_nil_value();
   }
   SDL_GetWindowSize(data->window, &w, &h);
   return mrb_fixnum_value(w);
}

static mrb_value
mrb_sdl2_video_window_height(mrb_state *mrb, mrb_value self)
{
   int w,h;
   mrb_sdl2_video_window_data_t *data =
     (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
   if (NULL == data->window) {
     return mrb_nil_value();
   }
   SDL_GetWindowSize(data->window, &w, &h);
   return mrb_fixnum_value(h);
}

static mrb_value
mrb_sdl2_video_window_set_size(mrb_state *mrb, mrb_value self)
{
  mrb_int w,h;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  mrb_get_args(mrb, "ii", &w, &h);
  SDL_SetWindowSize(data->window, w, h);
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_position(mrb_state *mrb, mrb_value self)
{
  int x, y;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_GetWindowPosition(data->window, &x, &y);
  return mrb_sdl2_point(mrb, x, y);
}

static mrb_value
mrb_sdl2_video_window_set_position(mrb_state *mrb, mrb_value self)
{
  SDL_Point * p;
  mrb_value arg;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  mrb_get_args(mrb, "o", &arg);
  p = mrb_sdl2_point_get_ptr(mrb, arg);
  SDL_SetWindowPosition(data->window, p->x, p->y);
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_title(mrb_state *mrb, mrb_value self)
{
  const char *title;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  title = SDL_GetWindowTitle(data->window);
  if (NULL == title) {
    return mrb_nil_value();
  }
  return mrb_str_new_cstr(mrb, title);
}

static mrb_value
mrb_sdl2_video_window_set_title(mrb_state *mrb, mrb_value self)
{
  mrb_value title;
  SDL_Window * window = NULL;
  mrb_get_args(mrb, "S", &title);
  window = mrb_sdl2_video_window_get_ptr(mrb, self);
  SDL_SetWindowTitle(window, RSTRING_PTR(title));
  return self;
}

static mrb_value
mrb_sdl2_video_window_maximize(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_MaximizeWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_minimize(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_MinimizeWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_show(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_ShowWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_hide(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_HideWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_restore(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_RestoreWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_raise(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_RaiseWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_set_icon(mrb_state *mrb, mrb_value self)
{
  mrb_value surface;
  mrb_get_args(mrb, "o", &surface);
  SDL_SetWindowIcon(mrb_sdl2_video_window_get_ptr(mrb,  self),
                    mrb_sdl2_video_surface_get_ptr(mrb, surface));
  return self;
}


static mrb_value
mrb_sdl2_video_window_get_display_index(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(
      SDL_GetWindowDisplayIndex(
          mrb_sdl2_video_window_get_ptr(mrb, self)));
}


static mrb_value
mrb_sdl2_video_window_get_flags(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(
      SDL_GetWindowFlags(
          mrb_sdl2_video_window_get_ptr(mrb, self)));
}

static mrb_value
mrb_sdl2_video_window_get_grab(mrb_state *mrb, mrb_value self)
{
  return (SDL_FALSE == SDL_GetWindowGrab(
      mrb_sdl2_video_window_get_ptr(mrb, self))) ?
      mrb_true_value() : mrb_false_value();
}

static mrb_value
mrb_sdl2_video_window_set_grab(mrb_state *mrb, mrb_value self)
{
  mrb_bool grab;
  mrb_get_args(mrb, "b", &grab);
  SDL_SetWindowGrab(mrb_sdl2_video_window_get_ptr(mrb, self),
                    grab ? SDL_TRUE : SDL_FALSE);
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_maximum_size(mrb_state *mrb, mrb_value self)
{
  int w, h;
  mrb_value ary;
  SDL_GetWindowMaximumSize(mrb_sdl2_video_window_get_ptr(mrb, self),
                           &w,
                           &h);
  ary = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ary, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ary, mrb_fixnum_value(h));
  return ary;
}

static mrb_value
mrb_sdl2_video_window_set_maximum_size(mrb_state *mrb, mrb_value self)
{
  mrb_int max_w, max_h;
  mrb_get_args(mrb, "ii", &max_w, &max_h);
  SDL_SetWindowMaximumSize(mrb_sdl2_video_window_get_ptr(mrb, self),
                           max_w,
                           max_h);
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_minimum_size(mrb_state *mrb, mrb_value self)
{
  int w, h;
  mrb_value ary;
  SDL_GetWindowMinimumSize(mrb_sdl2_video_window_get_ptr(mrb, self),
                           &w,
                           &h);
  ary = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ary, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ary, mrb_fixnum_value(h));
  return self;
}

static mrb_value
mrb_sdl2_video_window_set_minimum_size(mrb_state *mrb, mrb_value self)
{
  mrb_int min_w, min_h;
  mrb_get_args(mrb, "ii", &min_w, &min_h);
  SDL_SetWindowMaximumSize(mrb_sdl2_video_window_get_ptr(mrb, self),
                           min_w,
                           min_h);
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_surface(mrb_state *mrb, mrb_value self)
{
  SDL_Surface *s;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  s = SDL_GetWindowSurface(data->window);
  if (NULL == s) {
    mruby_sdl2_raise_error(mrb);
  }
  return mrb_sdl2_video_surface(mrb, s, true);
}

static mrb_value
mrb_sdl2_video_window_update_surface(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  if (0 != SDL_UpdateWindowSurface(data->window)) {
    mruby_sdl2_raise_error(mrb);
  }
  return self;
}

static mrb_value
mrb_sdl2_video_window_update_surface_rects(mrb_state *mrb, mrb_value self)
{
  mrb_value rects_ary;
  int size, i;
  SDL_Rect *rects;
  mrb_get_args(mrb, "A", &rects_ary);
  size = RARRAY_LEN(rects_ary);
  rects = (SDL_Rect *) SDL_malloc(size * sizeof(SDL_Rect));
  for (i = 0; i < size; i++) {
    SDL_Rect *r = mrb_sdl2_rect_get_ptr(mrb, mrb_ary_ref(mrb, rects_ary, i));
    rects[i] = (SDL_Rect) {r->x, r->y, r->w, r->h};
  }
  if (0 != SDL_UpdateWindowSurfaceRects(
          mrb_sdl2_video_window_get_ptr(mrb, self),
          rects,
          size)) {
    mruby_sdl2_raise_error(mrb);
  }
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_renderer(mrb_state *mrb, mrb_value self)
{
  SDL_Renderer *renderer;
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  renderer = SDL_GetRenderer(data->window);
  if (NULL == renderer) {
    mruby_sdl2_raise_error(mrb);
  }
  return mrb_sdl2_video_renderer(mrb, renderer);
}

static mrb_value
mrb_sdl2_video_window_swap(mrb_state *mrb, mrb_value self)
{
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_GL_SwapWindow(data->window);
  return self;
}

static mrb_value
mrb_sdl2_video_window_set_opacity(mrb_state *mrb, mrb_value self)
{
#if SDL_VERSION_ATLEAST(2,0,5)
  mrb_float opacity;
  mrb_get_args(mrb, "f", &opacity);
  mrb_sdl2_video_window_data_t *data =
    (mrb_sdl2_video_window_data_t*)mrb_data_get_ptr(mrb, self, &mrb_sdl2_video_window_data_type);
  if (NULL == data->window) {
    return mrb_nil_value();
  }
  SDL_SetWindowOpacity(data->window,opacity);
#endif
  return self;
}

static mrb_value
mrb_sdl2_video_window_get_id(mrb_state *mrb, mrb_value self) {
  SDL_Window *w = mrb_sdl2_video_window_get_ptr(mrb, self);
  if (NULL == w)
    mrb_raise(mrb, E_RUNTIME_ERROR, "Couldn't find window");
  return mrb_fixnum_value(SDL_GetWindowID(w));
}

/* * * * */

void
mruby_sdl2_video_init(mrb_state *mrb)
{
  int arena_size;
  mod_Video = mrb_define_module_under(mrb, mod_SDL2,  "Video");
  class_Window      = mrb_define_class_under(mrb, mod_Video, "Window",      mrb->object_class);

  MRB_SET_INSTANCE_TT(class_Window,      MRB_TT_DATA);

  mrb_define_module_function(mrb, mod_Video, "init", mrb_sdl2_video_init, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Video, "quit", mrb_sdl2_video_quit, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Video, "screen_saver?", mrb_sdl2_video_get_screen_saver_enabled, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Video, "screen_saver=", mrb_sdl2_video_set_screen_saver_enabled, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_Video, "video_drivers", mrb_sdl2_video_get_video_drivers,        MRB_ARGS_NONE());

  mrb_define_module_function(mrb, mod_Video, "displays",      mrb_sdl2_video_get_displays,             MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_Video, "current_driver",mrb_sdl2_video_get_current_video_driver, MRB_ARGS_NONE());

  mrb_define_method(mrb, class_Window, "initialize",           mrb_sdl2_video_window_initialize,           MRB_ARGS_OPT(6));
  mrb_define_method(mrb, class_Window, "create",               mrb_sdl2_video_window_create,               MRB_ARGS_REQ(6));
  mrb_define_method(mrb, class_Window, "create_with_renderer", mrb_sdl2_video_window_create_with_renderer, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, class_Window, "destroy",              mrb_sdl2_video_window_destroy,              MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "width",                mrb_sdl2_video_window_width,                MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "height",               mrb_sdl2_video_window_height,               MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "set_size",             mrb_sdl2_video_window_set_size,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, class_Window, "position",             mrb_sdl2_video_window_get_position,         MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "position=",            mrb_sdl2_video_window_set_position,         MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Window, "title",                mrb_sdl2_video_window_get_title,            MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "title=",               mrb_sdl2_video_window_set_title,            MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Window, "maximize",             mrb_sdl2_video_window_maximize,             MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "minimize",             mrb_sdl2_video_window_minimize,             MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "show",                 mrb_sdl2_video_window_show,                 MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "hide",                 mrb_sdl2_video_window_hide,                 MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "restore",              mrb_sdl2_video_window_restore,              MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "raise",                mrb_sdl2_video_window_raise,                MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "icon=",                mrb_sdl2_video_window_set_icon,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Window, "diplay_index",         mrb_sdl2_video_window_get_display_index,    MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "flags",                mrb_sdl2_video_window_get_flags,            MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "grab",                 mrb_sdl2_video_window_get_grab,             MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "grab=",                mrb_sdl2_video_window_set_grab,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Window, "id",                   mrb_sdl2_video_window_get_id,               MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "maximum_size",         mrb_sdl2_video_window_get_maximum_size,     MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "maximum_size=",        mrb_sdl2_video_window_set_maximum_size,     MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Window, "minimum_size",         mrb_sdl2_video_window_get_minimum_size,     MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "minimum_size=",        mrb_sdl2_video_window_set_minimum_size,     MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_Window, "surface",              mrb_sdl2_video_window_get_surface,          MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "update_surface",       mrb_sdl2_video_window_update_surface,       MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "update_surface_rects", mrb_sdl2_video_window_update_surface_rects, MRB_ARGS_ANY());
  mrb_define_method(mrb, class_Window, "renderer",             mrb_sdl2_video_window_get_renderer,         MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "swap",                 mrb_sdl2_video_window_swap,                 MRB_ARGS_NONE());
  mrb_define_method(mrb, class_Window, "set_opacity",          mrb_sdl2_video_window_set_opacity,          MRB_ARGS_REQ(1));

  arena_size = mrb_gc_arena_save(mrb);

  /* SDL_BlendMode */
  mrb_define_const(mrb, mod_Video, "SDL_BLENDMODE_NONE",  mrb_fixnum_value(SDL_BLENDMODE_NONE));
  mrb_define_const(mrb, mod_Video, "SDL_BLENDMODE_BLEND", mrb_fixnum_value(SDL_BLENDMODE_BLEND));
  mrb_define_const(mrb, mod_Video, "SDL_BLENDMODE_ADD",   mrb_fixnum_value(SDL_BLENDMODE_ADD));
  mrb_define_const(mrb, mod_Video, "SDL_BLENDMODE_MOD",   mrb_fixnum_value(SDL_BLENDMODE_MOD));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  /* SDL_WindowFlags */
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_FULLSCREEN",         mrb_fixnum_value(SDL_WINDOW_FULLSCREEN));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_FULLSCREEN_DESKTOP", mrb_fixnum_value(SDL_WINDOW_FULLSCREEN_DESKTOP));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_OPENGL",             mrb_fixnum_value(SDL_WINDOW_OPENGL));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_SHOWN",              mrb_fixnum_value(SDL_WINDOW_SHOWN));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_HIDDEN",             mrb_fixnum_value(SDL_WINDOW_HIDDEN));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_BORDERLESS",         mrb_fixnum_value(SDL_WINDOW_BORDERLESS));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_RESIZABLE",          mrb_fixnum_value(SDL_WINDOW_RESIZABLE));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_MINIMIZED",          mrb_fixnum_value(SDL_WINDOW_MINIMIZED));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_MAXIMIZED",          mrb_fixnum_value(SDL_WINDOW_MAXIMIZED));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_INPUT_GRABBED",      mrb_fixnum_value(SDL_WINDOW_INPUT_GRABBED));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_INPUT_FOCUS",        mrb_fixnum_value(SDL_WINDOW_INPUT_FOCUS));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_MOUSE_FOCUS",        mrb_fixnum_value(SDL_WINDOW_MOUSE_FOCUS));
  mrb_define_const(mrb, class_Window, "SDL_WINDOW_FOREIGN",            mrb_fixnum_value(SDL_WINDOW_FOREIGN));

  mrb_define_const(mrb, class_Window, "SDL_WINDOWPOS_CENTERED",  mrb_fixnum_value(SDL_WINDOWPOS_CENTERED));
  mrb_define_const(mrb, class_Window, "SDL_WINDOWPOS_UNDEFINED", mrb_fixnum_value(SDL_WINDOWPOS_UNDEFINED));

  mrb_gc_arena_restore(mrb, arena_size);
  arena_size = mrb_gc_arena_save(mrb);

  mruby_sdl2_video_renderer_init(mrb, mod_Video);
  mruby_sdl2_video_surface_init(mrb, mod_Video);
  mruby_sdl2_video_gl_init(mrb);
  mruby_sdl2_video_display_init(mrb);

  mrb_gc_arena_restore(mrb, arena_size);
}

void
mruby_sdl2_video_final(mrb_state *mrb)
{
  mruby_sdl2_video_surface_final(mrb, mod_Video);
  mruby_sdl2_video_renderer_final(mrb, mod_Video);
  mruby_sdl2_video_gl_final(mrb);
  mruby_sdl2_video_display_final(mrb);
}
