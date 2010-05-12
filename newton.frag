//uniforms are variables that can be set by the program running on the GPU.
//they allow parameters than control the shader to be changed.
//they are set before the rendering of each frame.

//viewport uniforms

uniform float deltax;
//uniform float deltay;

//uniform float minx;
//uniform float miny;

uniform float centerx;
uniform float centery;

uniform float theta;

uniform int iterations;

//for constants in j-set
uniform float realc = 0.0;
uniform float imagc = 0.0;

//to determine the current location
uniform int wW;
uniform int wH;

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

//complex number math functions
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

//http://mathworld.wolfram.com/NewtonsMethod.html
vec2 fp(vec2 x)
{
    vec2 tmp = mul( mul(x, x), x );
    return vec2(4.0*tmp.x, 4.0*tmp.y);
}

//should have roots on +/- 1.27201965 and +/- 0.786151378i
//x^4-x^2-1
vec2 f2(vec2 x)
{
    return sub(sub(mul(mul(mul(x, x), x), x), mul(x, x)) , vec2(1.0, 0.0));
}

//http://mathworld.wolfram.com/NewtonsMethod.html
vec2 fp2(vec2 x)
{
    //x^3
    vec2 tmp1 = mul( mul(x, x), x );
    //2x
    vec2 tmp2 = vec2(2.0*x.x, 2.0*x.y);
    //4x^3
    vec2 tmp3 = vec2(4.0*tmp1.x, 4.0*tmp1.y);
    //4x^2-2x
    vec2 tmp4 = sub(tmp3, tmp2);
    return tmp4;
}

vec2 znext(vec2 z)
{
        return sub( z , div(f2(z), fp2(z) ));
}

vec2 newtonfractal(vec2 z)
{
    int n = 0;
    float sink = 0.0;
    while ( ++n < iterations )
    {
        z = znext(z);
        if (z.x > 1.272018 && z.x < 1.272020 ) {
            sink = .2;
            break;
        } else if (z.y > .7861512 && z.y < .7861514 ){
            sink = .4;
            break;
        } else if (z.x < -1.272018 && z.x > -1.272020 ) {
            sink = .6;
            break;
        } else if (z.y < -.7861512 && z.y > -.7861514 ){
            sink = .8;
            break;
        }
    }
    //(float(n) + (0.32663426-log(log(sqrt(z.x * z.x + z.y * z.y))))/0.693147181)/float(iterations)
    //float(n)/float(iterations)
    return vec2(sink, float(n)/float(iterations));
}

void main(void)
{
    float real, imag, value, deltay;
    deltay = float(wH)/float(wW) * deltax;
    real = (centerx - deltax/2.0) + (gl_FragCoord.x/float(wW))*deltax;
    imag = (centery - deltay/2.0) + (gl_FragCoord.y/float(wH))*deltay;

    float realR, imagR, thetaPi, cosThetaPi, sinThetaPi;

    float /*centerX, centerY,*/ offsetx, offsety;

    //centerX = minx + deltax/2.0 ;
    //centerY = miny + deltay/2.0 ;

    offsetx = centerx - real;
    offsety = centery - imag;

    thetaPi = theta * PI;

    cosThetaPi = cos(thetaPi);
    sinThetaPi = sin(thetaPi);

    realR = centerx + (offsetx*cosThetaPi - offsety*sinThetaPi);
    imagR = centery + (offsetx*sinThetaPi + offsety*cosThetaPi);

    vec2 coord, initcoord, results;

    coord = vec2(realR, imagR);
    //initcoord = coord;

    //vec2 juliaconstant = vec2(realc, imagc);

    results = newtonfractal(coord);

    //good for newton's method:
    float v = 1.0 - results.y;
    float h = results.x;
    float s = .88;


    gl_FragColor = hsv2rgb(vec3(h, s, v));//vec4(1.0, g, b, 1.0);
};
