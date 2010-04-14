//uniforms are variables that can be set by the program running on the GPU.
//they allow parameters than control the shader to be changed.
//they are set before the rendering of each frame.

//viewport uniforms

uniform float deltax;
//uniform float deltay;

uniform float minx;
uniform float miny;
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

vec2 function(vec2 v1, vec2 v2)
{
    return add( mul( v1, v1 ), v2 );
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
            return vec2(value, .90);
        }
    }
    return vec2(0.0, 0.0);
}


vec2 f(vec2 x)
{
    return sub(mul(mul(mul(x, x), x), x) , vec2(1.0, 0.0));
}


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
    return vec2(sink, float(n)/float(iterations));
}

/*
vec2 newtonfractal(vec2 z)
{
    int n = 0;
    float sink = 0.0;
    while ( ++n < iterations )
    {
        z = znext(z);
        if (z.x > .99999 && z.x < 1.00001 ) {
            sink = .2;
            break;
        } else if (z.y > .99999 && z.y < 1.00001 ){
            sink = .4;
            break;
        } else if (z.x < -.99999 && z.x > -1.00001 ) {
            sink = .6;
            break;
        } else if (z.y < -.99999 && z.y > -1.00001 ){
            sink = .8;
            break;
        }
    }
    return vec2(sink, float(n)/float(iterations));
}
*/

void main(void)
{
    float real, imag, value, deltay;
    deltay = float(wH)/float(wW) * deltax;
    real = minx + (gl_FragCoord.x/float(wW))*deltax;
    imag = miny + (gl_FragCoord.y/float(wH))*deltay;

    float realR, imagR, thetaPi, cosThetaPi, sinThetaPi;

    float centerX, centerY, offsetX, offsetY;

    centerX = minx + deltax/2.0 ;
    centerY = miny + deltay/2.0 ;

    offsetX = centerX - real;
    offsetY = centerY - imag;

    thetaPi = theta * PI;

    cosThetaPi = cos(thetaPi);
    sinThetaPi = sin(thetaPi);

    realR = centerX + (offsetX*cosThetaPi - offsetY*sinThetaPi);
    imagR = centerY + (offsetX*sinThetaPi + offsetY*cosThetaPi);

    vec2 coord, initcoord, results;

    coord = vec2(realR, imagR);
    initcoord = coord;

    vec2 juliaconstant = vec2(realc, imagc);

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
    float h = cycle(results.x+time/15.0);//.5 - .5*sin(results.x - time/30.0);
    //results.x*10.0;//
    float s = .88;

    gl_FragColor = hsv2rgb(vec3(h, s, v));//vec4(1.0, g, b, 1.0);
};
