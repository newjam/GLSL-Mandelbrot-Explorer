#version 120

uniform float deltax;
uniform float deltay;
uniform float minx;
uniform float miny;
uniform int iterations;

uniform int wW;
uniform int wH;

uniform float time;

const float PI=3.1415926535;

vec4 hsv2rgb(vec3 col)
{
    //
    int iH = int(floor(col.x*6.0));
    float fH = col.x*6.0-float(iH);
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

void main(void)
{
    float tempreal, real, imag, initreal, initimag;

    float v = 0.0;
    float h = 0.0;
    float s = .88;

    float value = 0.0;

    float vals[4];

    float yDeltas[4];
    float xDeltas[4];

    yDeltas[0] = -.25;
    yDeltas[1] = -.25;
    yDeltas[2] =  .25;
    yDeltas[3] =  .25;

    xDeltas[0] = .25;
    xDeltas[1] = -.25;
    xDeltas[2] = .25;
    xDeltas[3] = -.25;


    for(int i = 0; i < 4; ++i)
    {
        real = minx + (( gl_FragCoord.x + xDeltas[i] )/float(wW))*deltax;
        imag = miny + (( gl_FragCoord.y + yDeltas[i] )/float(wH))*deltay;
        initreal = real;
        initimag = imag;
        int n = 0;

        while(++n<iterations)
        {
            tempreal = real*real - imag*imag + initreal;
            imag = real * imag * 2.0 + initimag;
            real = tempreal;
            if(real*real+imag*imag>4.0)
            {
                //value = float(n)/float(iterations);
                //value = (float(n) + (log(2.0*log(2.0)-log(log(sqrt(real*real+imag*imag))))/log(2.0)))/float(iterations);
                value = (float(n) + (0.32663426-log(log(sqrt(real*real+imag*imag))))/0.693147181)/float(iterations);
                //
                v = .88;
                vals[i] = value;
                break;
            }
        }
    }

    float sum = 0.0;
    for (int i = 0; i < 4; i++)
    {
        sum += vals[i];
    }

    value = sum/4.0;

    h = cycle(value-time/15.0);

    gl_FragColor = hsv2rgb(vec3(h, s, v));//vec4(1.0, g, b, 1.0);
}
