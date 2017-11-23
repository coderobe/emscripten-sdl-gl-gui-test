// This work is licensed under the GNU AFFERO GENERAL PUBLIC LICENSE v3
// Copyright (c) 2017 Robin Broda

#include <cstdarg>
#include <cstdio>
#include <emscripten.h>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui_ui.h>
#include <imgui_io.h>
#include "platform_sdl.h"

using namespace imgui;

void println(const char* format, ...){
  va_list log;
  char buffer[256];
  va_start(log, format);
  vsnprintf(buffer, 256, format, log);
  va_end(log);
  emscripten_log(EM_LOG_CONSOLE, buffer);
}

unsigned int last_char = 0;
SDL_Window* gWindow = NULL;
SDL_GLContext gContext;
SDL_Renderer* gRenderer;
bool gRenderQuad = true;
int mouse_wheel = 0;
PlatformSDL gPlatform;
RenderSDL renderer;
Ui ui;
Rollout* rollout;
SDL_Event e;

float gR = 0.0;
float gG = 0.0;
float gB = 0.0;
bool gColorPositive = true;

void loop(){
  if(gColorPositive){
    if(gR < 1.0) gR += 0.01;
    if(gR >= 1.0 && gG < 1) gG += 0.01;
    if(gR >= 1.0 && gG >= 1.0 && gB < 1) gB += 0.01;
    if(gR >= 1.0 && gG >= 1.0 && gB >= 1.0) gColorPositive = false;
  }else{
    if(gR > 0.1) gR -= 0.01;
    if(gR <= 0.1 && gG > 0.1) gG -= 0.01;
    if(gR <= 0.1 && gG <= 0.1 && gB > 0.1) gB -= 0.01;
    if(gR <= 0.1 && gG <= 0.1 && gB <= 0.1) gColorPositive = true;
  }

  int x, y, w, h;
  SDL_GetMouseState(&x, &y);
  SDL_GetWindowSize(gWindow, &w, &h);
  uint keys_pressed = 0;

  ui.set_text_align(ALIGN_LEFT);
  ui.begin_frame(w, h, x, y, -mouse_wheel, last_char, keys_pressed);
  last_char = 0;
  mouse_wheel = 0;

  ui.begin_rollout(rollout);
  ui.indent();
  ui.label("Pointless button");
  ui.button("Click me");
  ui.separator(true);
  ui.label("- coderobe 2017");
  ui.unindent();
  ui.end_rollout();
  ui.end_frame();

  glClearColor(gR, gG, gB, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  ui.render_draw(true);
  SDL_GL_SwapWindow(gWindow);
}

int main(int argc, char** argv){
  println("hello, world");

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gWindow = SDL_CreateWindow("r",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    300,
    300,
    SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
  if(gWindow == NULL){
    println("Window could not be created! SDL Error: %s", SDL_GetError());
    return 1;
  }

  gContext = SDL_GL_CreateContext(gWindow);
  if(gContext == NULL){
    println("Window could not be created! SDL Error: %s", SDL_GetError());
    return 1;
  }

  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if(gRenderer == NULL){
    println("Renderer could not be created! SDL Error: %s", SDL_GetError());
    return 1;
  }

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if(glewError != GLEW_OK){
    println("Error initializing GLEW! %s", glewGetErrorString(glewError));
    return 1;
  }

  println("Initializing GL");
  glClearColor(0.0, 0.3, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  if(!renderer.create() && !renderer.begin(300, 300)){
    println("Unable to initialize OpenGL");
    return 1;
  }
  println("Initialized GL");

  float scaleX, scaleY;
  getDisplayScaleFactor(scaleX, scaleY);
  
  ui.set_item_height(15);
  ui.create(&gPlatform, &renderer);

  println("Building UI");
  rollout = ui.create_rollout("TEST", WND_STYLE);
  ui.insert_rollout(rollout, 0.2f, true, ui.get_root_rollout());  

  println("Loading font");
  const char* font_path = "DroidSans.ttf";
  ui.font(font_path, 15);

  SDL_StartTextInput();
  println("setup done, entering render loop");
  emscripten_set_main_loop(loop, 0, 1);

  return 0;
}
