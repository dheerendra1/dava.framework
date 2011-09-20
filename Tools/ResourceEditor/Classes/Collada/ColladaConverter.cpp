// ColladaConverter.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

#include "FCollada.h"
#include "ColladaDocument.h"
#include "CommandLineParser.h"
#include "AutoreleasePool.h"
#include "DAVAEngine.h"

enum{	
	PARAM_SOURCE_FILEPATH = 1,

};


using namespace DAVA;

#include <GL/glew.h>

#if defined(_WIN32)
#	include <GL/glut.h>
#	include <direct.h>
#	define getcwd _getcwd

#elif defined(__APPLE__) || defined(MACOSX)
#	include <GLUT/glut.h>
#endif


// 
ColladaDocument colladaDocument;

int angle = 0;
int distance = -50;

int specialKey;
int leftButtonState;
int mouseStartX;
int mouseStartY;
int mouseCurrentX;
int mouseCurrentY;

int width = 640;
int height = 640;

int anim = 0;

void processKeyboard(unsigned char key, int x, int y)
{
	if ((key >= 49) && (key <= 49 + 9))
	{
		anim = key - 49;
		colladaDocument.colladaScene->SetExclusiveAnimation(anim);
	}
	
	if (key == 'a')
	{
		anim++;
		if (anim >= colladaDocument.colladaScene->colladaAnimations.size())
			anim = 0;
		colladaDocument.colladaScene->SetExclusiveAnimation(anim);
	}
	else if (key == 'q')
	{
		anim--;
		if (anim < 0)
			anim = colladaDocument.colladaScene->colladaAnimations.size() - 1;
		colladaDocument.colladaScene->SetExclusiveAnimation(anim);
		
	}
	//printf("key pressed: %d\n
}

void processMouse(int button, int state, int x, int y) 
{
	specialKey = glutGetModifiers();
	if (state == GLUT_DOWN)
	{
		mouseCurrentX = mouseStartX = x;
		mouseCurrentY = mouseStartY = y;
	}
	if (state == GLUT_UP)
	{

	}

}

void processMouseActiveMotion(int x, int y) 
{
	if (specialKey == GLUT_ACTIVE_SHIFT) 
	{
		distance += (y - mouseCurrentY);
	}
	
	{
		angle += (x - mouseCurrentX);
		
		if (angle < 0) angle += 360;
		if (angle > 360)angle -= 360;

	}
	mouseCurrentX = x;
	mouseCurrentY = y;

//	glutPostRedisplay();
}


void processMousePassiveMotion(int x, int y) 
{

}


void renderScene(void) 
{
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0, 1.0f, 5.0, 5000.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0f, 0.0f, distance);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glRotatef(-70.0f, 1.0f, 0.0f, 0.0f);
	//glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE);

	//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	
	colladaDocument.Render();


	if (glGetError() != GL_NO_ERROR)
	{
		printf("OpenGL Error: \n");
	}

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}


void FrameworkDidLaunched()
{
	Vector<String> & commandLine = Core::Instance()->GetCommandLine();
		
	CommandLineParser parser(commandLine);
	
	FCollada::Initialize();
	
	if (CommandLineParser::Instance()->GetParamCount() < 2)
	{
		printf("*** Usage: ColladaConverter.exe [collada_in_file]\n");
		printf("Flags: \n");
		printf("\t--preview\t\tshow 3D preview of export scene\n");
		return;
	}else
	{
		for (int k = 0; k < CommandLineParser::Instance()->GetParamCount(); ++k)
		{
			printf("[file] - %s\n", CommandLineParser::Instance()->GetParam(k).c_str());
		}
	}
	
	
	
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Collada Loader Preview");
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
	if (GLEW_ARB_multitexture)
	{
		printf("Multitexture extension supported: %p\n", glMultiTexCoord2fARB);
	}
	
	glutKeyboardFunc(processKeyboard); 
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	//	glutEntryFunc(processMouseEntry);
	
	char workpath[256];
	getcwd(workpath, 256);
	
	String sourceFilePath = String(workpath) + String("/") + CommandLineParser::Instance()->GetParam(PARAM_SOURCE_FILEPATH);
	sourceFilePath = CommandLineParser::RealPath(sourceFilePath);
	
	String sourcePath;
	String sourceFilename;
	CommandLineParser::SplitFilePath(sourceFilePath, sourcePath, sourceFilename);
	
	printf("Opening File: %s workpath:%s filepath:%s filename: %s\n", sourceFilePath.c_str(), workpath, sourcePath.c_str(), sourceFilename.c_str());
	
	if (!colladaDocument.Open(sourceFilePath.c_str()))
	{
		printf("*** ERROR: Failed to read %s\n", sourceFilePath.c_str());
		return;
	}
	
	int paramCount = CommandLineParser::Instance()->GetParamCount();
	for (int k = PARAM_SOURCE_FILEPATH + 1; k < paramCount; ++k)
	{
		colladaDocument.ExportAnimations(CommandLineParser::Instance()->GetParam(k).c_str());
	}
	
	colladaDocument.SaveScene(sourcePath, CommandLineParser::ReplaceExtension(sourceFilename, ".sce"));
	
	if (CommandLineParser::Instance()->IsFlagSet("--preview"))
	{
		glutDisplayFunc(renderScene);
		glutMainLoop();
	}
	
	FCollada::Release();
}


void FrameworkWillTerminate()
{
	
}



int main(int argc, char * argv[])
{
	glutInit(&argc, (char **)argv);

	//Core::Create();
	DAVA::Core::RunCmdTool(argc, argv);
	

	//Core::Release();

	return 0;
}

