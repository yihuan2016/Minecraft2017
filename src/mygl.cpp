#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <tuple>
#include <iostream>
#include<soil/Simple OpenGL Image Library/src/SOIL.h>

#define BOUNDRY 160

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cube(this),
      prog_lambert(this), prog_flat(this),prog_tex(this),
      gl_camera(),chunkManager(),chunk(this,glm::vec3(0,0,0)),
      geom_plus(this),timecount(-1),velocity(0),
      isJumping(false),oeye(0,0,0),
      isFlying(false),flyingHeight(0.0f),collisiondirecition(-1)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    geom_cube.destroy();
    chunk.destroy();
    geom_plus.destroy();

}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.7f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of Cube
    geom_cube.create();
    geom_plus.create();
    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    prog_tex.create(":/glsl/texture.vert.glsl", ":/glsl/texture.frag.glsl");
    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));
    prog_tex.setGeometryColor(glm::vec4(0,1,0,1));
    prog_tex.setupTexture();
    prog_tex.setupNorMap();
 
    setMouseTracking(true);
    QPoint mousePos(width()/2,height()/2);
    mousePos=mapToGlobal(mousePos);
    QCursor::setPos(mousePos);
    setCursor(Qt::BlankCursor);
    timecount=0;

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);

   scene.CreateTestScene();
   //updateChunk();
    createOrigChunk();
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
//    gl_camera = Camera(w, h);
    gl_camera = Camera(w, h, glm::vec3(scene.dimensions.x/2, scene.dimensions.y/2 + 25, scene.dimensions.z/2),
                           glm::vec3(scene.dimensions.x/2, scene.dimensions.y/2+25, scene.dimensions.z/2+1), glm::vec3(0,1,0));
    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    oeye=gl_camera.eye;
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);
    prog_tex.setViewProjMatrix(viewproj);
    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
//DO NOT CONSTRUCT YOUR SCENE GRAPH IN THIS FUNCTION!
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_tex.setViewProjMatrix(gl_camera.getViewProj());
  //  prog_lambert.setModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, 0, 0)));

    //prog_lambert.draw(chunk);
    //updateChunk();

    GLDrawScene();

    ///-------------added by Xi Yang-------------------
    ///----------------Draw plus mark------------------
    glDisable(GL_DEPTH_TEST);
    prog_flat.setViewProjMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(geom_plus);
    glEnable(GL_DEPTH_TEST);
    ///----------------Draw plus mark------------------


    if (scene.scene[std::make_tuple(gl_camera.eye[0], gl_camera.eye[1], gl_camera.eye[2])] == Snow ||
            scene.scene[std::make_tuple(gl_camera.eye[0], gl_camera.eye[1]-1, gl_camera.eye[2])] == Snow ||
            scene.scene[std::make_tuple(gl_camera.eye[0], gl_camera.eye[1]-2, gl_camera.eye[2])] == Snow)
    {
        if (isSnowPlaying == false)
        {
            SnowPlayer->play();
            isSnowPlaying = true;
        }
        else;
    }
    else{
        isSnowPlaying = false;
        SnowPlayer->stop();
    }

    if (scene.scene[std::make_tuple(gl_camera.eye[0], gl_camera.eye[1], gl_camera.eye[2])] == Water ||
            scene.scene[std::make_tuple(gl_camera.eye[0], gl_camera.eye[1]-1, gl_camera.eye[2])] == Water ||
            scene.scene[std::make_tuple(gl_camera.eye[0], gl_camera.eye[1]-2, gl_camera.eye[2])] == Water)
    {
        if (isWaterPlaying == false)
        {
            WaterPlayer->play();
            isWaterPlaying = true;
        }
        else;
    }
    else{
        isWaterPlaying = false;
        WaterPlayer->stop();
    }
}

void MyGL::GLDrawScene()
{
    if(chunkManager.renderList.size()!=0){
        for(Chunk* chunk:chunkManager.renderList){
            if(chunk->isActive()==true){
             prog_lambert.setModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, 0, 0)));
             prog_tex.setModelMatrix(glm::translate(glm::mat4(), glm::vec3(0, 0, 0)));
             prog_tex.useMe();
             glUniform1i(prog_tex.unifSampler,0);
             glActiveTexture(GL_TEXTURE0);
             glBindTexture(GL_TEXTURE_2D, prog_tex.textureHandle);
             glUniform1i(prog_tex.unifNormal,1);
             glActiveTexture(GL_TEXTURE1);
             glBindTexture(GL_TEXTURE_2D, prog_tex.NormalHandle);
             prog_tex.draw(*chunk);
            // prog_lambert.draw(*chunk);
            }
        }
    }
