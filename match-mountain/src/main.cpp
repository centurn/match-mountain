#include <SDL.h>
#include <cstdio>

int main(int argc, char *argv[])
{
  if (SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC)) != 0)
  {
    printf("SDL_Init error: %s\n", SDL_GetError());
  }else{
    printf("SDL Init OK\n");
  }
    return 0;
}
