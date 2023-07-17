#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if(x>=im.w)
        x=im.w-1;
    else if(x<0)
        x=0;
    if(y>=im.h)
        y=im.h-1;
    else if(y<0)
        y=0;
    float pixel=im.data[x+y*im.w+c*im.w*im.h];
    return pixel;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    im.data[x+y*im.w+c*im.w*im.h]=v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data,im.data,sizeof(int)*im.w*im.h*im.c);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            float r=im.data[i*im.w+j];
            float g=im.data[i*im.w+j+im.w*im.h];
            float b=im.data[i*im.w+j+2*im.w*im.h];
            float y=0.299*r+0.587*g+0.114*b;
            gray.data[i*im.w+j]=y;
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            im.data[j+i*im.w+c*im.w*im.h]+=v;
        }
    }
}

void clamp_image(image im)
{
    for(int i=0;i<im.c;i++){
        for(int j=0;j<im.h;j++){
            for(int k=0;k<im.w;k++){
                if(im.data[i*im.w*im.h+j*im.w+k]>1)
                    im.data[i*im.w*im.h+j*im.w+k]=1;
                else if(im.data[i*im.w*im.h+j*im.w+k]<0)
                    im.data[i*im.w*im.h+j*im.w+k]=0;
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            float r=im.data[j+i*im.w];
            float g=im.data[j+i*im.w+im.w*im.h];
            float b=im.data[j+i*im.w+2*im.w*im.h];
            
            float V=three_way_max(r,g,b);

            float m=three_way_min(r,g,b);
            float C=V-m;
            float S;
            if(V==0)
                S=0;
            else
                S=C/V;
            
            float H_=0,H;
            if(C==0)
                H_=0;
            else if(V==r)
                H_=(g-b)/C;
            else if(V==g)
                H_=(b-r)/C+2;
            else if(V==b)
                H_=(r-g)/C+4;
            
            if(H_<0)
                H=H_/6+1;
            else
                H=H_/6;

            if(H>1)
                H-=1;
            else if(H<0)
                H+=1;

            im.data[j+i*im.w]=H;
            im.data[j+i*im.w+im.w*im.h]=S;
            im.data[j+i*im.w+2*im.w*im.h]=V;
        }
    }
}

void hsv_to_rgb(image im)
{
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            float H=im.data[j+i*im.w];
            float S=im.data[j+i*im.w+im.w*im.h];
            float V=im.data[j+i*im.w+2*im.w*im.h];

            float r=0,g=0,b=0;
            float k1,k2,k3;
            k1=((int)(5+6*H))%6 + ((5+6*H)-(int)(5+6*H));
            k2=((int)(3+6*H))%6 + ((3+6*H)-(int)(3+6*H));
            k3=((int)(1+6*H))%6 + ((1+6*H)-(int)(1+6*H));

            float f1,f2,f3;
            f1=V-V*S*three_way_max(0,0,three_way_min(k1,4-k1,1));
            f2=V-V*S*three_way_max(0,0,three_way_min(k2,4-k2,1));
            f3=V-V*S*three_way_max(0,0,three_way_min(k3,4-k3,1));

            r=f1;
            g=f2;
            b=f3;

            im.data[j+i*im.w]=r;
            im.data[j+i*im.w+im.w*im.h]=g;
            im.data[j+i*im.w+2*im.w*im.h]=b;
        }
    }
}

void scale_image(image im, int c, float v){
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            im.data[j+i*im.w+c*im.w*im.h]*=v;
        }
    }
}

float ge(float c){
    float EXP=0.05,ans=0;
    if(c-EXP<10 && 10<c+EXP){
        ans=c/3294.6;
    }
    else{
        ans=pow((c+14.025)/269.025,2.4);
    }
    return ans;
}

void rgb_to_hcl(image im){
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            float r=0,g=0,b=0;
            r=im.data[j+i*im.w];
            g=im.data[j+i*im.w+im.w*im.h];
            b=im.data[j+i*im.w+2*im.w*im.h];

            float R=0,G=0,B=0;
            R=ge(r);
            G=ge(g);
            B=ge(b);

            float X=0,Y=0,Z=0;
            X=R*0.4124108464885388   + G*0.3575845678529519  + B*0.18045380393360833;
            Y=R*0.21264934272065283  + G*0.7151691357059038  + B*0.07218152157344333;
            Z=R*0.019331758429150258 + G*0.11919485595098397 + B*0.9503900340503373;

            float D=X+15*Y+3*Z;
            float u1=4*X/D;
            float v1=9*Y/D;

            float uw=0.1978330369967827,vw=0.4683304743525223;

            float EXP=0.05;
            float L_=0,u_=0,v_=0;
            if(24389*Y-EXP<=216 && 216<=24389*Y+EXP){
                L_=24389*Y/27;
            }
            else{
                L_=116*cbrt(Y)-16;
            }
            u_=13*L_*(u1-uw);
            v_=13*L_*(v1-vw);

            im.data[j+i*im.w]=L_;
            im.data[j+i*im.w+im.w*im.h]=u_;
            im.data[j+i*im.w+2*im.w*im.h]=v_;
        }
    }
}
