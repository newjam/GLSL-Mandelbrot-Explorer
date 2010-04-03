#include "utils.h"

FrameSaver :: FrameSaver (int width, int height)
{
    // keeps track of the current picture, used for the filename.
    saveCount = 0;

    // save off time to be the unique basename for the filename for this session
    struct timeval tim;
    gettimeofday(&tim, NULL);
    baseNum = tim.tv_usec;

    // width and height are needed for malloc and reading/writing.
    windowW = width;
    windowH = height;
}

Cursor :: Cursor ()
{
    x = dx = y = dy = z = dz = 0;
}
void Cursor :: update()
{
    int nx, ny;
    glfwGetMousePos(&nx, &ny);
    dx = x - nx;
    dy = y - ny;
    x  = nx;
    y  = ny;
    int nz;
    nz = glfwGetMouseWheel();
    dz = z - nz;
    z  = nz;
}



void FrameSaver :: saveFrameBuffer( )
{
    unsigned char* pixBuffer = (unsigned char*)malloc(windowW*windowH*3);
    glReadPixels(0,0, windowW, windowH, GL_RGB, GL_UNSIGNED_BYTE, pixBuffer);

    stringstream ss;
    ss << baseNum << "-" << ++saveCount ;
    const char* filename = ss.str().c_str();
    //sprintf(filename, "%d-%d.png",baseNum, ++saveCount);

    png_t pngStruct;
    png_init(0, 0);
    if(png_open_file_write(&pngStruct, filename )==0)
    if(png_set_data(&pngStruct, windowW, windowH, 8, PNG_TRUECOLOR, pixBuffer)==0)
    if(png_close_file(&pngStruct)==0)
    {
    printf("Saved frame buffer.\n");
    }
    free ( pixBuffer );
}

char* getContents(string fName)
{
    int size;

    ifstream sourceFile;
    sourceFile.open ( fName.c_str(), ios::ate );

    size = (int) sourceFile.tellg();

    char* source = new char[size+1];

    sourceFile.seekg (0, ios::beg);
    sourceFile.read (source, size);

    sourceFile.close();
    return source;
}

// NOTE: any OGL, functions require a function context to already be present.
GLhandleARB createShader(const char* source)
{
    const GLchar** pSource = &source;
    // setup my shader
    GLhandleARB fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // read the shader source into opengl.
    glShaderSource( fragmentShader, 1, pSource, NULL);
    // now that shader source is loaded into openGL, free that memory.
    return fragmentShader;
}

void compileShader(GLhandleARB fragmentShader)
{
    // compile shader.
    glCompileShader( fragmentShader );

    // check for compile errors.
    GLint compiled;
    glGetObjectParameterivARB(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE)
    {
        printf("success compiling shader\n");
    }else
    {
        GLint blen = 0;
        GLsizei slen = 0;

        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH , &blen);

        cout<<blen;

        if(blen>1)
        {
            //GLchar* compiler_log = (GLchar*)malloc(blen);
            GLchar compiler_log[blen];
            glGetInfoLogARB(fragmentShader, blen, &slen, compiler_log);
            printf("%s\n", compiler_log);
            //cout << compiler_log << "\n";
            delete compiler_log;
        }

        printf("no success compiling shader\n");
    }
}

GLhandleARB linkShader(GLhandleARB fragmentShader)
{
    // create program and attach shader.
    GLhandleARB program = glCreateProgram();
    glAttachShader(program, fragmentShader);

    // link the program up with render pipeline.
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked == GL_TRUE)
    {
        printf("success linking shader\n");
    }else
    {
        printf("no success linking shader\n");
    }
    return program;
}