//chunkManager.renderList.clear();
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    //Perlin* perlin_generator = new Perlin();

    float amount = 1.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    }
    /* else if (e->key() == Qt::Key_W) {

        //gl_camera.TranslateAlongLook(amount);
        oldeye=gl_camera.eye;
        TranslateAlongLook_new(amount);
         updateChunk();


    } else if (e->key() == Qt::Key_S) {

        oldeye=gl_camera.eye;
        //gl_camera.TranslateAlongLook(-amount);
        TranslateAlongLook_new(-amount);
        updateChunk();

    } else if (e->key() == Qt::Key_D) {

        //gl_camera.TranslateAlongRight(amount);
       oldeye=gl_camera.eye;
        TranslateAlongRight_new(amount);
        updateChunk();


    } else if (e->key() == Qt::Key_A) {

        //gl_camera.TranslateAlongRight(-amount);
        oldeye=gl_camera.eye;
        TranslateAlongRight_new(-amount);
        updateChunk();

    } else if (e->key() == Qt::Key_Q) {
        oldeye=gl_camera.eye;
        gl_camera.TranslateAlongUp(-amount);

        scene.lowerBoundX += 1;
        scene.higherBoundX -= 1;
        scene.lowerBoundZ += 1;
        scene.higherBoundZ -= 1;

        for (int z = scene.lowerBoundZ; z < scene.higherBoundX; z++)
            for (int y = scene.lowerBoundY; y < scene.higherBoundY; y++)
            {
                scene.containBlock[std::make_tuple(scene.higherBoundX, y, z)] = false;
                scene.containBlock[std::make_tuple(scene.lowerBoundX-1, y, z)] = false;
            }

        for (int x = scene.lowerBoundX; x < scene.higherBoundX; x++)
            for (int y = scene.lowerBoundY; y < scene.higherBoundY; y++)
            {
                scene.containBlock[std::make_tuple(x, y, scene.higherBoundZ)] = false;
                scene.containBlock[std::make_tuple(x, y, scene.lowerBoundZ-1)] = false;
            }

    } else if (e->key() == Qt::Key_E) {
        oldeye=gl_camera.eye;
        gl_camera.TranslateAlongUp(amount);

        scene.lowerBoundX -= 1;
        scene.higherBoundX += 1;
        scene.lowerBoundZ -= 1;
        scene.higherBoundZ += 1;

        for (int z = scene.lowerBoundZ; z < scene.higherBoundX; z++)
        {
            int height_1 = perlin_generator->OctavePerlin((scene.higherBoundX-1)/64.0, 0, z/64.0, 6, 1) * 64;
            int height_2 = perlin_generator->OctavePerlin((scene.lowerBoundX)/64.0, 0, z/64.0, 6, 1) * 64;
            for (int i = 0; i < height_1; i++)
            {
                scene.containBlock[std::make_tuple(scene.higherBoundX-1, i, z)] = true;
                scene.scene[std::make_tuple(scene.higherBoundX-1, i, z)] = Default;
            }
            for (int i = 0; i < height_2; i++)
            {
                scene.containBlock[std::make_tuple(scene.lowerBoundX, i, z)] = true;
                scene.scene[std::make_tuple(scene.lowerBoundX, i, z)] = Default;
            }
        }
        for (int x = scene.lowerBoundX; x < scene.higherBoundX; x++)
        {
            int height_1 = perlin_generator->OctavePerlin(x/64.0, 0, (scene.higherBoundZ-1)/64.0, 6, 1) * 64;
            int height_2 = perlin_generator->OctavePerlin(x/64.0, 0, (scene.lowerBoundZ)/64.0, 6, 1) * 64;
            for (int i = 0; i < height_1; i++)
            {
                scene.containBlock[std::make_tuple(x, i, scene.higherBoundZ-1)] = true;
                scene.scene[std::make_tuple(x, i, scene.higherBoundZ-1)] = Default;
            }
            for (int i = 0; i < height_2; i++)
            {
                scene.containBlock[std::make_tuple(x, i, scene.lowerBoundZ)] = true;
                scene.scene[std::make_tuple(x, i, scene.lowerBoundZ)] = Default;
            }
        }
    } else if (e->key() == Qt::Key_R) {
        gl_camera = Camera(this->width(), this->height());
    }
    else if (e->key() == Qt::Key_Space) {
           oldeye=gl_camera.eye;
           isJumping=true;
        }*/
    else if (e->key() == Qt::Key_W) {

                    collisiondirecition=0;
                TranslateAlongLook_new(amount);
                 updateChunk();

            } else if (e->key() == Qt::Key_S) {

                collisiondirecition=1;
                TranslateAlongLook_new(-amount);
                updateChunk();

            } else if (e->key() == Qt::Key_D) {


               collisiondirecition=2;
                TranslateAlongRight_new(amount);
                updateChunk();

            } else if (e->key() == Qt::Key_A) {

                collisiondirecition=3;
                TranslateAlongRight_new(-amount);
                updateChunk();

            }else if (e->key() == Qt::Key_R) {
            gl_camera = Camera(this->width(), this->height());
    }///added by Xi Yang
    else if (e->key() == Qt::Key_Space) {
           flyingHeight=gl_camera.eye[1]+1.0;
           isJumping=true;
        }
    else if (e->key() == Qt::Key_F)
    {
        if(isFlying==false)
        {
            flyingHeight=gl_camera.eye[1]+8.0;
            collisiondirecition=4;
            isFlying=true;}
        else
           isFlying=false;

    }
    else if (e->key() == Qt::Key_E)
    {
        if(isFlying==true)
            flyingHeight+=0.2;
    }
    else if(e->key()==Qt::Key_Q)
    {
        if(isFlying==true)
            flyingHeight-=0.2;
    }
    gl_camera.RecomputeAttributes();

    update();  // Calls paintGL, among other things
}



