#include <stdio.h>
#include <stdlib.h>

//#include <GL/gl3.h>
//#include <GL/glext.h>
//-lGL -lglfw -lXrandr -lpthread -lz -lpnglite -lftgl
//-lGL -lglfw -lXrandr -lpthread -lz -lpnglite -lftgl
// glfw should include everything I need for ogl.
#include <GL/glfw.h>
// ftgl is used for rendering text.
#include <FTGL/ftgl.h>

#include <math.h>
#include <sys/time.h>
#include <pnglite.h>

#define keyboard 0
#define zoom_factor .02

#define scroll_zoom_factor .05

// C doesn't have boolean primitives
#define bool int
#define true 0xFFFFFFFF
#define false 0

#define INT 0
#define FLOAT 1

#define NPARAMS 7
//11

//static const int window_width = 1440;
//static const int window_height = 900;

const float pi = 3.1415926;

int window_width;
int window_height;

void HandleGLErrors( char *function_name)
{
    printf("%s: ",function_name);
    GLenum error_code = glGetError();
    switch(error_code)
    {
            case GL_NO_ERROR:
                printf("GL_NO_ERROR");
                break;
            case GL_INVALID_ENUM:
                printf("GL_INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                printf("GL_INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                printf("GL_INVALID_VALUE");
                break;
            case GL_OUT_OF_MEMORY:
                printf("GL_OUT_OF_MEMORY");
                break;
            default:
                printf("unknown error: 0x%x", error_code);
                break;
    }
    printf("\n");
}

GLhandleARB SetupFragmentShader(char *filename)
{
    // load shader from file
    FILE *pFile;
    pFile = fopen( filename, "r" );

    if(pFile==0){
        printf("Error opening shader file for reading.");
    }

    fseek(pFile, 0L, SEEK_END);  /* Position to end of file */
    long lSize = ftell(pFile);     /* Get file length */
    rewind(pFile);               /* Back to start of file */

    // allocate room in memory for the shader source.
    GLchar * buffer = (char*) malloc (sizeof(char)*lSize);
    // read the shader source into memory.
    fread(buffer, 1, lSize, pFile);

    // close the file.
    fclose (pFile);

    // setup my shader
    GLhandleARB fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // read the shader source into opengl.
    glShaderSource( fragmentShader, 1, &buffer, NULL);
    // now that shader source is loaded into openGL, free that memory.
    free (buffer);

    // compile shader.
    glCompileShader( fragmentShader );

    // check for compile errors.
    GLint compiled;
    glGetObjectParameterivARB(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (compiled)
    {
        printf("success compiling shader\n");
    }else
    {
        GLint blen = 0;
        GLsizei slen = 0;

        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH , &blen);

        if(blen>1)
        {
            GLchar* compiler_log = (GLchar*)malloc(blen);
            glGetInfoLogARB(fragmentShader, blen, &slen, compiler_log);
            printf("%s\n", compiler_log);
            free (compiler_log);
        }

        printf("no success compiling shader\n");
    }

    // create program and attach shader.
    GLhandleARB program = glCreateProgram();
    glAttachShader(program, fragmentShader);

    // link the program up with render pipeline.
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked)
    {
        printf("success linking shader\n");
    }else
    {
        printf("no success linking shader\n");
    }
    return program;
}

int baseNum = 0;
int saveNumber = 0;

void saveFrameBuffer()
{
    unsigned char* pixBuffer = (unsigned char*)malloc(window_width*window_height*4);
    glReadPixels(0,0, window_width, window_height, GL_RGB, GL_UNSIGNED_BYTE, pixBuffer);

    char filename[50];
    sprintf(filename, "%d-%d.png",baseNum, ++saveNumber);

    // initialize
    png_t pngStruct;
    png_init(0, 0);
    if(png_open_file_write(&pngStruct, filename)==0)
    if(png_set_data(&pngStruct, window_width, window_height, 8, PNG_TRUECOLOR, pixBuffer)==0)
    if(png_close_file(&pngStruct)==0)
    printf("Saved frame buffer.\n");
}

