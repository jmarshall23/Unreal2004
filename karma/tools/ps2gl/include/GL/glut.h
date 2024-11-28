/*           Copyright (C) 2001 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */

#ifndef ps2gl_glut_h
#define ps2gl_glut_h

#include "GL/gl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GLUT_NOT_VISIBLE	0
#define GLUT_VISIBLE		1

#define GLUT_RGB		0
#define GLUT_RGBA		GLUT_RGB
#define GLUT_INDEX		1
#define GLUT_SINGLE		0
#define GLUT_DOUBLE		2
#define GLUT_ACCUM		4
#define GLUT_ALPHA		8
#define GLUT_DEPTH		16
#define GLUT_STENCIL		32

#define GLUT_KEY_LEFT		100
#define GLUT_KEY_UP		101
#define GLUT_KEY_RIGHT		102
#define GLUT_KEY_DOWN		103
#define GLUT_KEY_PAGE_UP	104
#define GLUT_KEY_PAGE_DOWN	105
#define GLUT_KEY_HOME		106
#define GLUT_KEY_END		107
#define GLUT_KEY_INSERT		108

#define GLUT_ELAPSED_TIME	700

extern void glutInit(int *argcp, char **argv);
extern void glutInitDisplayMode(unsigned int mode);
extern int glutCreateWindow(const char *title);
extern void glutInitWindowPosition(int x, int y);
extern void glutInitWindowSize(int width, int height);

extern void glutMainLoop(void);
extern void glutPostRedisplay(void);
extern void glutSwapBuffers(void);

extern int glutGet(GLenum type);

extern void glutDisplayFunc(void (*func)(void));
extern void glutReshapeFunc(void (*func)(int width, int height));
extern void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
extern void glutMouseFunc(void (*func)(int button, int state, int x, int y));
extern void glutMotionFunc(void (*func)(int x, int y));
extern void glutPassiveMotionFunc(void (*func)(int x, int y));
extern void glutSpecialFunc(void (*func)(int key, int x, int y));
extern void glutEntryFunc(void (*func)(int state));
extern void glutVisibilityFunc(void (*func)(int state));
extern void glutIdleFunc(void (*func)(void));


#ifdef __cplusplus
}
#endif

#endif // ps2gl_glut_h
