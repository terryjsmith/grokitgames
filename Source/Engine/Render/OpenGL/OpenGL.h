
#ifndef opengl_h
#define opengl_h

#ifdef WIN32

#include <GL/gl3w.h>

#else

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#endif

#ifdef DEBUG
#define GL_CHECK(stuff) stuff; { GLenum error = glGetError(); if(error) { printf("GL error %d\n", error); assert(error == 0); } }
#else
#define GL_CHECK(stuff) stuff;
#endif

// OSX stop telling me OpenGL is deprecated; I know, and I'm still mad about it
#define GL_SILENCE_DEPRECATION  1

void __stdcall OGLDebugMessage(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam);

#endif
