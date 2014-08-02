#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#define GLSL(src) "#version 330\n" #src

class Window
{
private:
  GLFWwindow * _window;
public:
  void static init()
  {
    if(glfwInit() == GL_FALSE)
    {
      throw 0;
    }
  }
  void static terminate()
  {
    glfwTerminate();
  }
  void static events()
  {
    glfwPollEvents();
  }
  Window(int width, int height, const char * title)
  {
    _window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (_window == NULL)
    {
      terminate();
      throw 0;
    }
  }
  void current() const
  {
    glfwMakeContextCurrent(_window);
  }
  bool closing() const
  {
    return glfwWindowShouldClose(_window);
  }
  void swap() const
  {
    glfwSwapBuffers(_window);
  }
  void destroy()
  {
    glfwDestroyWindow(_window);
  }
};


template <GLenum TYPE>
class Shader
{
private:
  GLuint _id;
public:
  Shader()
  {
    _id = glCreateShader(TYPE);
    if(_id == 0)
    {
      throw 0;
    }
  }
  void source(GLsizei count, const GLchar * const * string, const GLint * length)
  {
    glShaderSource(_id, count, string, length);
  }
  void source(GLchar const *cstr, GLint length)
  {
    source(1, &cstr, &length);
  }
  void source(const std::string& string)
  {
    source(string.c_str(), string.size());
  }
  void source(const std::ifstream& f)
  {
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::cout << buffer.str();
    source(buffer.str());
  }
  void compile()
  {
    glCompileShader(_id);
  }
  bool status()
  {
    GLint status;
    get(GL_COMPILE_STATUS, &status);
    return (status == GL_TRUE);
  }
  void info(std::string& string)
  {
    GLint length;
    get(GL_INFO_LOG_LENGTH, &length);
    string.resize(length);
    glGetShaderInfoLog(_id, length, &length, &string[0]);
  }
  void destroy()
  {
    glDeleteShader(_id);
  }
  void get(GLenum pname, GLint * params)
  {
    glGetShaderiv(_id, pname, params);
  }

  friend class ShaderProgram;
};

class VertexShader : public Shader<GL_VERTEX_SHADER>{};
class FragmentShader : public Shader<GL_FRAGMENT_SHADER>{};


class ShaderProgram
{
public:
  GLuint _id;
public:
  ShaderProgram()
  {
    _id = glCreateProgram();
  }
  template <GLenum TYPE>
  void attach(const Shader<TYPE>& shader)
  {
    glAttachShader(_id, shader._id);
  }
  void link()
  {
    glLinkProgram(_id);
  }
  void use()
  {
    glUseProgram(_id);
  }
  void get(GLenum pname, GLint * params)
  {
    glGetProgramiv(_id, pname, params);
  }
  bool status()
  {
    GLint status;
    get(GL_LINK_STATUS, &status);
    return (status == GL_TRUE);
  }
  void info(std::string& string)
  {
    GLint length;
    get(GL_INFO_LOG_LENGTH, &length);
    string.resize(length);
    glGetProgramInfoLog(_id, length, &length, &string[0]);
  }
  template <GLenum TYPE>
  void detach(const Shader<TYPE>& shader)
  {
    glDetachShader(_id, shader._id);
  }
  void destroy()
  {
    glDeleteProgram(_id);
  }
};




template <GLuint ID>
class VertexAttribute
{
public:
  static void enable()
  {
    glEnableVertexAttribArray(ID);
  }
  static void disable()
  {
    glDisableVertexAttribArray(ID);
  }
  static void set(GLint size, GLenum type, GLboolean norm, GLsizei stride, const GLvoid * pointer)
  {
    glVertexAttribPointer(ID, size, type, norm, stride, pointer);
  }
};


class VertexArray
{
private:
  GLuint _id;
public:
  VertexArray()
  {
    glGenVertexArrays(1, &_id);
  }
  void bind()
  {
    glBindVertexArray(_id);
  }
  void destroy()
  {
    glDeleteVertexArrays(1, &_id);
  }
  static void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
  {
    glDrawElements(mode, count, type, indices);
  }
};


template <GLenum TARGET>
class Buffer
{
private:
  GLuint _id;
public:
  Buffer()
  {
    glGenBuffers(1, &_id);
  }
  void bind()
  {
    glBindBuffer(TARGET, _id);
  }
  void destroy()
  {
    glDeleteBuffers(1, &_id);
  }
  static void data(GLsizeiptr size, const GLvoid * data, GLenum usage)
  {
    glBufferData(TARGET, size, data, usage);
  }
  static void staticData(GLsizeiptr size, const GLvoid * data)
  {
    Buffer::data(size, data, GL_STATIC_DRAW);
  }
};

class ArrayBuffer : public Buffer<GL_ARRAY_BUFFER>{};
class ElementArrayBuffer : public Buffer<GL_ELEMENT_ARRAY_BUFFER>{};










static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}


int main() {

  Window::init();
  Window window(640,480,"Title");

  // select opengl version
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwSetErrorCallback(error_callback);

  window.current();


  std::string vertexSource = GLSL
  (
    layout(location = 0) in vec4 vposition;
    layout(location = 1) in vec4 vcolor;
    out vec4 fcolor;
    void main()
    {
      fcolor = vcolor;
      gl_Position = vposition;
    }
  );

  std::string fragmentSource = GLSL
  (
    in vec4 fcolor;
    layout(location = 0) out vec4 FragColor;
    void main()
    {
      FragColor = fcolor;
    }
  );



  VertexShader vertexShader;
  FragmentShader fragmentShader;
  vertexShader.source(vertexSource);
  fragmentShader.source(fragmentSource);

  vertexShader.compile();
  fragmentShader.compile();
  if(!vertexShader.status() && !fragmentShader.status())
  {
    std::string error;
    vertexShader.info(error);
    std::cerr << error;
    fragmentShader.info(error);
    std::cerr << error;
    window.destroy();
    Window::terminate();
    return 1;
  }

  ShaderProgram program;
  program.attach(vertexShader);
  program.attach(fragmentShader);
  program.link();
  if(!program.status())
  {
    std::string errorMsg;
    program.info(errorMsg);
    std::cerr << errorMsg;
    window.destroy();
    Window::terminate();
    return 1;
  }


  VertexArray va;
  va.bind();

  ArrayBuffer vb;
  vb.bind();


  GLfloat vertexData[] =
  {
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,-1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  };
  ArrayBuffer::staticData(sizeof(GLfloat)*4*6, vertexData);


  VertexAttribute<0>::enable();
  VertexAttribute<0>::set(3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));
  VertexAttribute<1>::enable();
  VertexAttribute<1>::set(3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));


  ElementArrayBuffer ib;
  ib.bind();
  GLuint indexData[] = {
    0,1,2,
    2,1,3,
  };
  ElementArrayBuffer::staticData(sizeof(GLuint)*2*3, indexData);

  while(!window.closing())
  {
    Window::events();
    glClear(GL_COLOR_BUFFER_BIT);
    program.use();
    va.bind();
    VertexArray::drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
      std::cerr << error << std::endl;
      break;
    }
    window.swap();
  }

  va.destroy();
  vb.destroy();
  ib.destroy();
  program.detach(vertexShader);
  program.detach(fragmentShader);
  vertexShader.destroy();
  fragmentShader.destroy();
  program.destroy();
  window.destroy();
  Window::terminate();
  return 0;
}