GLint minxLoc, minyLoc, deltaxLoc, deltayLoc, iterationsLoc, timeLoc, wWLoc, wHLoc, imagcLoc, realcLoc, thetaLoc;

GLfloat minx = -2.0f;
GLfloat miny = -1.0f;
GLfloat deltax = 3.0f;
GLfloat deltay = 2.0f;
GLint iterations = 500;

GLfloat theta = 0.0;

GLfloat imagc = .4;
GLfloat realc = -.6;




typedef struct {
    char *name;
    GLint loc;
    int utype;
    union u_tag {
        GLint ival;
        GLfloat fval;
    } val;
    union mntag {
        GLint ival;
        GLfloat fval;
    } min;
    union mxtag {
        GLint ival;
        GLfloat fval;
    } max;
    bool needs_update;

} Parameter;

typedef struct {
    Parameter *minx;
    Parameter *miny;
    Parameter *deltax;
    Parameter *theta;
} Viewport;


Parameter createGenericParameter (GLhandleARB program, char* name, uint type)
{
    Parameter temp;
    temp.name = name;
    temp.utype = type;
    temp.loc = glGetUniformLocation(program, temp.name);
    temp.needs_update = true;
    return temp;
}

Parameter createParameter1i ( GLhandleARB program, char* name, GLint init_val ){
    Parameter temp = createGenericParameter(program, name, INT );
    temp.val.ival = init_val;
    return temp;
}
Parameter createParameter1f ( GLhandleARB program, char* name, GLfloat init_val ){
    Parameter temp = createGenericParameter(program, name, FLOAT );
    temp.val.fval = init_val;
    return temp;
}

Parameter parameters[NPARAMS];

void prepareUniforms(GLhandleARB program)
{


    minxLoc = glGetUniformLocation(program, "minx");
    minyLoc = glGetUniformLocation(program, "miny");
    deltaxLoc = glGetUniformLocation(program, "deltax");
    deltayLoc = glGetUniformLocation(program, "deltay");
    iterationsLoc = glGetUniformLocation(program, "iterations");


    thetaLoc =  glGetUniformLocation(program, "theta");
    //for changing values in the julia set:
    imagcLoc = glGetUniformLocation(program, "imagc");
    realcLoc = glGetUniformLocation(program, "realc");

    timeLoc = glGetUniformLocation(program, "time");

    //get locations for setting the window width and height in the shader, should resame throughout program exe.
    //wWLoc = glGetUniformLocation(program, "wW");
    //wHLoc = glGetUniformLocation(program, "wH");
}

void synchVariableUniforms()
{
    glUniform1f(minxLoc, minx );
    glUniform1f(minyLoc, miny );
    glUniform1f(deltayLoc, deltay );
    glUniform1f(deltaxLoc, deltax );
    glUniform1f(imagcLoc, imagc );
    glUniform1f(realcLoc, realc );
    glUniform1i(iterationsLoc, iterations);
    glUniform1f(thetaLoc, theta);
}


bool synchParameters (Parameter *params)
{
    bool success = true;
    int param_i;
    for ( param_i = 0; param_i < sizeof params; param_i++ )
    {
        Parameter *param = &parameters[param_i];
        if ( param->needs_update )
        {
            switch ( param->utype )
            {
                case INT:
                    glUniform1i(param->loc, param->val.ival);
                    break;
                case FLOAT:
                    glUniform1f(param->loc, param->val.fval);
                    break;
                default:
                    success &= false;
                    break;
            }
            param->needs_update = false;
        }
    }
    return success;
}



void zoomIn(GLfloat amount)
{
    //increase minx and miny by some small percentage of deltax and deltay
    minx += deltax*amount;
    miny += deltay*amount;

    //decrease deltax and deltay by 2 times that small percentage.
    deltax -= 2*deltax*amount;
    deltay -= 2*deltay*amount;
}

