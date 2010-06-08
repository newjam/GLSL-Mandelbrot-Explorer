#include "shader_interface.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>
#include <fstream>
#include <exception>

#define PI 3.1415926

using namespace std;

ShaderInterface :: ShaderInterface( string fName , Box bounds, int start_iterations, int width, int height)
{
    font = new FTPixmapFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
    if( !font ){
        cout << "failed to load font.";
    }
    font->FaceSize(45, 45);
    render_text = false;

    char* source = getContents(fName);

    //cout << source;

    GLhandleARB fragmentShader = createShader( source );

    delete source;

    compileShader( fragmentShader );

    program = linkShader( fragmentShader );

    glUseProgram( program );

    // double check to see if the bounds should be abstracted as parameters and not hardcoded.

    time = Parameter<GLfloat>( program, "time", 0.0f );
    addParam( &time );

    wW = Parameter<GLint>(program, "wW", width);
    addParam( &wW );

    wH = Parameter<GLint>(program, "wH", height);
    addParam( &wH );

    iterations = Parameter<GLint>(program, "iterations", start_iterations, 2, 2000, false );
    addParam( &iterations );

    deltax = Parameter<GLfloat>(program, "deltax", 3.0, .00005, 10.0, false);
    addParam( &deltax);

    centerx = Parameter<GLfloat>(program, "centerx", (bounds.minx + bounds.maxx)/2.0 , bounds.minx, bounds.maxx, false);
    addParam( &centerx );

    centery = Parameter<GLfloat>(program, "centery", (bounds.miny + bounds.maxy)/2.0 , bounds.miny, bounds.maxy, false);
    addParam( &centery );

    theta = Parameter<GLfloat>(program, "theta", 1.0, -1.0, 1.0, true);
    addParam( &theta );
}

ShaderInterface :: ~ShaderInterface()
{
    cout << "destroying shader interface" << endl;
    for (unsigned int i=0; i<params.size(); i++)
    {
        delete params[i];
    }
    //delete font;
    glDeleteProgram(program);
}

void ShaderInterface :: printInfo()
{
    cout << "location: (" << centerx << ", " << centery << ")\tzoom: " << deltax << endl;
}

void ShaderInterface :: zoomIn(GLfloat dz, bool relative)
{
    //normalize for time
    if(relative)
    {
        dz = dz * dt;
    }

    deltax -= deltax*dz;
}

void ShaderInterface :: pan(GLfloat dx, GLfloat dy, bool relative)
{
    GLfloat deltay = ( GLfloat(wH)/GLfloat(wW) ) * deltax;

    //normalize for /*time and*/ current zoom (these are absolute, so normalizing for time is N/A) still scale for sanity, though
    if(relative)
    {
        dx =  dx * dt;
        dy =  dy * dt;
    }
    dx = dx * deltax;
    dy = dy * deltay;

    centerx -= dx*cos(PI * theta) - dy*sin(PI *  theta );
    centery -= dx*sin(PI * theta) + dy*cos(PI *  theta );
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

void ShaderInterface :: reset()
{
    for (unsigned int i=0; i<params.size(); i++)
    {
        params[i]->reset();
    }
}

void ShaderInterface :: render()
{
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // we should alread be using this shader, but to be safe, set again.
    glUseProgram(program);
    //since all calculations are being done in the fragment shader, all we draw is a surface.
    glRects(-1, -1, 1, 1);

    if(render_text)
    {
        // default program (white?)
        glUseProgram(0);

        stringstream oss;

        oss.precision(1);
        oss.setf(ios::fixed,ios::floatfield);
        oss << "fps="  << setw (5) << 1.0/dt << ",";
        oss << "iter=" << iterations << ",";
        oss.precision(2);
        oss << "zoom=" << 3.0/deltax << "," ;
        oss.precision(5);
        oss << "pos=("<< centerx << "," << centery << ")";

        font->Render( oss.str().c_str() );

        glUseProgram(program);
    }
}

void ShaderInterface :: toggleText()
{
    render_text = !render_text;
}

/*
// removed so reset would work.
void ShaderInterface :: setResolution( GLint x, GLint y )
{
    wW = x;
    wH = y;
}*/

void ShaderInterface :: changeIterations(float f)
{
    iterations += (f < 0.0) ?  floor( f * dt ) : ceil( f * dt );
}

// I just want to say, GOOD DESIGN FEELS SWELL
