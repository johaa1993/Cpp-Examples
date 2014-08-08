#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>




class Window
{
private:
	GLFWwindow * _window;
public:
	void static init()
	{
		if (glfwInit() == GL_FALSE)
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
	void current()
	{
		glfwMakeContextCurrent(_window);
		if (glewInit() != GLEW_OK)
		{
			destroy();
			terminate();
			throw 0;
		}
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
		if (_id == 0)
		{
			throw 0;
		}
	}
	void source(GLsizei count, const GLchar ** string, const GLint * length)
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


class Matrix4f
{
public:
	union
	{
		GLfloat _data[16];
		struct{float c1[4], c2[4], c3[4], c4[4];};
		struct{float e11,e21,e31,e41,e12,e22,e32,e42,e13,e23,e33,e43,e14,e24,e34,e44;};
	};
public:
	Matrix4f()
	{}
	void frustum(float n, float f, float r, float t)
	{
		zero();
		e11 = n / r;
		e22 = n / t;
		e33 = (f + n) / (n - f);
		e34 = -1.0f;
		e43 = (2.0f * f * n) / (n - f);
	}
	void zero()
	{
		memset(_data, 0, sizeof(_data));
	}
	void translate(float t1, float t2, float t3)
	{
		for (int i = 0; i < 4; i++)
		{
			c4[i] = (c1[i] * t1) + (c2[i] * t2) + (c3[i] * t3) + c4[i];
		}
	}
	void multiply(Matrix4f& m)
	{
		float result[16];
		for (int c = 0; c < 4; c++) {
			for (int r = 0; r < 4; r++) {
				int index = c * 4 + r;
				float total = 0;
				for (int i = 0; i < 4; i++) {
					int p = i * 4 + r;
					int q = c * 4 + i;
					total += m._data[p] * _data[q];
				}
				result[index] = total;
			}
		}
		for (int i = 0; i < 16; i++) {
			_data[i] = result[i];
		}
	}
	void identity()
	{
		memset(_data, 0, sizeof(_data));
		e11 = 1;
		e22 = 1;
		e33 = 1;
		e44 = 1;
	}
	void scale(GLfloat a)
	{
		for (int i = 0; i < 4; i++)
		{
			c1[i] = c1[i] * a;
			c2[i] = c2[i] * a;
			c3[i] = c3[i] * a;
		}
	}
	void rotateZ(float a)
	{
		float cosin = cos(a);
		float sinus = sin(a);
		for (int i = 0; i < 4; i++)
		{
			float t1 = c1[i];
			float t2 = c2[i];
			c1[i] = t1 * cosin - t2 * sinus;
			c2[i] = t1 * sinus + t2 * cosin;
		}
	}
	void rotateY(float a)
	{
		float cosin = cos(a);
		float sinus = sin(a);
		for (int i = 0; i < 4; i++)
		{
			float t1 = c1[i];
			float t3 = c3[i];
			c1[i] = t1 * cosin - t3 * sinus;
			c3[i] = t1 * sinus + t3 * cosin;
		}
	}
	void print()
	{
		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				std::cout << std::setprecision(2) << _data[(c * 4) + r] << "\t";
			}
			std::cout << std::endl;
		}
	}
};

template <GLuint ID>
class Uniform
{
public:
	static void matrix4f(const Matrix4f& m)
	{
		glUniformMatrix4fv(ID, 1, GL_FALSE, m._data);
	}
};




static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


int main() {

	Window::init();
	Window window(640, 480, "Title");
	glfwSetErrorCallback(error_callback);
	window.current();

	VertexShader vertexShader;
	FragmentShader fragmentShader;
	vertexShader.source(std::ifstream("vs.glsl"));
	fragmentShader.source(std::ifstream("fs.glsl"));

	vertexShader.compile();
	fragmentShader.compile();
	if (!vertexShader.status() && !fragmentShader.status())
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
	if (!program.status())
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

	ArrayBuffer vb1;
	vb1.bind();
	GLfloat vertexData[] =
	{
		//  X     Y     Z           R     G     B
		// face 0:
		1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // vertex 0
		-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // vertex 1
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // vertex 2
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // vertex 3

		// face 1:
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // vertex 0
		1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // vertex 1
		1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // vertex 2
		1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // vertex 3

		// face 2:
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // vertex 0
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // vertex 1
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // vertex 2
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // vertex 3

		// face 3:
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // vertex 0
		1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // vertex 1
		-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // vertex 2
		-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // vertex 3

		// face 4:
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // vertex 0
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // vertex 1
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // vertex 2
		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // vertex 3

		// face 5:
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // vertex 0
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // vertex 1
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, // vertex 2
		-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, // vertex 3
	};
	ArrayBuffer::staticData(sizeof(vertexData), vertexData);
	VertexAttribute<0>::enable();
	VertexAttribute<0>::set(3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (char*)0);
	VertexAttribute<1>::enable();
	VertexAttribute<1>::set(3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (char*)0 + 3 * sizeof(GLfloat));










	ElementArrayBuffer ib;
	ib.bind();
	GLuint indexData[] = {
		// face 0:
		0, 1, 2,      // first triangle
		2, 1, 3,      // second triangle
		// face 1:
		4, 5, 6,      // first triangle
		6, 5, 7,      // second triangle
		// face 2:
		8, 9, 10,     // first triangle
		10, 9, 11,    // second triangle
		// face 3:
		12, 13, 14,   // first triangle
		14, 13, 15,   // second triangle
		// face 4:
		16, 17, 18,   // first triangle
		18, 17, 19,   // second triangle
		// face 5:
		20, 21, 22,   // first triangle
		22, 21, 23,   // second triangle
	};
	ElementArrayBuffer::staticData(sizeof(indexData), indexData);


	
	glEnable(GL_DEPTH_TEST);
	//glFrontFace(GL_CW);
	//glDepthFunc(GL_LESS);

	Matrix4f mvp;

	float a = 0;
	while (!window.closing())
	{
		Window::events();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		program.use();


		a += 0.005f;
		mvp.frustum(1.0f, 200.0f, 1.0f, 1.2f);
		
		mvp.translate(0, 0, -3 + tan(a));
		mvp.rotateY(a);
		mvp.rotateZ(tan(a));
		
		Uniform<2>::matrix4f(mvp);
		mvp.print();
		system("cls");

		va.bind();
		VertexArray::drawElements(GL_LINES, 36, GL_UNSIGNED_INT, (char*)0);



		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << error << std::endl;
			break;
		}
		window.swap();
	}

	va.destroy();
	vb1.destroy();
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