//---------------------------------------------------------------
//-----------------------------ADD BY YI HUANG--------------------
void MyGL::pushBlock(Chunk *chunk){

    bool is_active;
    glm::vec3 pos=chunk->getPos();
    int width=chunk->CHUNK_SIZE;
    int lowerx=pos.x;
    int lowery=pos.y;
    int lowerz=pos.z;
    int highx=pos.x+width;
    int highy=pos.y+width;
    int highz=pos.z+width;
   for(int x=lowerx;x<highx;x++)
       for(int y=lowery;y<highy;y++)
           for(int z=lowerz;z<highz;z++){
            // loop every block in the map
        is_active=scene.containBlock[std::make_tuple(x,y,z)];
              //find the block belongs to which
        int cx=x-pos.x;
        int cy=y-pos.y;
        int cz=z-pos.z;
       if(is_active){
           chunk->active[cx][cy][cz]=true;
           chunk->type[cx][cy][cz]=scene.scene[std::make_tuple(x,y,z)];
        }
       else{
           chunk->active[cx][cy][cz]=false;
       }
    }


}
//-----------------------------------------------------------------
//-------------------------ADD BY YI HUANG--------------------------
void MyGL::createOrigChunk(){
    glm::vec3 pos=gl_camera.eye;
    int x=floor(pos.x/16);
    int y=floor(pos.y/16);
    int z=floor(pos.z/16);
    std::cout<<"current origin  chunk"<<x<<" "<<y<<" "<<z<<std::endl;
    glm::ivec3 lowerBound=glm::ivec3(scene.lowerBoundX,scene.lowerBoundY,scene.lowerBoundZ);
    glm::ivec3 higherBound=glm::ivec3(scene.higherBoundX,scene.higherBoundY,scene.higherBoundZ);
        int gapsize=16;
        int chunkNumx=(higherBound.x-lowerBound.x)/16;
        int chunkNumy=(higherBound.y-lowerBound.y)/16;
        int chunkNumz=(higherBound.z-lowerBound.z)/16;
        std::cout<<"size x is"<<chunkNumx<<std::endl;
    for(int i=0;i<chunkNumx;i++){
        for(int j=0;j<chunkNumy;j++){
            for(int k=0;k<chunkNumz;k++){

               int x=lowerBound.x+gapsize*i;
                int y=lowerBound.y+gapsize*j;
               int z=lowerBound.z+gapsize*k;

               glm::vec3 Chunkpos= glm::vec3(x,y,z);

                           //this chunk is new and need to be creat first
                   Chunk* chunk=new Chunk(this,Chunkpos);
                   //chunkManager.newLoadList.push_back(chunk);
                   chunkManager.updateList.push_back(chunk);
                   pushBlock(chunk);

            }

        }
    }

       chunkManager.update(gl_camera);
       first_rend=false;
       oldhigherBoundX=scene.higherBoundX;
       oldhigherBoundY=scene.higherBoundY;
       oldhigherBoundZ=scene.higherBoundZ;
       oldlowerBounderX=scene.lowerBoundX;
       oldlowerBounderY=scene.lowerBoundY;
       oldlowerBounderZ=scene.lowerBoundZ;
}
void MyGL::minusBoundX(){

    Perlin* perlin_generator=new Perlin();
    scene.higherBoundX-=16;
    scene.lowerBoundX-=16;
    int posz=floor(gl_camera.eye.z/16.0f)*16;
    int posx=floor(gl_camera.eye.x/16.0f)*16;
   for(int x=posx-16;x<posx;x++){
    for(int z = posz-16; z < posz+32; z++)
           {
//               int height = perlin_generator->OctavePerlin(x/64.0, 0, z/64.0, 6, 1) * 64;
//               for (int i = 0; i < height; i++)
//               {
//                   scene.containBlock[std::make_tuple(x, i, z)] = true;
//                   scene.scene[std::make_tuple(x, i, z)] = Default;
//               }
        int height = perlin_generator->OctavePerlin((float)x/BOUNDRY, 0, (float)z/BOUNDRY, 6, 1) * BOUNDRY;

        // height field is around ~80 with 16 as variance
        // terrain part + part between sea level and bedrock
        // COORDINATE: bedrock: 16, sea level: 80, above sea level(terrain): 160
        std::tuple<int,int,int> pos;
        pos = std::make_tuple(x, height+15, z);
        scene.containBlock[pos] = true;
        scene.scene[pos] = Grass;

        for (int i = 64; i < height+15; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Default;
        }
        for (int i = 16; i < 64; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Stone;
        }
        // At a depth of 128 blocks, a layer of BEDROCK should be generated;
        for (int i = 0; i < 16; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = BedRock;
        }
           }
   }
   scene.UpdateRiver();    //added by Xi Yang
     int range=(scene.higherBoundX-scene.lowerBoundX)/16;
     for(int j=0;j<range;j++){
         for(int k=0;k<3;k++){
             bool chunkExist=false;
            int x=posx-16;
            int y=scene.lowerBoundY+16*j;
            int z=posz+16*(k-1);
            glm::vec3 Chunkpos= glm::vec3(x,y,z);

            for(Chunk* chunk:chunkManager.renderList){
                if(chunk->getPos()==Chunkpos){
                    //this chunk is not new, need to be update
                   // chunkManager.renderList.push_back(chunk);
                    chunkExist=true;
                    break;
                }
            }
            if(chunkExist==false){               //this chunk is new and need to be creat first
                Chunk* chunk=new Chunk(this,Chunkpos);
               // chunkManager.newLoadList.push_back(chunk);
                chunkManager.updateList.push_back(chunk);
                pushBlock(chunk);
            }
         }
     }

}
void MyGL::addBoundX(){
    Perlin* perlin_generator=new Perlin();
    scene.higherBoundX+=16;
    scene.lowerBoundX+=16;
     int posz=floor(gl_camera.eye.z/16.0f)*16;
     int posx=floor(gl_camera.eye.x/16.0f)*16;
    for(int x=posx+16;x<posx+32;x++){
    for(int z = posz-16; z < posz+32; z++)
           {
//               int height = perlin_generator->OctavePerlin((x)/64.0, 0, z/64.0, 6, 1) * 64;
//               for (int i = 0; i < height; i++)
//               {
//                   scene.containBlock[std::make_tuple(x, i, z)] = true;
//                   scene.scene[std::make_tuple(x, i, z)] = Default;
//               }
        int height = perlin_generator->OctavePerlin((float)x/BOUNDRY, 0, (float)z/BOUNDRY, 6, 1) * BOUNDRY;
        std::tuple<int,int,int> pos;
        pos = std::make_tuple(x, height+15, z);
        scene.containBlock[pos] = true;
        scene.scene[pos] = Grass;

        for (int i = 64; i < height+15; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Default;
        }
        for (int i = 16; i < 64; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Stone;
        }
        // At a depth of 128 blocks, a layer of BEDROCK should be generated;
        for (int i = 0; i < 16; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = BedRock;
        }
           }
    }
    scene.UpdateRiver();
     int range=(scene.higherBoundX-scene.lowerBoundX)/16;
    for(int j=0;j<range;j++){
        for(int k=0;k<3;k++){
            bool chunkExist=false;
           int x=posx+16;
           int y=scene.lowerBoundY+16*j;
           int z=posz+16*(k-1);
           glm::vec3 Chunkpos= glm::vec3(x,y,z);
           //std::cout<<"chunk pos new is "<<x<<" "<<y<<" "<<z<<std::endl;
           for(Chunk* chunk:chunkManager.renderList){
               if(chunk->getPos()==Chunkpos){
                   //this chunk is not new, need to be update
                  // chunkManager.renderList.push_back(chunk);
                   chunkExist=true;
                   break;
               }
           }
           if(chunkExist==false){               //this chunk is new and need to be creat first
               Chunk* chunk=new Chunk(this,Chunkpos);
              // chunkManager.newLoadList.push_back(chunk);
               chunkManager.updateList.push_back(chunk);
               pushBlock(chunk);
           }
        }
    }
}

