//include "shader_interface.h"
#include <sstream>
#include <iostream>
#include <fstream>

template<class T> Parameter<T> :: Parameter(GLint program, char* name, T initVal)
{
    loc = glGetUniformLocation(program, name );
    dirty = true;
    val = initVal;
}

template<class T> void Parameter<T> :: synch()
{
    if ( dirty )
        update(loc, val);
}

// return the address of a Parameter class of type T.
template<class T> Parameter<T>& Parameter<T> :: operator+=( T dv ) {
    val     += dv;
    dirty   = true;
    return *this;
}

template<class T> Parameter<T>& Parameter<T> :: operator-=( T dv ) {
    val     -= dv;
    dirty   = true;
    return *this;
}

template<class T> Parameter<T>& Parameter<T> :: operator=( T v ) {
    val     = v;
    dirty   = true;
    return *this;
}

template<class T> string Parameter<T> :: toString()
{
    stringstream oss;
    oss << T(val);
    return oss.str();
}

//FloatParameter::FloatParameter ( GLhandleARB program,  char *initName, GLfloat initValue/*, GLfloat min, GLfloat max, bool rotate*/)
//{
//    loc = glGetUniformLocation(program, initName );
//    dirty = true;
//    val = initValue;
//}
//FloatParameter :: FloatParameter ()
//{
//    loc = 0;
//    dirty = true;
//    val = 0.0;
//}
//
//string FloatParameter :: toString()
//{
//    stringstream oss;
//    oss << float(val);
//    return oss.str();
//}
//
//string IntParameter :: toString()
//{
//    stringstream oss;
//    oss << int(val);
//    return oss.str();
//}
//
//IntParameter::IntParameter ( GLhandleARB program,  char *initName, GLint initValue/*, GLint min, GLint max, bool rotate*/)
//{
//    loc = glGetUniformLocation( program, initName );
//    dirty = true;
//    val = initValue;
//
//}
//IntParameter :: IntParameter ()
//{
//    loc = 0;
//    dirty = true;
//    val = 0;
//}
//
//void IntParameter :: synch()
//{
//    if (dirty) glUniform1i(loc, val);
//}
//
//void FloatParameter :: synch()
//{
//    if (dirty) {
//        glUniform1f(loc, val);
//        //cout << "fval changed" << val << "\n";
//        //printf ("%f \n", val);
//    }
//}
//
//IntParameter& IntParameter :: operator+=( GLint  dv ) {
//    val     += dv;
//    dirty   = true;
//    return *this;
//}
//IntParameter& IntParameter :: operator-=( GLint  dv ) {
//    val     -= dv;
//    dirty   = true;
//    return *this;
//}
//IntParameter& IntParameter :: operator=( GLint dv ) {
//    val     = dv;
//    dirty   = true;
//    return *this;
//}
//
//FloatParameter& FloatParameter :: operator+=( GLfloat dv ) {
//    val     += dv;
//    dirty   = true;
//    return *this;
//}
//FloatParameter& FloatParameter :: operator-=( GLfloat dv ) {
//    val     -= dv;
//    dirty   = true;
//    return *this;
//}
//FloatParameter& FloatParameter :: operator=( GLfloat dv ) {
//    val     = dv;
//    dirty   = true;
//    return *this;
//}