void pan(GLfloat dx, GLfloat dy)
{
    minx -= dx*cos(pi * theta) - dy*sin(pi *  theta );
    miny -= dx*sin(pi * theta) + dy*cos(pi *  theta );
}

void rotate(GLfloat dr)
{
    theta += dr;
    if ( theta > 1.0 )
    {
        theta -= 2.0;
    }else if (theta < -1.0)
    {
        theta += 2.0;
    }
}

Viewport* pan2( Viewport *viewport, GLfloat dx, GLfloat dy )
{
    GLfloat theta = viewport->theta->val.fval;
    viewport->minx->val.fval -= dx*cos(pi * theta) - dy*sin(pi *  theta );
    viewport->miny->val.fval -= dx*sin(pi * theta) + dy*cos(pi *  theta );
    viewport->minx->needs_update = true;
    viewport->miny->needs_update = true;
    return viewport;
}
/*void rotate2( Viewport *viewport )
{

}*/

void printInstructions()
{
    printf("Use mouse to pan image, use mouse wheel to zoom in/out. Alternatively, use up and down arrow to zoom in/out.\n");
    printf("Press 'r' to reset view.\n");
    printf("Press 'p' to take a screenshot.\n");
    printf("Press 'd' to print view settings.\n");
    printf("Press 'f' to toggle printing the frames per second and iterations.\n");
}

# define WIDTH 0
# define HEIGHT 1
# define TIME 2
# define MINX 3
# define MINY 4
# define DELTA 5
# define THETA 6