void MyGL::minusBoundZ(){
    Perlin* perlin_generator=new Perlin();
    scene.higherBoundZ-=16;
    scene.lowerBoundZ-=16;
    int posx=floor(gl_camera.eye.x/16.0f)*16;
    int posz=floor(gl_camera.eye.z/16.0f)*16;
   for(int x=posx-16;x<posx+32;x++){
    for(int z = posz-16; z <posz; z++)
           {
//               int height = perlin_generator->OctavePerlin(x/64.0, 0, z/64.0, 6, 1) * 64;
//               for (int i = 0; i < height; i++)
//               {
//                   scene.containBlock[std::make_tuple(x, i, z)] = true;
//                   scene.scene[std::make_tuple(x, i, z)] = Default;
//               }
        int height = perlin_generator->OctavePerlin((float)x/BOUNDRY, 0, (float)z/BOUNDRY, 6, 1) * BOUNDRY;

        // height field is around ~80 with 16 as variance
        // terrain part + part between sea level and bedrock
        // COORDINATE: bedrock: 16, sea level: 80, above sea level(terrain): 160
        std::tuple<int,int,int> pos;
        pos = std::make_tuple(x, height+15, z);
        scene.containBlock[pos] = true;
        scene.scene[pos] = Grass;

        for (int i = 64; i < height+15; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Default;
        }
        for (int i = 16; i < 64; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Stone;
        }
        // At a depth of 128 blocks, a layer of BEDROCK should be generated;
        for (int i = 0; i < 16; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = BedRock;
        }
           }
   }
   scene.UpdateRiver();
     int range=(scene.higherBoundX-scene.lowerBoundX)/16;
     for(int j=0;j<range;j++){
         for(int k=0;k<3;k++){
             bool chunkExist=false;
            int z=posz-16;
            int y=scene.lowerBoundY+16*j;
            int x=posx+16*(k-1);
            glm::vec3 Chunkpos= glm::vec3(x,y,z);
           // std::cout<<"chunk pos new is "<<x<<" "<<y<<" "<<z<<std::endl;
            for(Chunk* chunk:chunkManager.renderList){
                if(chunk->getPos()==Chunkpos){
                    //this chunk is not new, need to be update
                   // chunkManager.renderList.push_back(chunk);
                    chunkExist=true;
                    break;
                }
            }
            if(chunkExist==false){               //this chunk is new and need to be creat first
                Chunk* chunk=new Chunk(this,Chunkpos);
               // chunkManager.newLoadList.push_back(chunk);
                chunkManager.updateList.push_back(chunk);
                pushBlock(chunk);
            }
         }
     }
}
void MyGL::addBoundZ(){
    Perlin* perlin_generator=new Perlin();
    scene.higherBoundZ+=16;
    scene.lowerBoundZ+=16;
     int posx=floor(gl_camera.eye.x/16.0f)*16;
     int posz=floor(gl_camera.eye.z/16.0f)*16;
    for(int z=posz+16;z<posz+32;z++){
    for(int x = posx-16; x < posx+32; x++)
           {
//               int height = perlin_generator->OctavePerlin((x)/64.0, 0, z/64.0, 6, 1) * 64;
//               for (int i = 0; i < height; i++)
//               {
//                   scene.containBlock[std::make_tuple(x, i, z)] = true;
//                   scene.scene[std::make_tuple(x, i, z)] = Default;
//               }

        int height = perlin_generator->OctavePerlin((float)x/BOUNDRY, 0, (float)z/BOUNDRY, 6, 1) * BOUNDRY;
        std::tuple<int,int,int> pos;
        pos = std::make_tuple(x, height+15, z);
        scene.containBlock[pos] = true;
        scene.scene[pos] = Grass;

        for (int i = 64; i < height+15; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Default;
        }
        for (int i = 16; i < 64; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = Stone;
        }
        // At a depth of 128 blocks, a layer of BEDROCK should be generated;
        for (int i = 0; i < 16; i++)
        {
            pos = std::make_tuple(x, i, z);
            scene.containBlock[pos] = true;
            scene.scene[pos] = BedRock;
        }
           }
    }
    scene.UpdateRiver();
     int range=(scene.higherBoundX-scene.lowerBoundX)/16;
    for(int j=0;j<range;j++){
        for(int k=0;k<3;k++){
            bool chunkExist=false;
           int z=posz+16;
           int y=scene.lowerBoundY+16*j;
           int x=posx+16*(k-1);
           glm::vec3 Chunkpos= glm::vec3(x,y,z);
          // std::cout<<"chunk pos new is "<<x<<" "<<y<<" "<<z<<std::endl;
           for(Chunk* chunk:chunkManager.renderList){
               if(chunk->getPos()==Chunkpos){
                   //this chunk is not new, need to be update
                  // chunkManager.renderList.push_back(chunk);
                   chunkExist=true;
                   break;
               }
           }
           if(chunkExist==false){               //this chunk is new and need to be creat first
               Chunk* chunk=new Chunk(this,Chunkpos);
              // chunkManager.newLoadList.push_back(chunk);
               chunkManager.updateList.push_back(chunk);
               pushBlock(chunk);
           }
        }
    }
}

