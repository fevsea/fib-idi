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

        Ex = Ey = Ez = 0;

        get_box(m);
        ini_capsa();
        ini_camera();
}

void MyGLWidget::paintGL ()
{
        // Esborrem el frame-buffer
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Carreguem la transformació de model
        modelTransform(1);

        // Activem el VAO per a pintar la caseta
        glBindVertexArray (VAO_Homer);

        // pintem
        glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);


        modelTransform(2);

        // Activem el VAO per a pintar la caseta
        glBindVertexArray (VAO_Homer);

        // pintem
        glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);


        glBindVertexArray (VAO_Base);
        glm::mat4 transform(1.0f);
        transform = glm::scale(transform, glm::vec3(scale));
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray (0);
}

void MyGLWidget::resizeGL (int w, int h)
{
        glViewport(0, 0, w, h);
        ra = float (w) / float(h);
        if(ra < 1) {
                float av = asin(R/d);
                FOV = 2 * atan(tan(av)/ra);
        }
        projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
        makeCurrent();
        switch (event->key()) {

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
        case Qt::Key_X: { // escalar a més petit
                zoom += 0.05;
                projectTransform();
                break;
        }
        case Qt::Key_Z: { // escalar a més petit
                 zoom -= 0.05;
                 projectTransform();
                 break;
        }
        default: event->ignore(); break;
        }
        update();

}

void MyGLWidget::mousePressEvent( QMouseEvent *event){
    makeCurrent();
    if(event->buttons() == Qt::LeftButton){
        Ox = event->pos().x();
        Oy = event->pos().y();
    }
}

void MyGLWidget::mouseMoveEvent( QMouseEvent *event){
        makeCurrent();
        float x = event->pos().x();
        float y = event->pos().y();
        Ez -= (Ox - x) * 0.0005;
        Ey += (Oy - y) * 0.0005;
        viewTransform();
        update();
}

void MyGLWidget::createBase() {
        glm::vec3 Vertices[4]; // Tres vèrtexs amb X, Y i Z
        Vertices[0] = glm::vec3(-2.0,  0.0, -2.0);
        Vertices[1] = glm::vec3(-2.0,  0.0,  2.0);
        Vertices[2] = glm::vec3( 2.0,  0.0, -2.0);
        Vertices[3] = glm::vec3( 2.0,  0.0,  2.0);

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
        m.load("../models/" + model + ".obj");


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

void MyGLWidget::modelTransform (int i)
{
        // Matriu de transformació de modela
        glm::mat4 transform(1.0f);
        transform = glm::scale(transform, glm::vec3(scale));
        if(i == 1) transform = glm::translate(transform, glm::vec3(1, (centre[1]-minP[1])/R, 1));
        if(i == 2) transform = glm::translate(transform, glm::vec3(-1, (centre[1]-minP[1])/R, -1));
        transform = glm::scale(transform, glm::vec3(scale/R));
        if(i == 1) transform = glm::rotate(transform, rot * (float)M_PI/8, rotateA);
        if(i == 2) transform = glm::rotate(transform, rot * (float)M_PI/8 + (float)M_PI, rotateA);
        transform = glm::translate(transform, glm::vec3(-1*centre[0], -1*centre[1], -1*centre[2]));
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform(){
        //glm:perspective(FOV en radians, ra window, znear, zfar);
        glm::mat4 Proj = glm::perspective((float) FOV*zoom, ra, znear, zfar);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform(){
        glm::mat4 VM(1.0f);
        VM = glm::translate(VM, glm::vec3(0,0,-d));
        VM = glm::rotate(VM, -1 *Ex , glm::vec3(0, 0, 1));
        VM = glm::rotate(VM, Ey , glm::vec3(1, 0, 0));
        VM = glm::rotate(VM, -1 * Ez , glm::vec3(0, 1, 0));
        VM = glm::translate(VM, glm::vec3(-1*VRP[0], -1*VRP[1], -1*VRP[2]));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &VM[0][0]);
}

void MyGLWidget::ini_capsa(){
        centre[0] = (maxP[0]+minP[0])/2;
        centre[1] = (maxP[1]+minP[1])/2;
        centre[2] = (maxP[2]+minP[2])/2;

        R = (float) glm::distance(maxP, minP)/2;
        d = 2*R;
}

void MyGLWidget::get_box(Model &mo){
  float xmin = mo.vertices()[0];
  float ymin = mo.vertices()[1];
  float zmin = mo.vertices()[2];
  float xmax = xmin;
  float ymax = ymin;
  float zmax = zmin;

  for(unsigned int i = 3; i < mo.vertices().size(); i+=3){
    float x = mo.vertices()[i];
    float y = mo.vertices()[i+1];
    float z = mo.vertices()[i+2];
    if(x > xmax) xmax = x;
    if(x < xmin) xmin = x;
    if(y > ymax) ymax = y;
    if(y < ymin) ymin = y;
    if(z > zmax) zmax = z;
    if(z < zmin) zmin = z;
  }
  maxP = glm::vec3(xmax, ymax, zmax);
  minP = glm::vec3(xmin, ymin, zmin);

}

void MyGLWidget::ini_camera(){
        OBS = glm::vec3(0,0,d);
        up  = glm::vec3(0,1,0);
        zoom = 1.0f;

        Ey += (float) M_PI/4;

        FOV = (float) 2 * asin(R/d);
        ra = 1.0f;
        znear = d-R;
        zfar = d+R;
        VRP = glm::vec3(0.0f,0.0f,0.0f);
        rotateA = glm::vec3(0.0f,1.0f,0.0f);
        rot = 0;

        projectTransform();
        viewTransform();
}

void MyGLWidget::loadModel(const QString & s){
    model = s.toStdString();
    std::cout << "../models/" + model + ".obj" << std::endl;
    createBuffers();

    get_box(m);
    ini_capsa();
    ini_camera();
}

void MyGLWidget::getInt(const int i){
    makeCurrent();
    std::cout << i << std::endl;
    rot = (int) (i-50)*0.1f;
    update();

}
