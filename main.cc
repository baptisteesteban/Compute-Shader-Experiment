#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

GLuint vao;

/*
 * Init OpenGL
 */
bool init_glew()
{
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cerr << "Glew init: " << glewGetErrorString(err) << std::endl;
    return false;
  }
  return true;
}

void init_gl()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_POINT_SIZE);
  glPointSize(10);
}

/*
 * Load a shader source code
 */
auto load_shader(const std::string& filename)
{
  std::ifstream in(filename);
  if (!in.is_open())
    throw std::invalid_argument("Could not open " + filename);

  std::stringstream ss;
  ss << in.rdbuf();
  auto res = ss.str();

  return res;
}

/*
 * Prepare the render pipeline (vertex - fragment)
 */
GLuint init_render_shaders(void)
{
  GLint error;
  
  auto vert_src = load_shader("vertex.glsl");
  GLuint vertex_shd = glCreateShader(GL_VERTEX_SHADER);
  auto* ptr = vert_src.c_str();
  glShaderSource(vertex_shd, 1, &ptr, nullptr);
  glCompileShader(vertex_shd);
  glGetShaderiv(vertex_shd, GL_COMPILE_STATUS, &error);
  if (error == GL_FALSE)
    throw std::runtime_error("Error while compiling fragment shader");
  
  auto frag_src = load_shader("fragment.glsl");
  GLuint fragment_shd = glCreateShader(GL_FRAGMENT_SHADER);
  ptr = frag_src.c_str();
  glShaderSource(fragment_shd, 1, &ptr, nullptr);
  glCompileShader(fragment_shd);
  glGetShaderiv(fragment_shd, GL_COMPILE_STATUS, &error);
  if (error == GL_FALSE)
    throw std::runtime_error("Error while compiling fragment shader");

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shd);
  glAttachShader(program, fragment_shd);
  glLinkProgram(program);
  
  glGetProgramiv(program, GL_LINK_STATUS, &error);
  if (error == GL_FALSE)
    throw std::runtime_error("Error while linking program shader");

  glDetachShader(program, vertex_shd);
  glDetachShader(program, fragment_shd);
  
  return program;
}

/*
 * Init compute shader
 */
GLuint init_compute_shader(void)
{
  GLint error;

  auto compute_src = load_shader("compute.glsl");
  GLuint compute_shd = glCreateShader(GL_COMPUTE_SHADER);
  auto* ptr = compute_src.c_str();
  glShaderSource(compute_shd, 1, &ptr, nullptr);
  glCompileShader(compute_shd);
  glGetShaderiv(compute_shd, GL_COMPILE_STATUS, &error);
  if (error == GL_FALSE)
    throw std::runtime_error("Error while compiling compute shader");
  
  GLuint program = glCreateProgram();
  glAttachShader(program, compute_shd);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &error);
  if (error == GL_FALSE)
    throw std::runtime_error("Error while linking program shader (compute shader)");
  glDetachShader(program, compute_shd);
  return program;
}

/*
 * Init buffer
 */
void init_buffers(void)
{
  GLfloat data[] = {0.0f, 0.0f, 0.0f};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
}

/*
 * Drawing function
 */
void draw(GLuint program)
{
  glUseProgram(program);
  glBindVertexArray(vao);
  glDrawArrays(GL_POINTS, 0, 1);
  glBindVertexArray(0);
}

/*
 * Update function
 */
void update(GLuint program)
{
  glUseProgram(program);
  glDispatchCompute(1, 1, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

/*
 * Main
 */
int main(void)
{  
  SDL_Window *window = SDL_CreateWindow("Compute Shader", 0, 0, 500, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!window)
  {
    std::cerr << "Window creation: error\n";
    return 1;
  }
  
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GLContext context = SDL_GL_CreateContext(window);
  (void) context;
  
  if (!init_glew())
    return 1;
  init_gl();

  init_buffers();

  auto program = init_render_shaders();
  auto cs_program = init_compute_shader();
  
  bool running = true;
  while(running)
  {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    if (event.type == SDL_QUIT)
	    running = false;

    glClear(GL_COLOR_BUFFER_BIT);
    update(cs_program);
    draw(program);
    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  
  return 0;
}