void MyGL::snowEffect()
{
    int time_count = clock();

    // snows
    if (time_count / 1000000 - oldTime >= 1)
    {
        for (int x = floor(gl_camera.eye[0]/16.0f)*16 - 32; x < floor(gl_camera.eye[0]/16.0f)*16 + 32; x++)
            for(int z = floor(gl_camera.eye[2]/16.0f)*16 - 32; z < floor(gl_camera.eye[2]/16.0f)*16 + 32; z++)
                for (int y = floor(gl_camera.eye[1]/16.0f)*16 - 16; y < floor(gl_camera.eye[1]/16.0f)*16 + 32; y++)
                {
                    std::tuple<int,int,int> pos;
                    pos = std::make_tuple(x, y, z);

                    if (scene.scene[pos] == SnowSpike)
                    {
                        scene.containBlock[pos] = false;
                        scene.scene[pos] = Default;
                    }
                }


        for (int x = floor(gl_camera.eye[0]/16.0f)*16 - 16; x < floor(gl_camera.eye[0]/16.0f)*16 + 16; x++)
            for(int z = floor(gl_camera.eye[2]/16.0f)*16 - 16; z < floor(gl_camera.eye[2]/16.0f)*16 + 16; z++)
            {
                std::tuple<int,int,int> pos;

                // generate snow effect, Y: eyepos ~ eyepos+16
                int random = (int)((double)rand()/RAND_MAX * 50);
                pos = std::make_tuple(x, gl_camera.eye[1] + random, z);
                scene.containBlock[pos] = true;
                scene.scene[pos] = SnowSpike;
            }

        oldTime = time_count / 1000000;
        printf("oldTime = %d\n", oldTime);

        int posx=floor(gl_camera.eye[0]/16.0f)*16;
        int posy=floor(gl_camera.eye[1]/16.0f)*16;
        int posz=floor(gl_camera.eye[2]/16.0f)*16;

        glm::vec3 pos1(posx,posy,posz);
        glm::vec3 pos2(posx-16,posy,posz);
        glm::vec3 pos3(posx+16,posy,posz);
        glm::vec3 pos4(posx,posy,posz-16);
        glm::vec3 pos5(posx,posy,posz+16);
        glm::vec3 pos6(posx,posy+16,posz);

        for(Chunk* chunk:chunkManager.renderList){
            if(chunk->getPos()==pos1 || chunk->getPos()==pos2 || chunk->getPos()==pos3 ||
                    chunk->getPos()==pos4 || chunk->getPos()==pos5 || chunk->getPos()==pos6){
                pushBlock(chunk);
                chunk->create();
            }
        }
    }


    // stop snow
/*
    else if (time_count / 1000000 - oldTime >= 2)
    {
        for (int x = floor(gl_camera.eye[0]/16.0f)*16 - 16; x < floor(gl_camera.eye[0]/16.0f)*16 + 16; x++)
            for(int z = floor(gl_camera.eye[2]/16.0f)*16 - 16; z < floor(gl_camera.eye[2]/16.0f)*16 + 16; z++)
                for (int y = floor(gl_camera.eye[1]/16.0f)*16; y <  floor(gl_camera.eye[1]/16.0f)*16 + 16; y++)
                {
                    std::tuple<int,int,int> pos;
                    pos = std::make_tuple(x, y, z);
                    scene.containBlock[pos] = false;
                }

        printf("clean snow.\n");

        int posx=floor(gl_camera.eye[0]/16.0f)*16;
        int posy=floor(gl_camera.eye[1]/16.0f)*16;
        int posz=floor(gl_camera.eye[2]/16.0f)*16;

        glm::vec3 pos1(posx,posy,posz);
        glm::vec3 pos2(posx-16,posy,posz);
        glm::vec3 pos3(posx+16,posy,posz);
        glm::vec3 pos4(posx,posy,posz-16);
        glm::vec3 pos5(posx,posy,posz+16);
        glm::vec3 pos6(posx,posy+16,posz);

        for(Chunk* chunk:chunkManager.renderList){
            if(chunk->getPos()==pos1 || chunk->getPos()==pos2 || chunk->getPos()==pos3 ||
                    chunk->getPos()==pos4 || chunk->getPos()==pos5 || chunk->getPos()==pos6){
                pushBlock(chunk);
                chunk->create();
            }
        }
    }
    else;
    */


}

