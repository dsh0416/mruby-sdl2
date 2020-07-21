#include "sdl2.h"
#include "sdl2_hints.h"
#include "sdl2_version.h"
#include "sdl2_video.h"
#include "sdl2_rect.h"
#include "sdl2_audio.h"
#include "sdl2_events.h"
#include "sdl2_keyboard.h"
#include "sdl2_mouse.h"
#include "sdl2_thread.h"
#include "sdl2_pixels.h"
#include "sdl2_mutex.h"
#include "sdl2_timer.h"
#include "sdl2_joystick.h"
#include "sdl2_haptic.h"
#include "sdl2_cpuinfo.h"
#include "sdl2_platform.h"
#include "sdl2_power.h"
#include "sdl2_gamecontroller.h"
#include "sdl2_rwops.h"
#include "sdl2_touch.h"
#include "sdl2_gesture.h"
#include "sdl2_filesystem.h"
#include "sdl2_messagebox.h"
#include "sdl2_clipboard.h"
#include "misc.h"
#include "mruby/string.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_platform.h>
#else
#include <SDL.h>
#include <SDL_platform.h>
#endif
#include <string.h>

struct RClass *mod_SDL2        = NULL;
struct RClass *class_SDL2Error = NULL;

/*
 * Helper function to raise SDL2Error
 */
void
mruby_sdl2_raise_error(mrb_state *mrb)
{
  mrb_value exc;
  char const *e = SDL_GetError();
  if (NULL == e) {
    e = "";
  }
  exc = mrb_exc_new(mrb, class_SDL2Error, e, strlen(e));
  SDL_ClearError();
  mrb_exc_raise(mrb, exc);
}


static mrb_value
mrb_sdl2_init(mrb_state *mrb, mrb_value self)
{
  mrb_int flags;
  int const argc = mrb_get_args(mrb, "|i", &flags);
  int ret;
  if (0 == argc) {
    ret = SDL_Init(SDL_INIT_EVERYTHING);
  } else {
    ret = SDL_Init(flags);
  }
  if (0 != ret) {
    mruby_sdl2_raise_error(mrb);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_quit(mrb_state *mrb, mrb_value self)
{
  SDL_Quit();
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_init_subsystem(mrb_state *mrb, mrb_value self)
{
  mrb_int flags;
  mrb_get_args(mrb, "i", &flags);
  if (0 != SDL_InitSubSystem(flags)) {
    mruby_sdl2_raise_error(mrb);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_quit_subsystem(mrb_state *mrb, mrb_value self)
{
  mrb_int flags;
  mrb_get_args(mrb, "i", &flags);
  SDL_QuitSubSystem(flags);
  return mrb_nil_value();
}

static mrb_value
mrb_sdl2_was_init(mrb_state *mrb, mrb_value self)
{
  mrb_int flags;
  mrb_get_args(mrb, "i", &flags);
  return mrb_fixnum_value(SDL_WasInit(flags));
}

static mrb_value
mrb_sdl2_get_platform(mrb_state *mrb, mrb_value self)
{
  return mrb_str_new_cstr(mrb, SDL_GetPlatform());
}

static mrb_value
mrb_sdl2_delay(mrb_state *mrb, mrb_value self)
{
  mrb_int ms;
  mrb_get_args(mrb, "i", &ms);
  SDL_Delay((Uint32)ms);
  return self;
}


static mrb_value
mrb_sdl2_log(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);
  SDL_Log(RSTRING_PTR(str));
  return self;
}


void
mruby_sdl2_module_init(mrb_state *mrb)
{
  mod_SDL2 = mrb_define_module(mrb, "SDL2");
  class_SDL2Error = mrb_define_class_under(mrb, mod_SDL2, "SDL2Error", mrb->eStandardError_class);

  mrb_define_module_function(mrb, mod_SDL2, "init",           mrb_sdl2_init,           MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, mod_SDL2, "quit",           mrb_sdl2_quit,           MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_SDL2, "init_subsystem", mrb_sdl2_init_subsystem, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_SDL2, "quit_subsystem", mrb_sdl2_quit_subsystem, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_SDL2, "init?",          mrb_sdl2_was_init,       MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_SDL2, "platform",       mrb_sdl2_get_platform,   MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mod_SDL2, "delay",          mrb_sdl2_delay,          MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mod_SDL2, "log",            mrb_sdl2_log,            MRB_ARGS_REQ(1));

  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_TIMER",          mrb_fixnum_value(SDL_INIT_TIMER));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_AUDIO",          mrb_fixnum_value(SDL_INIT_AUDIO));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_VIDEO",          mrb_fixnum_value(SDL_INIT_VIDEO));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_JOYSTICK",       mrb_fixnum_value(SDL_INIT_JOYSTICK));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_HAPTIC",         mrb_fixnum_value(SDL_INIT_HAPTIC));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_GAMECONTROLLER", mrb_fixnum_value(SDL_INIT_GAMECONTROLLER));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_EVENTS",         mrb_fixnum_value(SDL_INIT_EVENTS));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_EVERYTHING",     mrb_fixnum_value(SDL_INIT_EVERYTHING));
  mrb_define_const(mrb, mod_SDL2, "SDL_INIT_NOPARACHUTE",    mrb_fixnum_value(SDL_INIT_NOPARACHUTE));

  mrb_define_const(mrb, mod_SDL2, "SDL_BYTEORDER",  mrb_fixnum_value(SDL_BYTEORDER));
  mrb_define_const(mrb, mod_SDL2, "SDL_LIL_ENDIAN", mrb_fixnum_value(SDL_LIL_ENDIAN));
  mrb_define_const(mrb, mod_SDL2, "SDL_BIG_ENDIAN", mrb_fixnum_value(SDL_BIG_ENDIAN));
}

