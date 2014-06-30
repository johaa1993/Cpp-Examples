#include <GLFW/glfw3.h>
#include <stdlib.h>



struct Pixel
{
  float r;
  float g;
  float b;
};

const int width = 640;
const int height = 480;




int main(void)
{

  GLFWwindow* window;
  /* Initialize the library */
  if (!glfwInit())
  {
    return -1;
  }


  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);




  Pixel * pixels = new Pixel[width*height];



  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    for(int y=0;y<height;y++)
    for(int x=0;x<width;x++)
    {
      Pixel * pixel = &pixels[y*width+x];
      pixel->r = 1.0;
      pixel->g = drand48();
      pixel->b = 1.0;
    }


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawPixels(width,height,GL_RGB,GL_FLOAT,pixels);


  /* Swap front and back buffers */
  glfwSwapBuffers(window);

  /* Poll for and process events */
  glfwPollEvents();
  }

  glfwTerminate();
}