void MyGL::updateChunk(){

    glm::vec3 neweye=gl_camera.eye;
    int x=floor(neweye.x/16.0f)*16;
    int y=floor(neweye.y/16.0f)*16;
    int z=floor(neweye.z/16.0f)*16;
  //  std::cout<<" now chunk pos is there"<<x<<" "<<y<<" "<<z<<std::endl;
    glm::vec3 frontx(x-16,y,z);
    glm::vec3 frontz(x,y,z-16);
    glm::vec3 backx(x+16,y,z);
    glm::vec3 backz(x,y,z+16);
    bool hasfx=false;
    bool hasfz=false;
    bool hasbx=false;
    bool hasbz=false;

    for(Chunk* chunk:chunkManager.renderList){
       // std::cout<<"chunk pos is"<<chunk->getPos().x<<chunk->getPos().y<<chunk->getPos().z<<std::endl;
        if(chunk->getPos()==frontx){
            hasfx=true;
        }
        if(chunk->getPos()==backx){
            hasbx=true;
        }
        if(chunk->getPos()==frontz){
            hasfz=true;
        }
        if(chunk->getPos()==backz){
            hasbz=true;
         }
    }

        if(hasfx==false){
            minusBoundX();
            //needToUpdate=true;
            chunkManager.update(gl_camera);
       }
        if(hasbx==false){
            addBoundX();
            chunkManager.update(gl_camera);

        }
        if(hasfz==false){
            minusBoundZ();
            chunkManager.update(gl_camera);

        }
        if(hasbz==false){
            addBoundZ();
            chunkManager.update(gl_camera);
        }

       int range=(scene.higherBoundX-scene.lowerBoundX)/2;
        scene.higherBoundX=x+range;
        scene.lowerBoundX=x-range;
        scene.higherBoundZ=z+range;
        scene.lowerBoundZ=z-range;


}
void MyGL::updateBlock(Block block){
std::tuple<int,int,int> tempPos =block.pos;
int x=std::get<0>(tempPos);
int y=std::get<1>(tempPos);
int z=std::get<2>(tempPos);

int posx=floor(x/16.0f)*16;
int posy=floor(y/16.0f)*16;
int posz=floor(z/16.0f)*16;
x=x-posx;
y=y-posy;
z=z-posz;
glm::vec3 pos(posx,posy,posz);
//std::cout<<"chunk pos"<<posx<<posy<<posz<<std::endl;
for(Chunk* chunk:chunkManager.renderList){
    //std::cout<<"chunk in render list"<<chunk->getPos().x<<" ,"<<chunk->getPos().y<<","<<chunk->getPos().z<<std::endl;
    if(chunk->getPos()==pos){
     //  std::cout<<"can fid a chunk"<<std::endl;
        //chunk->active[x-posx][y-posy][z-posz]=block.isActive();
        chunk->active[x][y][z]=block.isActive();
        chunk->type[x][y][z]=Grass;
        chunk->create();
        update();
    }
}
}
///---------------------added by Xi Yang------------------------
/// ------------------------------------------------------------
void MyGL::timerUpdate()
{
    timecount++;
    prog_tex.setUnifTime(timecount);
    prog_tex.setUnifEye(gl_camera.eye);
    bool grounded=IfGrounded();
    if((grounded==false)&&(isJumping==false)&&(isFlying==false))
    {
        velocity+=gravityacc*timestep;
        gl_camera.eye[1]-=velocity*timestep+0.5*gravityacc*pow(timestep,2);
        gl_camera.ref[1]-=velocity*timestep+0.5*gravityacc*pow(timestep,2);
        gl_camera.RecomputeAttributes();
    }
    else
        velocity=0.0f;
    if(isJumping&&(isFlying==false))
    {
        if(gl_camera.eye[1]>flyingHeight)
        {    isJumping=false;
             flyingHeight=0.0f;}
        else
            PlayerJump(true);
    }
    bool collisioncheck=IfCollision();
    if(collisioncheck&&(isFlying==false)&&(grounded==true))
    {
        PlayerCollide();
    }
    if(isFlying==true)
    {
        if(gl_camera.eye[1]<flyingHeight)
            PlayerJump(true);
        if(gl_camera.eye[1]>flyingHeight)
            PlayerJump(false);
    }

    snowEffect();

    update();

}

