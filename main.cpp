#include "shader_interface.h"
#include "utils.h"
#include <iostream>

#include <string>
#include <fstream>

#include <cmath>

// templated function stolen from interwebs to parse
template<class T>
    T fromString(const std::string& s)
{
     std::istringstream stream (s);
     T t;
     stream >> t;
     return t;
}

FrameSaver *fs;
ShaderInterface *si;

void GLFWCALL keyHandler( int key, int action )
{
    if(action == GLFW_PRESS)
    {
        switch (key)
        {
            case 'P':
                fs->saveFrameBuffer();
                break;
            case 'R':
                si->reset();
                break;
            case 'O':
                si->toggleText();
                break;
        }
    }
}

int main(int argc, const char* argv[])
{
    if (argc <= 1)
    {
        cout << "Must specificy shader to use:" << endl << "\t" << argv[0] << " <shader.frag>" << endl;
        return -1;
    }

    Box bounds;
    if ( argc == 6 )
    {
        cout << "setting custom bounds" << endl;
        bounds.minx = fromString<float>( string( argv[2] ) );
        bounds.miny = fromString<float>( string( argv[3] ) );
        bounds.maxx = fromString<float>( string( argv[4] ) );
        bounds.maxy = fromString<float>( string( argv[5] ) );
    }else
    {
        bounds.minx =  -2.0;
        bounds.miny =  -1.5;
        bounds.maxx =  1.0;
        bounds.maxy =  1.5;
    }

    glfwInit();

    GLFWvidmode mode;
    glfwGetDesktopMode( &mode );

    int window_width = mode.Width;
    int window_height = mode.Height;

    fs = new FrameSaver(window_width, window_height);

    printf("%dx%d\n", window_width, window_height);
    glfwOpenWindow(mode.Width, mode.Height, mode.RedBits,  mode.GreenBits, mode.BlueBits, 0, 0, 0, GLFW_FULLSCREEN);

    // setup opengl perspective stuff.
    glMatrixMode(GL_PROJECTION);
    float aspect_ratio = ((float)window_height) / window_width;
    glFrustum(.5, -.5, -.5 * aspect_ratio, .5 * aspect_ratio, 1, 50);
    glDisable( GL_DEPTH_TEST );
    glTranslatef(0.0, 0.0, -1.00);
    glClearColor(0.0, 1.0, 1.0, 0.0);

    bool joystickPresent = glfwGetJoystickParam( GLFW_JOYSTICK_1, GLFW_PRESENT );
    if(joystickPresent)
    {
        printf("Joystick present. Can be used to navigate in addition to mouse.\n");
    }else
    {
        printf("no joystick present.\n");
    }

    Cursor cursor = Cursor();

    si = new ShaderInterface(argv[1], bounds, window_width, window_height);
    //si->setResolution( window_width, window_height );

    glfwSetKeyCallback( keyHandler );

    // keep on rendering the frame until escape is pressed.
    //unsigned int i = 2000;
    while( glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS )
    {
        // update the time, to get smooth translations
        si->setTime( glfwGetTime() );
        cursor.update();

        if( cursor.moved() )
            si->pan(cursor.getDx() / 40.0 , cursor.getDy() / 40.0 );
        if( cursor.scrolled() )
            si->zoomIn( cursor.getDz() * 10.0 );


        if(joystickPresent)
        {
            float epsilon = .005 ;

            float pos[3];
            glfwGetJoystickPos(GLFW_JOYSTICK_1, pos, 3);
            float xJoyPos = pos[0];
            float yJoyPos = pos[1];
            float zJoyPos = -pos[2];

            if( abs(xJoyPos) + abs(yJoyPos) > epsilon*2.0 )
                si->pan(xJoyPos, yJoyPos);
            if( abs(zJoyPos) > epsilon )
                si->zoomIn(zJoyPos);

            unsigned char buttons[3];
            glfwGetJoystickButtons(GLFW_JOYSTICK_1, buttons, 3);
            if(buttons[0]==GLFW_PRESS)
                fs->saveFrameBuffer();
            if(buttons[1]==GLFW_PRESS)
                si->rotate( 0.25 );
            else if(buttons[2]==GLFW_PRESS)
                si->rotate( -0.25 );
        }

        // zooming using the up/down arrow keys
        if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS)
            si->zoomIn(1.0);
        else if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
            si->zoomIn(-1.0);

        // changing iterations using left/right arrows
        if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
            si->changeIterations(-1.0);
        else if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
            si->changeIterations(1.0);

        // panning with WASD on the keyboard
        if( glfwGetKey('W') == GLFW_PRESS )
            si->pan( 0.0, 1.0 );
        if( glfwGetKey('S') == GLFW_PRESS )
            si->pan( 0.0, -1.0 );
        if( glfwGetKey('A') == GLFW_PRESS )
            si->pan( -1.0, 0.0 );
        if( glfwGetKey('D') == GLFW_PRESS )
            si->pan( 1.0, 0.0 );

        // end of input, synch and render and swap.
        si->synchParams();
        si->render();
        glfwSwapBuffers();
    }
}
