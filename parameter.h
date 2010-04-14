#include <sstream>
#include <iostream>
#include <fstream>
#include <GL/glfw.h>
#include <string>
#include <limits>

using namespace std;

// G++ forces you to put implementation of code in the same file as the header. Thanks a lot guys :(

template<class T> class Bounds
{
private:
    T minV;
    T maxV;
    bool rollover;
public:
    Bounds()
    {
        minV = numeric_limits<T>::min() ;
        maxV = numeric_limits<T>::max() ;
        rollover = false;
        cout << minV << " to " << maxV << endl;
    }
    Bounds(T mi, T mx, bool r)
    {
        minV = mi;
        maxV = mx;
        rollover = r;
    }
    T bind(T val)
    {
        if ( val > maxV )
        {
            cout << "overshot" << val << " > " << maxV << endl;
            if (rollover)
            {
                return minV + (val - maxV);
            }
            else
            {
                return maxV;
            }
        } else if (val < minV)
        {
            cout << "undershot: " << val << " < " << minV << endl;
            if (rollover)
            {
                return maxV + (val - minV);
            }
            else
            {
                return minV;
            }
        } else
        {
            return val;
        }
    }
};

template<class T> class Uniformer
{
    static void update(GLint location, T& value);
};

template<> class Uniformer<GLint>
{
public:
    static void update(GLint location, GLint value)
    {
        glUniform1i(location, value);
    }
};
template<> class Uniformer<GLfloat>
{
public:
    static void update(GLint location, GLfloat value)
    {
        glUniform1f(location, value);
    }
};

class ParameterInterface
{
public:
    virtual void synch() = 0;
};

template<class T> class Parameter
    : public Uniformer<T>,
      public ParameterInterface {
private:
    GLint loc;
    T val;
    Bounds<T>* bound;
protected:
    bool dirty;
public:
    Parameter()
    {
        loc = 0;
        val = 0;
        dirty = true;
    }
    Parameter(int prog, char* name, T initVal)
    {
        loc = glGetUniformLocation(prog, name );
        dirty = true;
        val = initVal;

        bound = new Bounds<T>();
    }

    //, T min, T max, bool rollover
        //Parameter( prog, name, initVal, min , max , false );
    Parameter(int prog, char* name, T initVal, T min, T max, bool rollover)
    {
        loc = glGetUniformLocation(prog, name );
        dirty = true;
        val = initVal;

        //replace bounds with our user specified params.
        bound = new Bounds<T>(min, max, rollover);

        //T min = numeric_limits<T>::min() ;
        //T max = numeric_limits<T>::max() ;
        //cout << min << " to "<< max << endl ;
        //Parameter( prog, name, initVal, min , max , false );

        //cout << min << endl ;
        /*loc = glGetUniformLocation(prog, name );
        dirty = true;
        val = initVal;*/
    }
    void synch()
    {
        if ( dirty )
            update(loc, val);
    }
    operator T ()
    {
        return val;
    }
    string toString()
    {
        stringstream oss;
        oss << T(val);
        return oss.str();
    }
    Parameter<T>& operator+=(T dv)
    {

        val     += dv;
        val     = bound->bind(val);
        dirty   = true;
        return *this;
    }
    Parameter<T>& operator-=(T dv)
    {
        val     -= dv;
        val     = bound->bind(val);
        dirty   = true;
        return *this;
    }
    Parameter<T>& operator=(T v)
    {
        val     = bound->bind(v);
        dirty   = true;
        return *this;
    }
};




///*class Parameter
//{
//protected:
//    bool dirty;
//    GLint loc;
//public:
//    void synch();
//    string toString();
//    FloatParameter& operator+=(GLfloat dv);
//    FloatParameter& operator-=(GLfloat dv);
//    FloatParameter& operator=(GLfloat dv);
//
//    //Parameter();
//    //Parameter(char *initName, int initVal);
//    //virtual operator GLfloat ();
//    //virtual operator GLint ();
//    /*virtual Parameter operator+=(Parameter &p); ...
//    virtual Parameter operator+(Parameter &p); ... */
//};
//
//class FloatParameter : public Parameter
//{
//protected:
//    GLfloat val;
//public:
//    FloatParameter();
//    FloatParameter(GLhandleARB program,  char *initName, GLfloat initValue /*, GLfloat min, GLfloat max, bool rotate*/ );
//    void synch();
//    operator GLfloat () { return val; }
//    FloatParameter& operator+=(GLfloat dv);
//    FloatParameter& operator-=(GLfloat dv);
//    FloatParameter& operator=(GLfloat dv);
//    string toString();
//    //operator GLint () { return GLfloat(val); }
//};
//
//class IntParameter : public Parameter
//{
//protected:
//    GLint val;
//public:
//    IntParameter();
//    IntParameter(GLhandleARB program,  char *initName,  GLint initValue /*,  GLint min, GLint max, bool rotate*/ );
//    void synch();
//    //operator GLfloat () { return GLfloat(val); }
//    operator GLint () { return GLint(val); }
//    IntParameter& operator+=(GLint dv);
//    IntParameter& operator-=(GLint dv);
//    IntParameter& operator=(GLint dv);
//    string toString();
//    //is it legal to have these as references, but the ones in the abstract class to not be?
//    /*IntParameter& operator+=(IntParameter &p);
//    IntParameter& operator-=(IntParameter &p);
//    IntParameter& operator*=(IntParameter &p);
//    IntParameter& operator/=(IntParameter &p);*/
//};/*
//
//
///*union TYPE
//{
//    int     i;
//    float   f;
//} value ;
