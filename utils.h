#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <cstdlib>

#include <sys/time.h>
#include <pnglite.h>

#include <GL/glfw.h>

using namespace std;

class FrameSaver
{
private:
    int windowH;
    int windowW;
    int baseNum;
    int saveCount;
public:
    FrameSaver( int x, int y);
    void saveFrameBuffer( );
};

class Cursor
{
private:
    int x, y, z, dx, dy, dz;
public:
    Cursor();
    inline bool moved()     {return dx || dy;   }
    inline bool scrolled()  {return dz;         }
    void update();
    inline int getDx()      { return -dx;       }
    inline int getDy()      { return dy;        }
    inline int getDz()      { return -dz;        }
};


char* getContents(string fName);

// NOTE: any OGL, functions require a function context to already be present.
GLhandleARB createShader(const char* source);

void compileShader(GLhandleARB fragmentShader);

GLhandleARB linkShader(GLhandleARB fragmentShader);
