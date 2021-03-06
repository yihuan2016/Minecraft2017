#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cube.h>
#include "camera.h"
#include <scene/scene.h>
#include <scene/chunk.h>
#include <scene/plusmark.h>
#include<scene/chunkmanager.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QtMultimedia/QSound>


const float gravityacc=0.5;    //added by Xi Yang
const float timestep=0.1;      //added by Xi Yang

class MyGL
    : public GLWidget277
{
private:
    Cube geom_cube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_tex;
    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;
    Scene scene;

    Perlin* perlin_generator = new Perlin();
    int oldTime = 27;

    bool isSnowPlaying = false;
    bool isWaterPlaying = false;
    QSound* SnowPlayer = new QSound("/Users/Groverchen/Downloads/person_running_on_snow.wav");
    QSound* WaterPlayer = new QSound("/Users/Groverchen/Downloads/walking_through_and_splashing_in_water.wav");

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;
    ChunkManager chunkManager;
    Chunk chunk;
    int oldlowerBounderX=0,oldlowerBounderY=0,oldlowerBounderZ=0,
    oldhigherBoundX=-1,oldhigherBoundY=-1,oldhigherBoundZ=-1;
    PlusMark geom_plus;   //add by Xi Yang
    int timecount;
    float velocity;
    bool isJumping;
    bool first_rend=true;
    glm::vec3 oeye;

    bool isFlying;       //added by Xi Yang
    float flyingHeight;
    int collisiondirecition;
public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void GLDrawScene();
    ///------add by Yi Huang-----
    void updateChunk();
    void pushBlock(Chunk* chunk);
    void updateBlock(Block block);
    void createOrigChunk();
    void minusBoundX();
    void addBoundX();
    void minusBoundZ();
    void addBoundZ();
    void snowEffect();
    ///-----added by Xi Yang---------
    /// -----------------------------
    void BreakBlock();
    void PlaceBlock();
    void TranslateAlongLook_new(float amt);
    void TranslateAlongRight_new(float amt);
    void GroundCheck();
    float PlayerFalling(int t);
    void PlayerJump(bool);
    bool IfGrounded();
    bool IfCollision();
    void PlayerCollide();


    ///------------------------------
protected:
    void keyPressEvent(QKeyEvent *e);
    ///-----added by Xi Yang---------
    /// -----------------------------
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    ///------------------------------

private slots:
    /// Slot that gets called ~60 times per second
    virtual void timerUpdate();
};


#endif // MYGL_H