void MyGL::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        BreakBlock();
    }else if(e->button()==Qt::RightButton)
    {
        PlaceBlock();
    }
}
void MyGL::BreakBlock()
{
    glm::vec3 forward=gl_camera.look;
    glm::vec3 rayPos=gl_camera.eye;
    float t=gl_camera.near_clip;
    bool Intersect=false;
    while((t<gl_camera.far_clip)&& (Intersect==false))
    {
        rayPos=rayPos+t*forward;
        for(int x = scene.lowerBoundX; x < scene.higherBoundX; x++)
        {   if(Intersect)
                break;
            if((x-0.5-FLT_EPSILON<rayPos[0])&&(rayPos[0]<x+0.5+FLT_EPSILON))
            {
                for (int y = scene.lowerBoundY; y < scene.higherBoundY; y++)
                {
                    if(Intersect)
                                    break;
                    if((y-0.5-FLT_EPSILON<rayPos[1])&&(rayPos[1]<y+0.5+FLT_EPSILON))
                    {
                        for (int z = scene.lowerBoundZ; z < scene.higherBoundZ; z++)
                        {
                            if(Intersect)
                                            break;
                            if((scene.containBlock[std::make_tuple(x,y,z)] == true))
                            {
                                if((z-0.5-FLT_EPSILON<rayPos[2])&&(rayPos[2]<z+0.5+FLT_EPSILON))
                                {
                                    Intersect=true;
                                    //std::cout<<x<<" "<<y<<" "<<z<<std::endl;
                                    if(scene.scene[std::make_tuple(x,y,z)]!=BedRock){
                                    scene.containBlock[std::make_tuple(x,y,z)] = false;
                                    Block block;
                                    block.pos=std::make_tuple(x,y,z);
                                    block.SetActive(false);
                                    updateBlock(block);
                                    update();}

                                }

                            }
                        }
                    }
                }
            }
        }

        t=t+0.01;
    }
}
void MyGL::PlaceBlock()
{
    glm::vec3 forward=gl_camera.look;
    glm::vec3 rayPos=gl_camera.eye;
    float t=gl_camera.near_clip;
    bool Intersect=false;
    while((t<gl_camera.far_clip)&& (Intersect==false))
    {
        rayPos=rayPos+t*forward;
        for(int x = scene.lowerBoundX; x < scene.higherBoundX; x++)
        {   if(Intersect)
                break;
            if((x-0.5<rayPos[0]-FLT_EPSILON)&&(rayPos[0]<x+0.5+FLT_EPSILON))
            {
                for (int y = scene.lowerBoundY; y < scene.higherBoundY; y++)
                {
                    if(Intersect)
                                    break;
                    if((y-0.5-FLT_EPSILON<rayPos[1])&&(rayPos[1]<y+0.5+FLT_EPSILON))
                    {
                        for (int z = scene.lowerBoundZ; z < scene.higherBoundZ; z++)
                        {
                            if(Intersect)
                                            break;
                            if((scene.containBlock[std::make_tuple(x,y,z)] == true))
                            {
                                if((z-0.5-FLT_EPSILON<rayPos[2])&&(rayPos[2]<z+0.5+FLT_EPSILON))
                                {
                                    Intersect=true;
                                    int nx=x;
                                    int ny=y;
                                    int nz=z;
                                    for(int i=0;i<200;i++)
                                    {
                                        float s=0.001*i;
                                        rayPos=rayPos-s*forward;
                                        if(fabs(x+0.5-rayPos[0])<0.01)
                                        {
                                            nx=x+1;
                                            break;
                                        }
                                        if(fabs(x-0.5-rayPos[0])<0.01)
                                        {
                                            nx=x-1;
                                            break;
                                        }
                                        if(fabs(y+0.5-rayPos[1])<0.01)
                                        {
                                            ny=y+1;
                                            break;
                                        }
                                        if(fabs(y-0.5-rayPos[1])<0.01)
                                        {
                                            ny=y-1;
                                            break;
                                        }
                                        if(fabs(z+0.5-rayPos[2])<0.01)
                                        {
                                            nz=z+1;
                                            break;
                                        }
                                        if(fabs(z-0.5-rayPos[2])<0.01)
                                        {
                                            nz=z-1;
                                            break;
                                        }


                                    }

                                    scene.containBlock[std::make_tuple(nx,ny,nz)] = true;
                                    scene.scene[std::make_tuple(nx,ny,nz)]=Grass;
                                    Block block;
                                    block.pos=std::make_tuple(nx,ny,nz);
                                    block.SetActive(true);
                                    updateBlock(block);
                                    update();

                                }

                            }
                        }
                    }
                }
            }
        }

        t=t+0.01;
    }
}

