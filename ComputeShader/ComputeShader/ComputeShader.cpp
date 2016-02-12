// ThirdComputeShader.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "main.h"

int _tmain(int argc, _TCHAR* argv[])
{
	glutInit(&argc,(char**)argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(1200,1200);
	glutInitWindowPosition(600,0);
	glutCreateWindow("Compute shader");
	glViewport(0,0,1200,1200);
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	InitGL();
	
	GLuint texHandle = GenTexture();
	renderHandle = GenRenderProg(texHandle);
	//computeHandle = genComputeProg(texHandle);


	glutDisplayFunc(Display);
	while(1)
	{
		glutMainLoopEvent();
		Idle();
	}

	//r (int i = 0; i < 1024; ++i) {
	//pdateTex(i);
	//raw();
	//
	


	return 0;
}

void UpdateTex(int frame) {

	glUseProgram(computeHandle);
	glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)frame*0.01f);
	glDispatchCompute(1024/16, 1024/16, 1); // 512^2 threads in blocks of 16^2
	CheckErrors("Dispatch compute shader");
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//updateTex(frame++);
	
	glUseProgram(renderHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	CheckErrors("Draw screen");

	glutSwapBuffers();
	glutPostRedisplay();
}

GLuint GenComputeProg(GLuint texHandle) {
	// Creating the compute shader, and the program object containing the shader
    GLuint progHandle = glCreateProgram();
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

	// In order to write to a texture, we have to introduce it as image2D.
	// local_size_x/y/z layout variables define the work group size.
	// gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
	// gl_LocalInvocationID is the local index within the work group, and
	// gl_WorkGroupID is the work group's index
	
	char *computeShaderSource;
	char computeShader[255]={"shader/"}; 
	strcat_s(computeShader,"compShader.comp");
	

	computeShaderSource = SourceRead(computeShader);

	const char* p_compute_source = computeShaderSource;


    glShaderSource(cs, 2, &p_compute_source, NULL);
	glCompileShader(cs);
    int rvalue;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(cs, 10239, &length, log);
        fprintf(stderr, "Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(progHandle, cs);

    glLinkProgram(progHandle);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(progHandle, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        exit(41);
    }   
	glUseProgram(progHandle);
    
	GLdouble vtx0[9] = {-0.0006, 0.0003, 0.1120 , -0.0008, -0.0003, 0.0920, 0.0006, 0.0004, 0.1022 };  
	glUniform3dv(glGetUniformLocation(progHandle, "vertex"),3,vtx0 );

	GLdouble carrierWave[3] = {0,0,100};  
	glUniform3d(glGetUniformLocation(progHandle, "carrierWave"),carrierWave[0],carrierWave[1],carrierWave[2]);

	GLdouble pixelPitch[2] = {(1.0e-05)*0.4, (1.0e-05)*0.4};  
	glUniform2d(glGetUniformLocation(progHandle, "pixelPitch"),pixelPitch[0],pixelPitch[1]);

	GLdouble pixelNumber[2] = {1024,1024};  
	glUniform2d(glGetUniformLocation(progHandle, "pixelNumber"),pixelNumber[0],pixelNumber[1]);

	GLdouble waveLength = {6.6000e-07};  
	glUniform1d(glGetUniformLocation(progHandle, "waveLength"),waveLength);

	CheckErrors("Compute shader");
	return progHandle;
}

GLuint GenRenderProg(GLuint texHandle) {
    GLuint progHandle = glCreateProgram();
    GLuint vp = glCreateShader(GL_VERTEX_SHADER);
    GLuint fp = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vpSrc[] = {
		"#version 430\n",
		"in vec2 pos;\
		 out vec2 texCoord;\
		 void main() {\
			 texCoord = pos*0.5f + 0.5f;\
			 gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\
		 }"
	};

	const char *fpSrc[] = {
		"#version 430\n",
		"uniform sampler2D srcTex;\
		 in vec2 texCoord;\
		 out vec4 color;\
		 void main() {\
			 float r = texture(srcTex, texCoord).x;\
			 float g = texture(srcTex, texCoord).y;\
			 float b = texture(srcTex, texCoord).z;\
			 float a = texture(srcTex, texCoord).a;\
			 color = vec4(r, 0, 0, 1);\
		 }"
	};

    glShaderSource(vp, 2, vpSrc, NULL);
    glShaderSource(fp, 2, fpSrc, NULL);

    glCompileShader(vp);
    int rvalue;
    glGetShaderiv(vp, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling vp\n");
        exit(30);
    }
    glAttachShader(progHandle, vp);

    glCompileShader(fp);
    glGetShaderiv(fp, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling fp\n");
        exit(31);
    }
    glAttachShader(progHandle, fp);

    glLinkProgram(progHandle);

    glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking sp\n");
        exit(32);
    }   
    
	glUseProgram(progHandle);
	
	GLuint posBuf;
	glGenBuffers(1, &posBuf);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	float data[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, data, GL_STREAM_DRAW);		//버퍼 만들어서 data 채워 넣기
	GLint posPtr = glGetAttribLocation(progHandle, "pos");						//shader 내의 pos 로의 접근자 만들기
    glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);					//pos에 2x2개의 vertex가 들어감을 알려줌. 
    glEnableVertexAttribArray(posPtr);											//vertex attribArray를 켬.

	CheckErrors("Render shaders");
	return progHandle;
}

GLuint GenTexture() {
	// We create a single float channel 512^2 texture
	GLuint texHandle;
	glGenTextures(1, &texHandle);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1024, 1024, 0, GL_RED, GL_FLOAT, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_V3F , NULL);

	// Because we're also using this tex as an image (in order to write to it),
	// we bind it to an image unit as well
	glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	//glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	CheckErrors("Gen texture");
	return texHandle;
}

void CheckErrors(std::string desc) {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n", desc.c_str(), gluErrorString(e), e);
		exit(20);
	}
}

void InitGL() 
{

	
	glewInit();

	// Finding the compute shader extension
	int extCount;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
	bool found = false;
	for (int i = 0; i < extCount; ++i)
		if (!strcmp((const char*)glGetStringi(GL_EXTENSIONS, i), "GL_ARB_compute_shader")) {
			printf("Extension \"GL_ARB_compute_shader\" found\n");
			found = true;
			break;
		}

	if (!found) {
		fprintf(stderr, "Extension \"GL_ARB_compute_shader\" not found\n");
		exit(14);
	}

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	CheckErrors("Window init");
}

void Idle()
{
	glutPostRedisplay();
}