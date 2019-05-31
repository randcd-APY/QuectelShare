#include "glyuvtorgb.h"
#include "util.h"
#include <unistd.h>
#include <sys/time.h>
#define VERTEXIN 0
#define TEXTUREIN 1

static int difftimeval(const struct timeval *start, const struct timeval *end)
{
        double d;
        time_t s;
        suseconds_t u;

        s = end->tv_sec - start->tv_sec;
        u = end->tv_usec - start->tv_usec;
        //if (u < 0)
        //        --s;

        d = s;
        d *= 1000000;//1 ¿ = 10^6 ¿¿
        d += u;

        return d;
}

GLYuvToRgb::GLYuvToRgb(int width, int height)
{
    pixels = (unsigned char *)malloc(width*height*4);
    memset(pixels, 0, width*height*4);

    videoW = width;
    videoH = height;
}
 
GLYuvToRgb::~GLYuvToRgb()
{
	free(pixels);
	egl_destroy(eDisplay, eSurface);
}
 
void GLYuvToRgb::initializeGL()
{
    static const GLfloat vertexs[] {
        //顶点坐标
        -1.0f,-1.0f,
        -1.0f,+1.0f,
        +1.0f,+1.0f,
        +1.0f,-1.0f,
	};

	static const GLfloat textures[] {
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
    };
 
 
    const char *vsrc =
   "attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
    {                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }";

    const char *fsrc = "varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_uv; \
    void main(void) \
    { \
		   float y, u, v, red, green, blue; \
         y = texture2D(tex_y, textureOut).r ; \
         u = texture2D(tex_uv, textureOut).r - 0.5; \
     	 v = texture2D(tex_uv, textureOut).a - 0.5; \
        red = y+1.5958*v; \
        green = y-0.39173*u-0.81290*v; \
        blue = y+2.017*u; \
        gl_FragColor = vec4(red, green, blue, 1.0); \
    }";

    const EGLint attribs[] = {
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_DEPTH_SIZE, 0,
            EGL_NONE
    };
    EGLint w, h, dummy;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    //调用eglGetDisplay
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY); // 第一步

    eglBindAPI(EGL_OPENGL_ES_API);
    eglInitialize(display, 0, 0);

    eglChooseConfig(display, attribs, &config, 1, &numConfigs); // 第三步
    //调用eglCreateWindowSurface将Surface s转换为本地窗口，
    //surface = eglCreateWindowSurface(display, config, mSurface.get(), NULL); // 第四步

    EGLint eglSurfaceAttribList[] = {EGL_WIDTH, videoW,
                                   EGL_HEIGHT, videoH,
                                   EGL_NONE,
                                   EGL_NONE,
                                   EGL_NONE};

    surface = eglCreatePbufferSurface(display, config, eglSurfaceAttribList); // 第四步

    EGLint eglContextAttribList[] = {EGL_CONTEXT_CLIENT_VERSION, 3,
                                   EGL_NONE,
                                   EGL_NONE,
                                   EGL_NONE};

    context = eglCreateContext(display, config, NULL, eglContextAttribList); // 第五步

    //eglMakeCurrent后生成的surface就可以利用opengl画图了
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        return ;
    }

    eSurface = surface;
    eDisplay = display;

    pgm = load_program(vsrc, fsrc);

    vertexIn = glGetAttribLocation(pgm, "vertexIn");
    textureIn = glGetAttribLocation(pgm, "textureIn");

    glVertexAttribPointer(vertexIn, 2, GL_FLOAT, GL_FALSE, 0, vertexs);  //设置顶点坐标
    glVertexAttribPointer(textureIn, 2, GL_FLOAT, GL_FALSE, 0, textures); //设置纹理坐标
    glEnableVertexAttribArray(vertexIn);
    glEnableVertexAttribArray(textureIn);

    textureUniformY = glGetUniformLocation(pgm, "tex_y");
    textureUniformUV = glGetUniformLocation(pgm, "tex_uv");

    glViewport(0, 0, videoW, videoH);
//generate tuxture

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &idY);
    //glGenTextures(1, &idU);
    //glGenTextures(1, &idV);
    glBindTexture(GL_TEXTURE_2D, idY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoW, videoH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idY, 0);

    glGenTextures(1, &idUV);
    glBindTexture(GL_TEXTURE_2D, idUV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, videoW, videoH, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idUV, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, videoW, videoH);  //this color format must be same with glReadPixels
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("check framebuffer status = 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
 
unsigned char* GLYuvToRgb::slotYuv(unsigned char *yuvPtr, int width, int height)
{
//    QMatrix4x4 m;
//    m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f );//透视矩阵随距离的变化，图形跟着变化。屏幕平面中心就是视点（摄像头）,需要将图形移向屏幕里面一定距离。
//    m.ortho(-2,+2,-2,+2,-10,10);//近裁剪平面是一个矩形,矩形左下角点三维空间坐标是（left,bottom,-near）,右上角点是（right,top,-near）所以此处为负，表示z轴最大为10；
                                //远裁剪平面也是一个矩形,左下角点空间坐标是（left,bottom,-far）,右上角点是（right,top,-far）所以此处为正，表示z轴最小为-10；
                                //此时坐标中心还是在屏幕水平面中间，只是前后左右的距离已限制。
    if(yuvPtr == nullptr)
        return NULL;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(pgm);
    glEnableClientState(GL_VERTEX_ARRAY);

    glActiveTexture(GL_TEXTURE0);  //激活纹理单元GL_TEXTURE0,系统里面的
    glBindTexture(GL_TEXTURE_2D,idY); //绑定y分量纹理对象id到激活的纹理单元

    //使用内存中的数据创建真正的y分量纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvPtr);
    //https://blog.csdn.net/xipiaoyouzi/article/details/53584798 纹理参数解析
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1); //激活纹理单元GL_TEXTURE1
    glBindTexture(GL_TEXTURE_2D,idUV);
    //使用内存中的数据创建真正的u分量纹理数据
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE_ALPHA, width >> 1, height >> 1,0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, yuvPtr + width * height);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //指定y纹理要使用新值
    glUniform1i(textureUniformY, 0);
    //指定u纹理要使用新值
    glUniform1i(textureUniformUV, 1);
    //使用顶点数组方式绘制图形
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glUseProgram(0);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);  //GL_RGBA take less time than GL_RGB

    return pixels;
	
}
