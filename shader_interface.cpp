#include "shader_interface.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <fstream>

#define PI 3.1415926

using namespace std;

ShaderInterface :: ShaderInterface( string fName )
{
    char* source = getContents(fName);

    cout << source;

    GLhandleARB fragmentShader = createShader( source );

    delete source;

    compileShader( fragmentShader );

    program = linkShader( fragmentShader );

    glUseProgram( program );

    time = Parameter<GLfloat>( program, "time", 0.0f );
    addParam( &time );

    // set the dimension, must be after program is used.
    //glUniform1i(wWLoc, window_width );
    //glUniform1i(wHLoc, window_height );

    wW = Parameter<GLint>(program, "wW", 100);
    addParam( &wW );

    wH = Parameter<GLint>(program, "wH", 100);
    addParam( &wH );

    iterations = Parameter<GLint>(program, "iterations", 200, 2, 2000, false );
    addParam( &iterations );

    deltax = Parameter<GLfloat>(program, "deltax", 3.0, .001, 3.0, false);
    addParam( &deltax);
/*
    deltay = Parameter<GLfloat>(program, "deltay", 2.0);
    addParam( &deltay );
*/
    minx = Parameter<GLfloat>(program, "minx", -2.0 , -3.0, 2.0, false);
    addParam( &minx );

    miny = Parameter<GLfloat>(program, "miny", -1.0, -2.0, 2.0, false);
    addParam( &miny );

    theta = Parameter<GLfloat>(program, "theta", 1.0, -1.0, 1.0, true);
    addParam( &theta );

    //width = Parameter<GLfloat>(program, "width", 5.0, 0.0, 5.0, true);
    //addParam( &theta );
}

void ShaderInterface :: zoomIn(GLfloat dz)
{
    //normalize for time
    dz = dz * dt;

    //cout << "dz: " << dz << endl;
     ;
    GLfloat deltay = ( GLfloat(wH)/GLfloat(wW) ) * deltax;

    //increase minx and miny by some small percentage of deltax and deltay
    minx += deltax*dz;
    miny += deltay*dz;

    //cout << "(" << minx.toString() << ", " << miny.toString() << ")" << endl;

    //decrease deltax and deltay by 2 times that small percentage.
    //deltax -= 2.0*deltax*dz;
    //deltay -= 2.0*deltay*dz;

    deltax -= 2.0*deltax*dz;
}

void ShaderInterface :: pan(GLfloat dx, GLfloat dy)
{
    GLfloat deltay = ( GLfloat(wH)/GLfloat(wW) ) * deltax;

    //normalize for time and current zoom
    dx = dx * dt * deltax;
    dy = dy * dt * deltay;

    minx -= dx*cos(PI * theta) - dy*sin(PI *  theta );
    miny -= dx*sin(PI * theta) + dy*cos(PI *  theta );
}

void ShaderInterface :: rotate(GLfloat dr)
{
    theta += dr * dt;
    if ( theta > 1.0 )
    {
        theta -= 2.0;
    }else if (theta < -1.0)
    {
        theta += 2.0;
    }
}

//ShaderInterface :: void rotate(float dr);
void  ShaderInterface :: setTime(float nTime)
{
    dt = nTime - time;
    time = nTime;
}

void ShaderInterface :: addParam(ParameterInterface* p)
{
    params.push_back(p);
}

void ShaderInterface :: synchParams()
{
    for (unsigned int i=0; i<params.size(); i++)
    {
        params[i]->synch();
    }
}

void ShaderInterface :: render()
{
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //since all calculations are being done in the fragment shader, all we draw is a surface.
    glUseProgram(program);
    glRects(-1, -1, 1, 1);
}

void ShaderInterface :: setResolution( GLint x, GLint y )
{
    wW = x;
    wH = y;
}

void ShaderInterface :: changeIterations(float f)
{

    iterations += (f < 0.0) ?  floor( f * dt ) : ceil( f * dt );
}

/*void test(){
    ShaderInterface si = ShaderInterface("mandelbrot (copy).frag");
    si.synchParams();

    IntParameter bbb = IntParameter(0,0,2);
    bbb += 4+IntParameter(0,0,3);
    cout<< GLint(bbb);
}*/

/*int main(void)
{
    test();
}*/
