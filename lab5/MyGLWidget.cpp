#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
        setFocusPolicy(Qt::ClickFocus); // per rebre events de teclat
        scale = 1.0f;
}

MyGLWidget::~MyGLWidget ()
{
        if (program != NULL)
                delete program;
}

void MyGLWidget::initializeGL ()
{
        // Cal inicialitzar l'ús de les funcions d'OpenGL
        initializeOpenGLFunctions();
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
        carregaShaders();
        createBuffers();
        createBase();

        ini_capsa(glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0, 1.0, 1.0));
        ini_camera();
}

void MyGLWidget::paintGL ()
{
        // Esborrem el frame-buffer
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Carreguem la transformació de model
        modelTransform ();

        // Activem el VAO per a pintar la caseta
        glBindVertexArray (VAO_Homer);

        // pintem
        glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);

        glBindVertexArray (VAO_Base);
        glm::mat4 transform(1.0f);
        transform = glm::scale(transform, glm::vec3(scale));
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray (0);
}

void MyGLWidget::resizeGL (int w, int h)
{
        glViewport(0, 0, w, h);
        ra = (float)w/h;
        if(ra < 1) {
                FOV = 2 * atan(tan((float)M_PI/4.0f)/ra);
        }
        projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
        switch (event->key()) {
                makeCurrent();
        case Qt::Key_S: { // escalar a més gran
                scale += 0.05;
                break;
        }
        case Qt::Key_D: { // escalar a més petit
                scale -= 0.05;
                break;
        }
        case Qt::Key_R: { // escalar a més petit
                if(event->modifiers()&(Qt::ShiftModifier)) {
                        rot -= 1;
                } else{
                        rot += 1;
                }
                break;
        }
        default: event->ignore(); break;
        }
        update();
}
void MyGLWidget::createBase() {
        glm::vec3 Vertices[4]; // Tres vèrtexs amb X, Y i Z
        Vertices[0] = glm::vec3(-1.0, -1.0,-1.0);
        Vertices[1] = glm::vec3(-1.0, -1.0, 1.0);
        Vertices[2] = glm::vec3( 1.0, -1.0, 1.0);
        Vertices[3] = glm::vec3( 1.0, -1.0,-1.0);

        glm::vec3 Colors[4]; // Tres vèrtexs amb X, Y i Z
        Colors[0] = glm::vec3(1.0, 0.0, 0.0);
        Colors[1] = glm::vec3( 0.0,  1.0, 0.0);
        Colors[2] = glm::vec3( 0.0,  0.0, 1.0);
        Colors[3] = glm::vec3( 1.0,  0.0, 1.0);

        // Creació del Vertex Array Object (VAO) que usarem per pintar
        glGenVertexArrays(1, &VAO_Base);
        glBindVertexArray(VAO_Base);

        // Creació del buffer amb les dades dels vèrtexs
        glGenBuffers(1, &VBO_BasePos);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_BasePos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
        // Activem l'atribut que farem servir per vèrtex
        glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexLoc);

        // Creació del buffer amb les dades dels vèrtexs
        glGenBuffers(1, &VBO_BaseCol);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_BaseCol);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
        // Activem l'atribut que farem servir per vèrtex
        glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(colorLoc);
        // Desactivem el VAO
        glBindVertexArray(0);
}

void MyGLWidget::createBuffers ()
{
        m.load("../models/HomerProves.obj");


        // Creació del Vertex Array Object per pintar
        glGenVertexArrays(1, &VAO_Homer);
        glBindVertexArray(VAO_Homer);

        glGenBuffers(1, &VBO_HomerPos);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerPos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size()*3*3,
                     m.VBO_vertices(), GL_STATIC_DRAW);

        // Activem l'atribut vertexLoc
        glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vertexLoc);

        glGenBuffers(1, &VBO_HomerCol);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerCol);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size()*3*3,
                     m.VBO_matdiff(), GL_STATIC_DRAW);

        // Activem l'atribut colorLoc
        glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(colorLoc);

        glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
        // Creem els shaders per al fragment shader i el vertex shader
        QOpenGLShader fs (QOpenGLShader::Fragment, this);
        QOpenGLShader vs (QOpenGLShader::Vertex, this);
        // Carreguem el codi dels fitxers i els compilem
        fs.compileSourceFile("shaders/fragshad.frag");
        vs.compileSourceFile("shaders/vertshad.vert");
        // Creem el program
        program = new QOpenGLShaderProgram(this);
        // Li afegim els shaders corresponents
        program->addShader(&fs);
        program->addShader(&vs);
        // Linkem el program
        program->link();
        // Indiquem que aquesprojLoct és el program que volem usar
        program->bind();

        // Obtenim identificador per a l'atribut “vertex” del vertex shader
        vertexLoc = glGetAttribLocation (program->programId(), "vertex");
        // Obtenim identificador per a l'atribut “color” del vertex shader
        colorLoc = glGetAttribLocation (program->programId(), "color");
        // Uniform locations
        transLoc = glGetUniformLocation(program->programId(), "TG");

        projLoc = glGetUniformLocation(program->programId(), "proj");
        viewLoc = glGetUniformLocation(program->programId(), "view");
}

void MyGLWidget::modelTransform ()
{
        // Matriu de transformació de modela
        glm::mat4 transform(1.0f);
        transform = glm::scale(transform, glm::vec3(scale));
        transform = glm::rotate(transform, rot * (float)M_PI/8, rotateA);
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform(){
        //glm:perspective(FOV en radians, ra window, znear, zfar);
        glm::mat4 Proj = glm::perspective((float) FOV, ra, znear, zfar);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform(){
        //glm:lokAt(OBS, VRP, up)
        glm::mat4 View = glm::lookAt(OBS, centre, up);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::ini_capsa(glm::vec3 minP, glm::vec3 maxP){
        centre[0] = (maxP[0]+minP[0])/2;
        centre[1] = (maxP[1]+minP[1])/2;
        centre[2] = (maxP[2]+minP[2])/2;

        R = (float) std::max(maxP[0], std::max(maxP[1], maxP[2])) - std::min(minP[0], std::min(minP[1], minP[2]));
        d = 1*R;
}

void MyGLWidget::ini_camera(){
        OBS = glm::vec3(0,0,2);
        up  = glm::vec3(0,0.5,0);

        FOV = (float)M_PI/2.0f;
        ra = 1.0f;
        znear = 1.0f;
        zfar = 4.0f;

        rotateA = glm::vec3(0.0f,1.0f,0.0f);
        rot = 0;

        projectTransform();
        viewTransform();
}