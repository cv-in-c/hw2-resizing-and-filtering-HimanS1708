#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    long long sum = 0;
    for(int i=0;i<im.w*im.h*im.c;i++)
        sum+=im.data[i];

    for(int i=0;i<im.w*im.h*im.c;i++)
        im.data[i]/=sum;
}

image make_box_filter(int w)
{
    image box=make_image(w,w,1);
    for(int i=0;i<w*w;i++)
        box.data[i]=1;
    l1_normalize(box);
    return box;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(im.c==filter.c || filter.c==1);

    image im1;

    if(preserve==1){
        im1=make_image(im.w,im.h,im.c);
    }
    else{
        im1=make_image(im.w,im.h,1);
    }

    for (int j = 0; j < im.h; j++)
    {
        for(int i = 0; i < im.w; i++)
        {
            int shift = filter.w / 2;

            for (int channel = 0; channel < im.c; ++channel)
            {
                float sum = 0;
                for (int f_row = 0; f_row < filter.w; ++f_row)
                {
                    for (int f_col = 0; f_col < filter.w; ++f_col)
                    {
                        sum += get_pixel(filter, f_col, f_row, 0) *
                               get_pixel(im, i + f_col - shift, j + f_row - shift, channel);
                    }
                }

                if (preserve)
                {
                    set_pixel(im1, i, j, channel, sum);
                }
                else
                {
                    float current_value = get_pixel(im1, i, j, 0);
                    set_pixel(im1, i, j, 0, current_value + sum);
                }
            }
        }
    }
    return im1;
}

image make_highpass_filter()
{
    image filter=make_image(3,3,1);
    filter.data[0]=filter.data[2]=filter.data[6]=filter.data[8]=0;
    filter.data[1]=filter.data[3]=filter.data[5]=filter.data[7]=-1;
    filter.data[4]=4;

    return filter;
}

image make_sharpen_filter()
{
    image filter=make_image(3,3,1);
    filter.data[0]=filter.data[2]=filter.data[6]=filter.data[8]=0;
    filter.data[1]=filter.data[3]=filter.data[5]=filter.data[7]=-1;
    filter.data[4]=5;

    return filter;
}

image make_emboss_filter()
{
    image filter=make_image(3,3,1);
    filter.data[0]=-2;
    filter.data[1]=-1;
    filter.data[2]=0;
    filter.data[3]=-1;
    filter.data[4]=1;
    filter.data[5]=1;
    filter.data[6]=0;
    filter.data[7]=1;
    filter.data[8]=2;

    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: For sharpen and emboss filter we should use preserve because they return a rgb image while we should not use preserve for highpass filter as it returns a grayscale image.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: We can use post-processing for highpass filter, i.e. we can smoothen the image first to get better edge detection.

image make_gaussian_filter(float sigma)
{
    int w=ceil(6*sigma);
    if(w%2==0) w++;

    int dx=w/2;

    image filter=make_image(w,w,1);
    for(int i=0;i<w;i++){
        for(int j=0;j<w;j++){
            filter.data[j+i*w]=(1.0/(TWOPI*sigma*sigma))*exp(-1*((j-dx)*(j-dx)+(i-dx)*(i-dx))/(2*sigma*sigma));
        }
    }

    return filter;
}

image add_image(image a, image b)
{
    assert(a.w==b.w && a.h==b.h && a.c==b.c);

    image im=make_image(a.w,a.h,a.c);
    for(int i=0;i<a.w*a.h*a.c;i++){
        im.data[i]=a.data[i]+b.data[i];
    }

    return im;
}

image sub_image(image a, image b)
{
    assert(a.w==b.w && a.h==b.h && a.c==b.c);

    image im=make_image(a.w,a.h,a.c);
    for(int i=0;i<a.w*a.h*a.c;i++){
        im.data[i]=a.data[i]-b.data[i];
    }

    return im;
}

image make_gx_filter()
{
    image filter=make_image(3,3,1);
    filter.data[0]=-1;
    filter.data[1]=0;
    filter.data[2]=1;
    filter.data[3]=-2;
    filter.data[4]=0;
    filter.data[5]=2;
    filter.data[6]=-1;
    filter.data[7]=0;
    filter.data[8]=1;

    return filter;
}

image make_gy_filter()
{
    image filter=make_image(3,3,1);
    filter.data[0]=-1;
    filter.data[1]=-2;
    filter.data[2]=-1;
    filter.data[3]=0;
    filter.data[4]=0;
    filter.data[5]=0;
    filter.data[6]=1;
    filter.data[7]=2;
    filter.data[8]=1;

    return filter;
}

void feature_normalize(image im)
{
    float max=0.0,min=500.0,r=0.0;
    for(int i=0;i<im.w*im.h*im.c;i++){
        if(max<im.data[i])
            max=im.data[i];
        if(min<im.data[i])
            min=im.data[i];
    }
    r=max-min;
    if(!r){
        for(int i=0;i<im.w*im.h*im.c;i++){
            im.data[i]=0;
        }
    }
    else{
        for(int i=0;i<im.w*im.h*im.c;i++){
            im.data[i]=(im.data[i]-min)/r;
        }
    }
}

image *sobel_image(image im)
{
    image *im_new=(image*)malloc(2*sizeof(image));
    im_new[0]=make_image(im.w,im.h,im.c);
    im_new[1]=make_image(im.w,im.h,im.c);
    image gx=make_gx_filter();
    image gy=make_gy_filter();

    image im1=convolve_image(im,gx,1);
    image im2=convolve_image(im,gy,1);

    for(int i=0;i<im.w*im.h*im.c;i++){
        im_new[0].data[i]=pow(pow(im1.data[i],2)+pow(im2.data[i],2),0.5);
        im_new[1].data[i]=atan2(im2.data[i],im1.data[i]);
    }

    return im_new;
}

image colorize_sobel(image im)
{
    // image im1=make_image(im.w,im.h,im.c);
    // image im2=make_image(im.w,im.h,im.c);
    // image im_m=make_image(im.w,im.h,im.c);
    // image im_g=make_image(im.w,im.h,im.c);
    // image *im_t;

    // im_t=sobel_image(im);
    // im_m=im_t[0];
    // im_g=im_t[1];

    // im1=copy_image(im);
    // im2=copy_image(im);

    // rgb_to_hsv(im2);

    // for(int i=0;i<im.h;i++){
    //     for(int j=0;j<im.w;j++){
    //         im2.data
    //     }
    // }
    return make_image(1,1,1);
}