int main(int argc, char *argv[])
{
    char *shader = "mandelbrot (copy).frag";
    if (argc > 1)
    {
        shader = argv[1];
    }

    glfwInit();



    /*int major, minor, rev;
    glfwGetGLVersion(&major, &minor, &rev);
    printf("using openGL %d.%d.%d\n", major, minor, rev);*/

    GLFWvidmode mode;
    glfwGetDesktopMode( &mode );

    window_width = mode.Width;
    window_height = mode.Height;

    printf("%dx%d\n", window_width, window_height);
    //glfwOpenWindow(window_width, window_height, 5, 6, 5, 0, 8, 0, GLFW_FULLSCREEN);
    glfwOpenWindow(mode.Width, mode.Height, mode.RedBits,  mode.GreenBits, mode.BlueBits, 0, 0, 0, GLFW_FULLSCREEN);

    // setup opengl perspective stuff.
    glMatrixMode(GL_PROJECTION);
    float aspect_ratio = ((float)window_height) / window_width;
    glFrustum(.5, -.5, -.5 * aspect_ratio, .5 * aspect_ratio, 1, 50);
    glDisable( GL_DEPTH_TEST );
    glTranslatef(0.0, 0.0, -1.00);
    glClearColor(0.0, 1.0, 1.0, 0.0);


    // will create and set shader program.
    GLhandleARB program = SetupFragmentShader(shader);
    // get the location of all the uniforms
    prepareUniforms(program);
    // must call use program before setting uniforms values
    glUseProgram(program);

    uint param_i = 0;
    parameters[WIDTH]   = createParameter1i(program, "wW", window_width );
    parameters[HEIGHT]  = createParameter1i(program, "wH", window_height );
    parameters[TIME]    = createParameter1f(program, "time", 0.0 );
    parameters[MINX]    = createParameter1f(program, "minx", -2.0);
    parameters[MINY]    = createParameter1f(program, "miny", 1.0);
    parameters[DELTA]   = createParameter1f(program, "deltax", 3.0);
    parameters[THETA]   = createParameter1f(program, "theta", 1.0);
    //synchParameters ( parameters );

    /*for ( param_i = 0; param_i < NPARAMS; param_i++ )
    {
        Parameter *param = &parameters[param_i];
        switch ( param->utype )
        {
            case INT:
                glUniform1i(param->loc, param->u.ival);
                break;
            case FLOAT:
                glUniform1f(param->loc, param->u.fval);
                break;
            default:
                break;
        }
    }*/

    // set the dimension, must be after program is used.
    //glUniform1i(wWLoc, window_width );
    //glUniform1i(wHLoc, window_height );

    printInstructions();

    double frame_start = glfwGetTime();
    double frame_time = 0.0;
    double temp_time = 0.0;

    //bool need_draw = true;
    bool need_view_synch = true;


    int mouseWheel = 0;
    int mouseWheelDelta = 0;

    double fIterations = (double)(iterations);

    int mouseX, mouseY, curMouseX, curMouseY;
    mouseX = mouseY = 0;

    bool joystickPresent = glfwGetJoystickParam( GLFW_JOYSTICK_1, GLFW_PRESENT );
    if(joystickPresent)
    {
        printf("Joystick present. Can be used to navigate in addition to mouse.\n");
    }else
    {
        printf("no joystick present.\n");
    }

    // set up fonts

    FTGLfont *font = NULL;
    //font = ftglCreateBufferFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
    font = ftglCreatePixmapFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf");
    if( !font )
    {
        printf("failed to load font.");
    }
    ftglSetFontFaceSize(font, 50, 50);
    //printf("%d\n", ftglGetFontError(font));
    //ftglSetFontCharMap(font, ft_encoding_unicode);
    //printf("%d\n", ftglGetFontError(font));

    bool show_fps = true;
    char text[256];

    // keep on rendering the frame until escape is pressed.
    while(glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS)
    {
        temp_time = glfwGetTime();
        frame_time = temp_time - frame_start;
        frame_start = temp_time;

        // process input
        if(glfwGetKey('E') == GLFW_PRESS)
        {
            printf("minx: %.31f\n", minx);
            printf("miny: %.31f\n", miny);
            printf("deltax: %.31f\n",deltax);
            printf("deltay: %.31f\n",deltay);
            printf("====================================\n");
        }else if(glfwGetKey('P') == GLFW_PRESS)
        {
            saveFrameBuffer();
        }else if(glfwGetKey('R') == GLFW_PRESS)
        {
            minx = -2.0;
            miny = -1.0;
            deltax = 3.0;
            deltay = 2.0;
            need_view_synch = true;
        }else if(glfwGetKey('F') == GLFW_PRESS)
        {
            show_fps=~show_fps;
            //printf("fps display toggled to: %d\n", show_fps);
        }

        else if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            fIterations += 5*(60*frame_time);
            iterations = floor(fIterations);
            need_view_synch = true;
        }else if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if(iterations>1)
            {
                fIterations -= 5*(60*frame_time);
            }
            iterations = floor(fIterations);
            need_view_synch = true;
        }
        // zooming using the arrow keys
        if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS)
        {
            zoomIn(2.0*zoom_factor*(frame_time*60.0));
            need_view_synch = true;
        }else if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            zoomIn(-2.0*zoom_factor*(frame_time*60.0));
            need_view_synch = true;
        }

        //changing the julia set constant
        if(glfwGetKey('W') == GLFW_PRESS)
        {
            realc += .0025*frame_time*60.0;
            need_view_synch = true;
        }else if(glfwGetKey('S') == GLFW_PRESS)
        {
            realc -= .0025*frame_time*60.0;
            need_view_synch = true;
        }
        if(glfwGetKey('A') == GLFW_PRESS)
        {
            imagc += .0025*frame_time*60.0;
            need_view_synch = true;
        }else if(glfwGetKey('D') == GLFW_PRESS)
        {
            imagc -= .0025*frame_time*60.0;
            need_view_synch = true;
        }

        // changing the area visible (panning left and right)
        glfwGetMousePos(&curMouseX, &curMouseY);
        int mouseDeltax = curMouseX - mouseX;
        int mouseDeltay = curMouseY - mouseY;

        if(mouseDeltax||mouseDeltay)//if one of them is different
        {
            //printf("delta coordinates: (%d, %d)\n", mouseDeltax, mouseDeltay);
            mouseX=curMouseX;
            mouseY=curMouseY;
            //printf("new mouse coordinates: (%d, %d)\n", x, y);
            pan(zoom_factor*deltax*mouseDeltax/20.0, -1*zoom_factor*deltay*mouseDeltay/20.0);
            need_view_synch = true;
        }
        if(joystickPresent)
        {
            float pos[4];
            glfwGetJoystickPos(GLFW_JOYSTICK_1, pos, 4);
            //the above function gives us bad results for my particular joystick :(, must fiddle around with numbers to get correct answer.
            float xJoyPos = pos[0];//(pos[0]*32767)/128.0 - 1.0;
            float yJoyPos = pos[1];//(pos[1]*32767)/128.0 + 1.0;
            float zJoyPos = -1*pos[2];//-1*((pos[2]*32767)/128.0 - 1.0);
            float rJoyPos = pos[3];
            float temp = 0.0;
            //printf("%f\n", xJoyPos);

            if(xJoyPos>.02||xJoyPos<-.02)
            {
                temp = zoom_factor*xJoyPos*deltax*1.0*frame_time*60.0;
                pan(temp, 0);
                need_view_synch = true;
            }
            if(yJoyPos>.02||yJoyPos<-.02)
            {
                temp = zoom_factor*yJoyPos*deltay*1.0*frame_time*60.0;
                pan(0, temp);
                need_view_synch = true;
            }
            if(zJoyPos>.04||zJoyPos<-.04)
            {
                zoomIn(zJoyPos*scroll_zoom_factor*(frame_time*10.0));
                need_view_synch = true;
            }
            if(rJoyPos>.02 || rJoyPos<-.02)
            {
                rotate( rJoyPos/5.0*frame_time );
                need_view_synch = true;
            }
            unsigned char buttons[5];

            glfwGetJoystickButtons(GLFW_JOYSTICK_1, buttons, 4);
            if(buttons[0]==GLFW_PRESS)
            {
                saveFrameBuffer();
            }
            if(buttons[1]==GLFW_PRESS)
            {
                rotate( 0.35*frame_time );
                need_view_synch = true;
            }else if(buttons[2]==GLFW_PRESS)
            {
                rotate( -0.35*frame_time );
                need_view_synch = true;
            }

            //printf("x:%f, y:%f\n", xJoyPos, yJoyPos);
        }

        // zooming using the mouse scroll
        mouseWheelDelta = glfwGetMouseWheel() - mouseWheel;
        mouseWheel = mouseWheel+mouseWheelDelta;
        if(mouseWheelDelta != 0)
        {
            zoomIn(mouseWheelDelta*scroll_zoom_factor*(frame_time*60.0));
            need_view_synch = true;
        }

        //printf("mouse wheel: %d\n", mouseWheelDelta);

        if(need_view_synch)
        {
            synchVariableUniforms();
            need_view_synch = false;
        }

        // update the time inside the shader for cool animations.

        parameters[2].val.fval = frame_start;
        parameters[2].needs_update = true;

        synchParameters ( parameters );

        //glUniform1f(timeLoc, frame_start);

        // write fps and iterations to a string.
        sprintf(text, "FPS: %4.1d; ITER: %d; ZOOMX: %-10.1f", (int)(floor(1.0/frame_time)), iterations, (3.0/deltax));

        //sprintf("fps %f\n", 1.0/frame_time);

        // clear the buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //since all calculations are being done in the fragment shader, all we draw is a surface.
        glUseProgram(program);
        glRects(-1, -1, 1, 1);

        // render font on top of this?
        if(show_fps != 0){
            glUseProgram(0);
            glColor3f(0.0f, 0.0f, 0.0f);
            ftglRenderFont(font, text, FTGL_RENDER_ALL);
            glUseProgram(program);
        }
        // note: swap buffers also updates the input events for glfw
        glfwSwapBuffers();
    }
    // clean up the font and exit.
    ftglDestroyFont(font);
    return 1;
}