void
mruby_sdl2_module_final(mrb_state *mrb)
{
}

void
mrb_mruby_sdl2_gem_init(mrb_state *mrb)
{
  int arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_module_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_hints_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_version_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_video_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_rect_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_audio_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_events_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_keyboard_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_mouse_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_thread_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_mutex_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_timer_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_misc_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_joystick_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_haptic_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_cpuinfo_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_platform_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_power_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_pixels_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_gamecontroller_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_rwops_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_touch_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_gesture_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_filesystem_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_messagebox_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);

  arena_size = mrb_gc_arena_save(mrb);
  mruby_sdl2_clipboard_init(mrb);
  mrb_gc_arena_restore(mrb, arena_size);
}

void
mrb_mruby_sdl2_gem_final(mrb_state *mrb)
{
  mruby_sdl2_misc_final(mrb);
  mruby_sdl2_timer_final(mrb);
  mruby_sdl2_mutex_final(mrb);
  mruby_sdl2_thread_final(mrb);
  mruby_sdl2_mouse_final(mrb);
  mruby_sdl2_keyboard_final(mrb);
  mruby_sdl2_events_final(mrb);
  mruby_sdl2_audio_final(mrb);
  mruby_sdl2_rect_final(mrb);
  mruby_sdl2_video_final(mrb);
  mruby_sdl2_version_final(mrb);
  mruby_sdl2_hints_final(mrb);
  mruby_sdl2_module_final(mrb);
  mruby_sdl2_pixels_final(mrb);
  mruby_sdl2_joystick_final(mrb);
  mruby_sdl2_haptic_final(mrb);
  mruby_sdl2_cpuinfo_final(mrb);
  mruby_sdl2_platform_final(mrb);
  mruby_sdl2_power_final(mrb);
  mruby_sdl2_gamecontroller_final(mrb);
  mruby_sdl2_rwops_final(mrb);
  mruby_sdl2_touch_final(mrb);
  mruby_sdl2_gesture_final(mrb);
  mruby_sdl2_filesystem_final(mrb);
  mruby_sdl2_messagebox_final(mrb);
  mruby_sdl2_clipboard_final(mrb);
}
