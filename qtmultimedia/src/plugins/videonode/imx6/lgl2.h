#ifndef __L_GL2_H__
#define __L_GL2_H__

// this file comes from <GLES2/gl2.h> and <GLES2/gl2ext.h>,
// it's a light gl2.h.

/*-------------------------------------------------------------------------
 * Definition of KHRONOS_APIENTRY
 *-------------------------------------------------------------------------
 * This follows the return type of the function  and precedes the function
 * name in the function prototype.
 */
#if defined(_WIN32) && !defined(_WIN32_WCE) && !defined(__SCITECH_SNAP__)
     /* Win32 but not WinCE */
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APIENTRY
#endif

#ifndef GL_APIENTRY
#   define GL_APIENTRY KHRONOS_APIENTRY
#endif

#ifndef GL_APIENTRYP
#   define GL_APIENTRYP GL_APIENTRY*
#endif

typedef void GLvoid;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLsizei;

#define GL_VIV_YV12                                             0x8FC0
#define GL_VIV_NV12                                             0x8FC1
#define GL_VIV_YUY2                                             0x8FC2
#define GL_VIV_UYVY                                             0x8FC3
#define GL_VIV_NV21                                             0x8FC4
#define GL_VIV_I420                                             0x8FC5

typedef void (GL_APIENTRYP PFNGLTEXDIRECTVIVMAPPROC) (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid ** Logical, const GLuint * Physical);
typedef void (GL_APIENTRYP PFNGLTEXDIRECTMAPVIVPROC) (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid ** Logical, const GLuint * Physical);
typedef void (GL_APIENTRYP PFNGLTEXDIRECTVIVPROC) (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid ** Pixels);
typedef void (GL_APIENTRYP PFNGLTEXDIRECTINVALIDATEVIVPROC) (GLenum Target);
typedef void (GL_APIENTRYP PFNGLTEXDIRECTTILEDMAPVIVPROC) (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid ** Logical, const GLuint * Physical);

#endif
