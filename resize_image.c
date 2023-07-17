#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int x1=round(x);
    if(x1<0) x1=0;
    int y1=round(y);
    if(y1<0) y1=0;
    float pixel=im.data[x1+y1*im.w+c*im.w*im.h];
    return pixel;
}

image nn_resize(image im, int w, int h)
{
    float o_w=im.w;
    float o_h=im.h;
    float ax=(o_w/w);
    float bx=(o_w-w)/(2.0*w);
    float ay=(o_h/h);
    float by=(o_h-h)/(2.0*h);
    image im1=make_image(w,h,im.c);
    for(int i=0;i<im.c;i++){
        for(int j=0;j<h;j++){
            for(int k=0;k<w;k++){
                im1.data[k+j*w+i*w*h]=nn_interpolate(im,ax*k+bx,ay*j+by,i);
            }
        }
    }
    return im1;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int x1=floor(x);
    int y1=floor(y);
    int x2=ceil(x);
    int y2=ceil(y);

    float q11=get_pixel(im,x1,y1,c);
    float q12=get_pixel(im,x1,y2,c);
    float q21=get_pixel(im,x2,y1,c);
    float q22=get_pixel(im,x2,y2,c);

    float q1=(y-y1)*q12+(y2-y)*q11;
    float q2=(y-y1)*q22+(y2-y)*q21;

    float q=(x-x1)*q2+(x2-x)*q1;
    return q;
}

image bilinear_resize(image im, int w, int h)
{
    float o_w=im.w;
    float o_h=im.h;
    float ax=(o_w/w);
    float ay=(o_h/h);
    image im1=make_image(w,h,im.c);

    for(int i=0;i<im.c;i++){
        for(int j=0;j<h;j++){
            for(int k=0;k<w;k++){
                float q=bilinear_interpolate(im,(k*ax+0.5*(ax-1)),(j*ay+0.5*(ay-1)),i);
                im1.data[k+j*w+i*w*h]=q;
            }
        }
    }
    return im1;
}

