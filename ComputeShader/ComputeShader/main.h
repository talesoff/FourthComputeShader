#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <Windows.h>

#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glut.h"
#include "GL/freeglut.h"
#include <iostream>
#include <io.h>
// This includes the new stuff, supplied by the application

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 #include <fstream>

//In an initialization routine
using namespace std;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

GLuint renderHandle, computeHandle;

int frame=0;

void InitGL();

// Return handles
GLuint GenTexture();
GLuint GenRenderProg(GLuint); // Texture as the param
GLuint GenComputeProg(GLuint);

void UpdateTex(int);
void Draw();

void CheckErrors(std::string);

GLuint GenComputeProg(GLuint texHandle);

void Display();
void Idle();

char* SourceRead(char* fileName)
{
	FILE* p_Sourcefile = NULL;
	char* contents = NULL;
	int length =0;
	if(fileName !=NULL)
	{
		int tempfile = _open(fileName, _A_RDONLY);

		length = _lseek(tempfile,0,SEEK_END);

		_close(tempfile);
		p_Sourcefile = fopen(fileName,"rt");
		if( p_Sourcefile != NULL)
		{
			if(0<length)
			{
				contents = (char*)malloc(sizeof(char)*(length+1));
				length = (int) fread(contents,sizeof(char),length,p_Sourcefile);
				contents[length] = '\0';
			}
			fclose(p_Sourcefile);
		}
	}
	return contents;
}