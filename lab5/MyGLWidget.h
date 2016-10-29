#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../Model/model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);


  private:
    void createBuffers ();
    void createBase ();
    void carregaShaders ();
    void modelTransform ();
    void projectTransform();
    void viewTransform();
    void ini_camera();
    void ini_capsa(glm::vec3 minP, glm::vec3 maxP);

    // attribute locations
    GLuint vertexLoc, colorLoc, projLoc, viewLoc;
    // uniform locations
    GLuint transLoc;
    // VAO i VBO names
    GLuint VAO_Homer, VBO_HomerPos, VBO_HomerCol;
    GLuint VAO_Base, VBO_BasePos, VBO_BaseCol;
    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale, rot;
    glm::vec3 pos, rotateA;
    Model m;

    //Camera
    glm::vec3 OBS, VRP, up;
    float FOV, ra, znear, zfar;

    glm::vec3 minP, maxP, centre;
    float R, d;
};