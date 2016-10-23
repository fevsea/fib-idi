#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#define GLM_FORCE_RADIANS

#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

  private:
    void createBuffers ();
    void carregaShaders ();

    void keyPressEvent(QKeyEvent *e);
    void modelTransform(int a);
    
    // attribute locations
    GLuint vertexLoc, colorLoc, transLoc;
    
    // Program
    QOpenGLShaderProgram *program;

    GLuint VAO, VBO, VAOb, VBOb;
};
