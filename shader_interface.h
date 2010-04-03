#include <GL/glfw.h>
#include <vector>
#include <string>

using namespace std;

struct intBoundary
{
    GLint min;
    GLint max;
};

struct floatBoundary
{
    GLfloat min;
    GLfloat max;
};
struct Boundary
{
    union
    {
        floatBoundary fb;
        intBoundary ib;
    };
    bool rollover;
};

class Parameter
{
protected:
    bool dirty;
    GLint loc;
    intBoundary bound;
public:
    //Parameter();
    //Parameter(char *initName, int initVal);
    virtual void synch () = 0;
    virtual string toString() = 0;
    //virtual operator GLfloat ();
    //virtual operator GLint ();
    /*virtual Parameter operator+=(Parameter &p); ...
    virtual Parameter operator+(Parameter &p); ... */
};

class FloatParameter : public Parameter
{
protected:
    GLfloat val;
public:
    FloatParameter();
    FloatParameter(GLhandleARB program,  char *initName, GLfloat initValue /*, GLfloat min, GLfloat max, bool rotate*/ );
    void synch();
    operator GLfloat () { return val; }
    FloatParameter& operator+=(GLfloat dv);
    FloatParameter& operator-=(GLfloat dv);
    FloatParameter& operator=(GLfloat dv);
    string toString();
    //operator GLint () { return GLfloat(val); }
};

class IntParameter : public Parameter
{
protected:
    GLint val;
public:
    IntParameter();
    IntParameter(GLhandleARB program,  char *initName,  GLint initValue /*,  GLint min, GLint max, bool rotate*/ );
    void synch();
    //operator GLfloat () { return GLfloat(val); }
    operator GLint () { return GLint(val); }
    IntParameter& operator+=(GLint dv);
    IntParameter& operator-=(GLint dv);
    IntParameter& operator=(GLint dv);
    string toString();
    //is it legal to have these as references, but the ones in the abstract class to not be?
    /*IntParameter& operator+=(IntParameter &p);
    IntParameter& operator-=(IntParameter &p);
    IntParameter& operator*=(IntParameter &p);
    IntParameter& operator/=(IntParameter &p);*/
};


/*union TYPE
{
    int     i;
    float   f;
} value ;*/




class ShaderInterface
{
private:
    FloatParameter time;
    FloatParameter minx;
    FloatParameter miny;
    FloatParameter deltax;
    FloatParameter deltay;
    FloatParameter theta;
    IntParameter wW;
    IntParameter wH;
    IntParameter iterations;

    GLhandleARB program;

    float dt;
    std::vector<Parameter*> params;
public:
    ShaderInterface(string fName);
    // and important thing to note is that these aren't absolute,
    // for example you might expect rotate .25 to rotate the view 1 quarter,
    // it actually rotates the view .25* the amount of time since the last frame was rendered.
    void zoomIn(float dz);
    void pan(float dx, float dy);
    void rotate(float dr);
    void setTime(float time);
    void addParam(Parameter* p);
    void synchParams();
    void render();
    void setResolution(GLint x, GLint y);
    void changeIterations(float factor);
};
