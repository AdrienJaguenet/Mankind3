#include "engine.h"

engine_t engine_new()
{
  engine_t engine;

  engine.window = SDL_CreateWindow("Mankind 3", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                   SDL_WINDOW_OPENGL);

  if (!engine.window) {
    FATAL("Failed to create SDL window: %s", SDL_GetError());
  }

  engine.renderer =
    SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_ACCELERATED);

  if (!engine.renderer) {
    FATAL("Failed to create SDL renderer: %s", SDL_GetError());
  }

  engine.running = true;

  return engine;
}

void engine_handle_event(engine_t * engine, SDL_Event * event)
{
  switch (event->type) {
    case SDL_QUIT:
      engine->running = false;
  }
}

void engine_update(engine_t * engine)
{
  (void) engine;
}

void engine_render(engine_t * engine)
{
  SDL_RenderClear(engine->renderer);

  SDL_RenderPresent(engine->renderer);
}

void engine_terminate(engine_t * engine)
{
  SDL_DestroyRenderer(engine->renderer);
  SDL_DestroyWindow(engine->window);
}
