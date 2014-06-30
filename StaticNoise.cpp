#include <GLFW/glfw3.h>
#include <stdlib.h>



int main(void)
{

  GLFWwindow* window;
  /* Initialize the library */
  if (!glfwInit())
  {
    return -1;
  }


  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);



  const int size = 640*480*3;
  float * pixels = new float[size];



  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {

    for(int i=0;i<size;i++)
    {
      pixels[i] = drand48();
    }


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawPixels(650,480,GL_RGB,GL_FLOAT,pixels);


  /* Swap front and back buffers */
  glfwSwapBuffers(window);

  /* Poll for and process events */
  glfwPollEvents();
  }

  glfwTerminate();
}