void MyGL::TranslateAlongLook_new(float amt)
{
    glm::vec3 translation = gl_camera.look * amt;
    translation[1]=0;
    gl_camera.eye += translation;
    gl_camera.ref += translation;
    gl_camera.RecomputeAttributes();
    update();

}
void MyGL::TranslateAlongRight_new(float amt)
{
    glm::vec3 translation = gl_camera.right * amt;
    translation[1]=0;
    gl_camera.eye += translation;
    gl_camera.ref += translation;
    gl_camera.RecomputeAttributes();
    update();
}

void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    int ht=height();
    int wd=width();
    float x=e->x();
    float  y=e->y();
    float sx=(2 * x/wd) - 1;
    float sy=1-(2*y/ht);
    glm::vec3 p=gl_camera.ref+sx*gl_camera.H+sy*gl_camera.V;
         glm::vec3 p2eye=p-gl_camera.eye;
         if(glm::dot(p2eye,glm::vec3(p2eye[0],0.0,p2eye[2]))<0)
             return;
         else
         {
             gl_camera.ref=p;
             gl_camera.RecomputeAttributes();
             update();
             QPoint mousePos(width()/2,height()/2);
             mousePos=mapToGlobal(mousePos);
             QCursor::setPos(mousePos);
         }


}

void MyGL::PlayerJump(bool jump)
{

    float v=0.9*gravityacc*timestep;
    //glm::vec3 translation = gl_camera.look;
    glm::vec3 translation(0.0f);
    if(jump)
     translation[1]=1.0;
    else
     translation[1]=-1.0;
    gl_camera.eye+=translation*v;
    gl_camera.ref+=translation*v;
    gl_camera.RecomputeAttributes();

}
void MyGL::PlayerCollide()
{
    float v=0.5*gravityacc*timestep;
//    glm::vec3 translation = oldeye-gl_camera.eye;
//     translation[1]=0;
    glm::vec3 translation(0.0f);

     if(collisiondirecition==0)
     {translation[0]=-1.0*gl_camera.look[0];
         translation[2]=-1.0*gl_camera.look[2];}
     else if(collisiondirecition==1)
     {    translation=gl_camera.look;
         translation[1]=0.0f;}
     else if(collisiondirecition==2)
     {    translation[0]=-1.0*gl_camera.right[0];
         translation[2]=-1.0*gl_camera.right[2];}
     else if(collisiondirecition==3)
     {    translation=gl_camera.right;
         translation[1]=0.0f;}
     else if(collisiondirecition==4)
         //translation = oldeye-gl_camera.eye;
         translation[1]=0.1f;


    //glm::normalize(translation);
    gl_camera.eye+=translation*v;
    gl_camera.ref+=translation*v;
    gl_camera.RecomputeAttributes();

}
bool MyGL::IfGrounded()
{
    glm::vec3 head=gl_camera.eye;
    bool FindGround=false;
    for(int x = scene.lowerBoundX; x < scene.higherBoundX; x++)
     {
        if(FindGround)
            break;
        if((x+FLT_EPSILON>head[0]-1)&&(x-FLT_EPSILON<head[0]+1))
        {
            for (int z = scene.lowerBoundZ; z < scene.higherBoundZ; z++)
            {
                if(FindGround)
                    break;
                if((z+FLT_EPSILON>head[2]-1)&&(z-FLT_EPSILON<head[2]+1))
                {
                    for (int y = scene.lowerBoundY; y < scene.higherBoundY; y++)
                    {
                        if(FindGround)
                            break;
                        if((scene.containBlock[std::make_tuple(x,y,z)] == true))
                        {

                            if((head[1]-2.0-y<0.2)&&(0.0<=head[1]-2.0-y))
                            {

                                FindGround=true;
                                return FindGround;
                            }

                        }

                    }
                }
            }

        }

     }

     return FindGround;
}

bool MyGL::IfCollision()
{
    glm::vec3 head=gl_camera.eye;
    bool collision=false;
    for(int y = scene.lowerBoundY; y < scene.higherBoundY; y++)
    {
        if(collision)
            break;
        if((y<head[1]+1)&&(y>head[1]-2))
        {
            for (int x = scene.lowerBoundX; x < scene.higherBoundX; x++)
            {
                if(collision)
                    break;
                if((head[0]-1<x)&&(head[0]+1>x))
                {
                    for (int z = scene.lowerBoundZ; z < scene.higherBoundZ; z++)
                    {
                        if((scene.containBlock[std::make_tuple(x,y,z)] == true))
                          {
                            if((head[2]-1<z+FLT_EPSILON)&&(head[2]+1>z+FLT_EPSILON))
                            {
                            collision=true;
                            return collision;}

                        }
                    }

                }
            }

        }
    }
    return collision;

}


///---------------------------------------------------------------
