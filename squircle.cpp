#include "squircle.h"



#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QOpenGLFunctions>

#include "camerakit.h"

#include <ctime>
clock_t start_time,end_time;



//#define MAIN_TEXTURE_WIDTH 640
//#define MAIN_TEXTURE_HEIGHT 480

#define MAIN_TEXTURE_WIDTH 640
#define MAIN_TEXTURE_HEIGHT 480

//#define MAIN_TEXTURE_WIDTH 320
//define MAIN_TEXTURE_HEIGHT 180
// MAIN_TEXTURE is YUV pixels not screen pixels...

#define TEXTURES 2  //how many images or textures?

int Pitch=MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT;
long size=Pitch*3/2;


GLuint TextureID0,TextureID1,TextureID2;
GLuint ytexture,utexture,vtexture;

extern unsigned char  *cvt_buf;

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

Squircle::Squircle()
    :  m_renderer(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    init_camera();
    camera_cce();

}

void Squircle::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
        connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
        //! [1]
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        //! [3]
        win->setClearBeforeRendering(false);
    }
}

//! [6]
void Squircle::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
        camera_release();
        m_renderer->cleanup2();
    }
}

void Squircle::repaint(void)
{
    if (m_renderer) {
        camera_cce();
        m_renderer->paint();
        if (window())
            window()->update();

    }
}

SquircleRenderer::~SquircleRenderer()
{
    delete m_program;
}
//! [6]

//! [9]
void Squircle::sync()
{
    if (!m_renderer) {
        m_renderer = new SquircleRenderer();
        m_renderer->Init_opengl();
        connect(window(), SIGNAL(beforeRendering()), m_renderer, SLOT(paint()), Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
}
//! [9]

//! [4]

void SquircleRenderer::Init_opengl()
{
    initializeOpenGLFunctions();
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 vertexIn;"
                                           "attribute highp vec2 textureIn;"
                                           "varying vec2 textureOut;"
                                           "void main(void) {"
                                           "gl_Position = vertexIn;"
                                           "textureOut = textureIn;}");

        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "varying vec2 textureOut;"
                                           "uniform sampler2D tex_y;"
                                           "uniform sampler2D tex_u;"
                                           "uniform sampler2D tex_v;"
                                           "void main(void){"
                                           "vec3 yuv;"
                                           "vec3 rgb;"
                                           "yuv.x = texture2D(tex_y, textureOut).r;"
                                           "yuv.y = texture2D(tex_u, textureOut).r - 0.5;"
                                           "yuv.z = texture2D(tex_v, textureOut).r - 0.5;"
                                           "rgb = mat3( 1,       1,         1,"
                                           "0,       -0.39465,  2.03211,"
                                           "1.13983, -0.58060,  0) * yuv;"
                                           "gl_FragColor = vec4(rgb, 1);}" );

        m_program->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
        m_program->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
        m_program->link();
    }
    glGenTextures(1, &ytexture);
    glBindTexture(GL_TEXTURE_2D, ytexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glGenTextures(1, &utexture);
    glBindTexture(GL_TEXTURE_2D, utexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glGenTextures(1, &vtexture);
    glBindTexture(GL_TEXTURE_2D, vtexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void SquircleRenderer::paint()
{

    m_program->bind();

    TextureID0=m_program->uniformLocation("tex_y");
    TextureID1=m_program->uniformLocation("tex_u");
    TextureID2=m_program->uniformLocation("tex_v");


    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };


    m_program->setAttributeArray(ATTRIB_VERTEX, GL_FLOAT, vertexVertices, 2);
    m_program->enableAttributeArray(ATTRIB_VERTEX);


    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };


    m_program->setAttributeArray(ATTRIB_TEXTURE, GL_FLOAT, textureVertices, 2);
    m_program->enableAttributeArray(ATTRIB_TEXTURE);



    unsigned char *y = cvt_buf, *u = y + Pitch, *v = u + (Pitch >> 2); // ie PITCH*HEIGHT16*1/4

    //    FILE *fw;
    //    fw=fopen("dat.txt","w");
    //    fwrite(&cvt_buf,sizeof(cvt_buf),size,fw);
    //    fclose(fw);


    //    start_time=clock();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ytexture);
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, y);
    m_program->setUniformValue(TextureID0, 0);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, utexture);
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, MAIN_TEXTURE_WIDTH/2, MAIN_TEXTURE_HEIGHT/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u);
    m_program->setUniformValue(TextureID1, 1);


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vtexture);
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, MAIN_TEXTURE_WIDTH/2, MAIN_TEXTURE_HEIGHT/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v);
    m_program->setUniformValue(TextureID2, 2);


    //    end_time=clock();
    //    printf("paint cost %f ms \n", (double)(end_time - start_time) / CLOCKS_PER_SEC*1000);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


void SquircleRenderer::cleanup2()
{

    camera_release();
    m_program->disableAttributeArray(ATTRIB_VERTEX);
    m_program->disableAttributeArray(ATTRIB_VERTEX);
    m_program->release();
}

//! [5]
