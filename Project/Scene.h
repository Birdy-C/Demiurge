#pragma once
#include "Win32_GLAppUtil.h"
struct Scene
{
	int     numModels;
	Model * Models[10];

	void    Add(Model * n)
	{
		Models[numModels++] = n;
	}

	void Render(Matrix4f view, Matrix4f proj)
	{
		for (int i = 0; i < numModels; ++i)
			Models[i]->Render(view, proj);
	}

	//GLuint CreateShader(GLenum type, const GLchar* src)
	//{
	//    GLuint shader = glCreateShader(type);

	//    glShaderSource(shader, 1, &src, NULL);
	//    glCompileShader(shader);

	//    GLint r;
	//    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	//    if (!r)
	//    {
	//        GLchar msg[1024];
	//        glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
	//        if (msg[0]) {
	//            OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
	//        }
	//        return 0;
	//    }

	//    return shader;
	//}


	GLuint CreateShader(GLenum type, const char* path);


	void Init(int includeIntensiveGPUobject);

	Scene() : numModels(0) {}
	Scene(bool includeIntensiveGPUobject) :
		numModels(0)
	{
		Init(includeIntensiveGPUobject);
	}
	void Release()
	{
		while (numModels-- > 0)
			delete Models[numModels];
	}
	~Scene()
	{
		Release();
	}
};
