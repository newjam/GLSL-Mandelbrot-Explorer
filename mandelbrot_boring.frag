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

uniform float power;

//to determine the current location
uniform int wW;
uniform int wH;

uniform float time;

const float PI=3.1415926535;
const float E =2.71828183;



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

/*float arg(vec2 a)
{
    if(a.x<0)
    {
        return PI;
    }else
    {
        return 2.0 * atan(a.y /(pow((a.x*a.x + a.y*a.y), .5) + a.x) );
    }
}

//complex number math functions
vec2 cpow(vec2 a, float x)
{
  //a^z * e^(ib*z)

  //float x = a.y*z;
  //float u = pow(a.x, z);
  //vec2( u*cos(x), u*sin(x) );

  float u = pow(a.x*a.x + a.y*a.y, x/2.0);
  float v = arg(a);
  return vec2( u * cos( v * x ) , u * sin( v * x ) );
}*/


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

//
vec2 function(vec2 v1, vec2 v2)
{
    return add( mul( v1, v1 ), v2 );//add( mul( mul( v1, v1 ), v1) , div(v2, v1) );
}


vec2 function2(vec2 v1, vec2 v2)
{
    return add( mul( v1, mul( v1, v1 )), v2 );
}

vec2 function3(vec2 v1, vec2 v2)
{
    return add( sub(mul( v1, v1 ), v2), v1 );
}

vec2 function4(vec2 v1, vec2 v2)
{
    return add(mul( mul(v1 ,v1), v2), v2);
}

//various fractals???
vec2 function5(vec2 v1, vec2 v2)
{
    vec2 tmp = add(v1, v2);
    return add(mul(tmp,tmp), v2);
}

vec2 function6(vec2 v1, vec2 v2)
{
    return mul(function2(v1, v2), function3(v2, v1));
}

vec2 function7(vec2 v1, vec2 v2)
{
    return sub(sub(mul(v1, v1), v1), v2);
}


// first value of the vector holds 0 for not in set and 1 for in set
// second value holds the smoothed "value"
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
            return vec2(value, 1.0);
        }
    }
    return vec2(0.0, 0.0);
}



void main(void)
{
    float real, imag, value, deltay;
    deltay = float(wH)/float(wW) * deltax;
    real = (centerx - deltax/2.0) + (gl_FragCoord.x/float(wW))*deltax;
    imag = (centery - deltay/2.0) + (gl_FragCoord.y/float(wH))*deltay;

    float realR, imagR, thetaPi, cosThetaPi, sinThetaPi;

    float offsetx, offsety;

    offsetx = centerx - real;
    offsety = centery - imag;

    thetaPi = theta * PI;

    cosThetaPi = cos(thetaPi);
    sinThetaPi = sin(thetaPi);

    realR = centerx + (offsetx*cosThetaPi - offsety*sinThetaPi);
    imagR = centery + (offsetx*sinThetaPi + offsety*cosThetaPi);

    vec2 coord, initcoord, results;

    coord = vec2(realR, imagR);
    initcoord = coord;

    //vec2 juliaconstant = vec2(realc, imagc);

    //results = mandelbrot(coord, juliaconstant);
    results = mandelbrot(coord, coord);
    //results = newtonfractal(coord);

    //good for newton's method:
    //float v = 1.0 - results.y;
    //float h = results.x;
    //float s = .88;

    //results.y;//results.y * results.x*10.0;//1.0-results.y ;//3.0*(results.y*results.x);//.5 + .5*sin(results.y/10.0);//pow(results.x, .35);//cycle(results.x-time/15.0);//.5 + sin(results.y)/2.0;

    //good for J-Set
    //float v = results.y * results.x*10.0;
    //float h = results.y;
    //float s = .88;

    //good for MB set
    float v = results.y; //for lightness/brightness
    //float v = results.x;
    //results.y*results.x*10.0; // for darkness/highlights
    //float h = 0.0;
    float h = .0;//cycle(results.x+time/15.0);//.5 - .5*sin(results.x - time/30.0);
    //results.x*10.0;//
    float s = .0;

    gl_FragColor = hsv2rgb(vec3(h, s, v));//vec4(1.0, g, b, 1.0);
};



