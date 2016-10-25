#include "MyGLWidget.h"
#include <iostream>

GLfloat scl = 1.0;
GLfloat tx = 0.0;
GLfloat ty = 0.0;
GLfloat tz = 0.0;
float step = 0.1;

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
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

  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Activem l'Array a pintar
  modelTransform(0);
  glBindVertexArray(VAO);

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

  modelTransform(1);
  glBindVertexArray(VAOb);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
}

void MyGLWidget::createBuffers ()
{
  glm::vec3 VerticesA[3];  // Tres vèrtexs amb X, Y i Z
  VerticesA[0] = glm::vec3(-1.0, -1.0, 0.0);
  VerticesA[1] = glm::vec3( 1.0,  -1.0, 0.0);
  VerticesA[2] = glm::vec3( 0.0,  1.0, 0.0);

  glm::vec3 VerticesB[3];  // Tres vèrtexs amb X, Y i Z
  VerticesB[0] = glm::vec3(-1.0, -1.0, 0.0);
  VerticesB[1] = glm::vec3( 1.0,  -1.0, 0.0);
  VerticesB[2] = glm::vec3( .0,  1.0, 0.0);



  glm::vec3 Colors[3];  // Tres vèrtexs amb X, Y i Z
  Colors[0] = glm::vec3(1.0, 0.0, 0.0);
  Colors[1] = glm::vec3( 0.0,  1.0, 0.0);
  Colors[2] = glm::vec3( 0.0,  0.0, 1.0);




  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesA), VerticesA, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAOb);
  glBindVertexArray(VAOb);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBOb);
  glBindBuffer(GL_ARRAY_BUFFER, VBOb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesB), VerticesB, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBOb);
  glBindBuffer(GL_ARRAY_BUFFER, VBOb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  // Desactivem el VAO
  glBindVertexArray(0);
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
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");
  transLoc = glGetUniformLocation (program->programId(), "TG");
}

// mousePressEvent(QMouseEvent *e);
// mouseRelaseEvent(QMouseEvent *e);
// mouseMoveEvent(QMouseEvent *e);
void MyGLWidget::keyPressEvent(QKeyEvent *e){
  //if(e->modifiers()&(Qt::ShiftModifier|Qt:AltModifier|Qt:ControlModifier)))
  makeCurrent();
  switch(e->key()){
    case Qt::Key_S:
      scl -= step;
    break;

    case Qt::Key_W:
      scl += step;

    break;

    case Qt::Key_Left:
      tx -= step;
    break;

    case Qt::Key_Right:
      tx += step;
    break;

    case Qt::Key_Up:
      ty += step;
    break;

    case Qt::Key_Down:
      ty -= step;
    break;

    case Qt::Key_D:
      tz -= 1;
    break;
    case Qt::Key_A:
      tz += 1;
    break;
    default: e->ignore();
  }
  update();
}

void MyGLWidget::modelTransform(int a){
  if (a == 1){
    tx = -tx;
    ty = -ty;
    tz = -tz;
  }

  glm::mat4 TG(1.0);
  TG = glm::translate(TG, glm::vec3(tx, ty, 0.0));
  TG = glm::rotate(TG, (float) (tz*M_PI/8), glm::vec3(0.0, 0.0, 1));
  TG = glm::scale(TG, glm::vec3(scl, scl, scl));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);


  if (a == 1){
    tx = -tx;
    ty = -ty;
    tz = -tz;
  }
}

// 695*695
// 710*710      glUniform1f(varLoc, scl);
