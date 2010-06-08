//uniforms are variables that can be set by the program running on the GPU.
//they allow parameters than control the shader to be changed.
//they are set before the rendering of each frame.

// viewport uniforms
uniform float deltax;
uniform float centerx;
uniform float centery;
uniform float theta;

// fractal algorithm controls
uniform int iterations;
// to determine the current location
uniform int wW;
uniform int wH;
// for pretty animations
uniform float time;

const float PI=3.1415926535;

//for pretty colors
vec4 hsv2rgb(vec3 col)
{
    //
    int iH = int(floor(col.x*6.0));
    float fH = col.x*6.0-int(iH);
    float p = col.z*(1.0-col.y);
    float q = col.z*(1.0-fH*col.y);
    float t = col.z*(1.0-(1.0-fH)*col.y);
    switch(iH)
    {
            case 0:
                return vec4(col.z, t, p, 1.0);
            case 1:
                return vec4(q, col.z, p, 1.0);
            case 2:
                return vec4(p, col.z, t, 1.0);
            case 3:
                return vec4(p, q, col.z, 1.0);
            case 4:
                return vec4(t, p, col.z, 1.0);
            case 5:
                return vec4(col.z, p, q, 1.0);
    }
}

// causes nice and pretty cycling of colors
float cycle(float val)
{
    int iPart = int(val);
    return abs(val - float(iPart));
}

//complex number math operations
vec2 add(vec2 a, vec2 b)
{
    return vec2(a.x + b.x, a.y + b.y);
}

vec2 mul(vec2 a, vec2 b)
{
    return vec2(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);
}

vec2 div(vec2 a, vec2 b)
{
    return vec2( (a.x*b.x + a.y*b.y)/(b.x*b.x + b.y*b.y), (a.y*b.x - a.x*b.y)/(b.x*b.x + b.y*b.y));
}

vec2 sub(vec2 a, vec2 b)
{
    return vec2( a.x - b.x, a.y - b.y);
}

/******************************
** END "required" stuff *******
** BEGIN custom fractal code **
*******************************/

vec2 function(vec2 v1, vec2 v2)
{
    return add( mul( v1, v1 ), v2 );
}

vec2 mandelbrot(vec2 coord, vec2 constant)
{
    int n = 0;

    float value = 0.0;
    float value2 = 0.0;

    while(++n<iterations)
    {
        coord = function(coord, constant);
        if( coord.x*coord.x + coord.y*coord.y > 4.0 )
        {
            //float(n)/float(iterations);
            value = (float(n) + (0.32663426-log(log(sqrt(coord.x * coord.x + coord.y * coord.y))))/0.693147181)/float(iterations);
            //value2 = (float(n) + (0.32663426-log(log(sqrt(coord.x * coord.x + coord.y * coord.y))))/0.693147181)/float(iterations);
            return vec2(value, float(n)/float(iterations) );
        }
    }
    return vec2(.5, .5);
}

void main(void)
{
    float real, imag, value, deltay;
    deltay = float(wH)/float(wW) * deltax;
    real = (0.0 - deltax/2.0) + (gl_FragCoord.x/float(wW))*deltax;
    imag = (0.0 - deltay/2.0) + (gl_FragCoord.y/float(wH))*deltay;

    float realR, imagR, thetaPi, cosThetaPi, sinThetaPi;

    float offsetx, offsety;

    offsetx = real;
    offsety = imag;

    thetaPi = theta * PI;

    cosThetaPi = cos(thetaPi);
    sinThetaPi = sin(thetaPi);

    realR = (offsetx*cosThetaPi - offsety*sinThetaPi);
    imagR = (offsetx*sinThetaPi + offsety*cosThetaPi);

    vec2 coord, initcoord, results;

    coord = vec2(realR, imagR);
    initcoord = coord;

    vec2 juliaconstant = vec2(centerx, centery);

    results = mandelbrot(coord, juliaconstant);

    //good for J-Set
    float v =  results.x*10.0;
    float h =  .45;//results.y;
    float s = .88;

    /*float v = results.y * 2.0;
    float h = 0.0;//results.x;
    float s = 0.0;*/

    gl_FragColor = hsv2rgb(vec3(h, s, v));//vec4(1.0, g, b, 1.0);
};


