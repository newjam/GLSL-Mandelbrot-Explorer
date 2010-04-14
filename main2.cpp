#include "shader_interface.h"
#include "utils.h"
#include <iostream>

#include <string>
#include <fstream>

#include <cmath>

int main(void)
{
    glfwInit();

    GLFWvidmode mode;
    glfwGetDesktopMode( &mode );

    int window_width = mode.Width;
    int window_height = mode.Height;

    FrameSaver fs = FrameSaver(window_width, window_height);

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

    ShaderInterface si = ShaderInterface("mandelbrot (copy).frag");
    si.setResolution( window_width, window_height );
    // keep on rendering the frame until escape is pressed.
    //unsigned int i = 2000;
    while( glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS )
    {
        // update the time, to get smooth translations
        si.setTime( glfwGetTime() );
        cursor.update();

        if( cursor.moved() )
            si.pan(cursor.getDx() / 20.0 , cursor.getDy() / 20.0 );
        if( cursor.scrolled() )
            si.zoomIn( cursor.getDz() * 10.0 );


        if(joystickPresent)
        {
            float epsilon = .005 ;

            float pos[3];
            glfwGetJoystickPos(GLFW_JOYSTICK_1, pos, 3);
            float xJoyPos = pos[0];
            float yJoyPos = pos[1];
            float zJoyPos = -pos[2];

            if( abs(xJoyPos) + abs(yJoyPos) > epsilon*2.0 )
                si.pan(xJoyPos, yJoyPos);
            if( abs(zJoyPos) > epsilon )
                si.zoomIn(zJoyPos);

            unsigned char buttons[3];
            glfwGetJoystickButtons(GLFW_JOYSTICK_1, buttons, 3);
            if(buttons[0]==GLFW_PRESS)
                fs.saveFrameBuffer();
            if(buttons[1]==GLFW_PRESS)
                si.rotate( 1.0 );
            else if(buttons[2]==GLFW_PRESS)
                si.rotate( -1.0 );
        }

        // zooming using the up/down arrow keys
        if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS)
            si.zoomIn(1.0);
        else if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
            si.zoomIn(-1.0);

        // changing iterations using left/right arrows
        if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
            si.changeIterations(-1.0);
        else if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
            si.changeIterations(1.0);

        // panning with WASD on the keyboard
        if( glfwGetKey('W') == GLFW_PRESS )
            si.pan( 0.0, 1.0 );
        if( glfwGetKey('S') == GLFW_PRESS )
            si.pan( 0.0, -1.0 );
        if( glfwGetKey('A') == GLFW_PRESS )
            si.pan( -1.0, 0.0 );
        if( glfwGetKey('D') == GLFW_PRESS )
            si.pan( 1.0, 0.0 );

        // end of input, synch and render and swap.
        si.synchParams();
        si.render();
        glfwSwapBuffers();
    }
}
