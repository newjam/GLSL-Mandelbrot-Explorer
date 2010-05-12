#include "parameter.h"
#include <FTGL/ftgl.h>
#include <GL/glfw.h>
#include <vector>

using namespace std;

class ShaderInterface
{
private:
    Parameter<GLfloat> time, centerx, centery, deltax, theta;

    Parameter<GLint> wW, wH, iterations;

    GLhandleARB program;

    bool render_text;
    FTFont *font;

    float dt;
    std::vector<ParameterInterface*> params;
public:
    ShaderInterface(string fName, Box space, int width, int height);
    //~ShaderInterface();
    // and important thing to note is that these aren't absolute,
    // for example you might expect rotate .25 to rotate the view 1 quarter,
    // it actually rotates the view .25* the amount of time since the last frame was rendered.
    void zoomIn(float dz);
    void pan(float dx, float dy);
    void rotate(float dr);
    void setTime(float time);
    void addParam(ParameterInterface* p);
    void synchParams();
    void render();
    void toggleText();
    //void setResolution(GLint x, GLint y);
    void changeIterations(float factor);
    void reset();
};
