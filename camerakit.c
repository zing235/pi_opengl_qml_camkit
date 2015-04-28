#include "camerakit.h"

#include <time.h>

clock_t start_time,end_time;

#define MAX_RTP_SIZE 1420
FILE *outfd = NULL;


struct cap_handle *caphandle = NULL;
struct cvt_handle *cvthandle = NULL;
struct enc_handle *enchandle = NULL;
struct cap_param capp;
struct cvt_param cvtp;
struct enc_param encp;

int stage = 0b00000011;

U32 vfmt = V4L2_PIX_FMT_YUYV;
U32 ofmt = V4L2_PIX_FMT_YUV420;


unsigned char  *cvt_buf;

init_camera()
{
    capp.dev_name = "/dev/video0";
    capp.width = 640;
    capp.height = 480;
    capp.pixfmt = vfmt;
    capp.rate = 25;

    cvtp.inwidth = 640;
    cvtp.inheight = 480;
    cvtp.inpixfmt = vfmt;
    cvtp.outwidth = 640;
    cvtp.outheight = 480;
    cvtp.outpixfmt = ofmt;

    encp.src_picwidth = 640;
    encp.src_picheight = 480;
    encp.chroma_interleave = 0;
    encp.fps = 25;
    encp.gop = 12;
    encp.bitrate = 1000;


    char *outfile = "testzz.h264";
    // options

    if (outfile)
        outfd = fopen(outfile, "wb");

    caphandle = capture_open(capp);
    if (!caphandle)
    {
        printf("--- Open capture failed\n");
    }

    if ((stage & 0b00000001) != 0)
    {
        cvthandle = convert_open(cvtp);
        if (!cvthandle)
        {
            printf("--- Open convert failed\n");
        }
    }

    if ((stage & 0b00000010) != 0)
    {
        enchandle = encode_open(encp);
        if (!enchandle)
        {
            printf("--- Open encode failed\n");
        }
    }

    // start capture encode loop

    capture_start(caphandle);		// !!! need to start capture stream!

}

void camera_cce()
{

    int ret;
    //unsigned char *cap_buf, *cvt_buf, *hd_buf, *enc_buf;
    unsigned char *cap_buf, *hd_buf, *enc_buf;
//    void *cap_buf, *cvt_buf, *hd_buf, *enc_buf;
    int cap_len, cvt_len, hd_len, enc_len;
    enum pic_t ptype;

    ret = capture_get_data(caphandle, &cap_buf, &cap_len);
    if (ret != 0)
    {
        if (ret < 0)		// error
        {
            printf("--- capture_get_data failed\n");

        }
        else	// again
        {
            usleep(10000);

        }
    }
    if (cap_len <= 0)
    {
        printf("!!! No capture data\n");

    }


    // convert


//    start_time=clock();
    ret = convert_do(cvthandle, cap_buf, cap_len, &cvt_buf, &cvt_len);
    if (ret < 0)
    {
        printf("--- convert_do failed\n");

    }
    if (cvt_len <= 0)
    {
        printf("!!! No convert data\n");

    }

//    end_time=clock();
//    printf("convert cost %f ms \n", (double)(end_time - start_time) / CLOCKS_PER_SEC*1000);


    // encode
    // fetch h264 headers first!
    while ((ret = encode_get_headers(enchandle, &hd_buf, &hd_len, &ptype))
           != 0)
    {
        if ((stage & 0b00000100) == 0)		// no pack
        {
            if (outfd)
                fwrite(hd_buf, 1, hd_len, outfd);

            continue;
        }
    }
    // do encode
    ret = encode_do(enchandle, cvt_buf, cvt_len, &enc_buf, &enc_len,
                    &ptype);
    if (ret < 0)
    {
        printf("--- encode_do failed\n");

    }
    if (enc_len <= 0)
    {
        printf("!!! No encode data\n");

    }

    if ((stage & 0b00000100) == 0)		// no pack
    {
        if (outfd)
            fwrite(enc_buf, 1, enc_len, outfd);

    }
}

camera_release()
{

    capture_stop(caphandle);

    if ((stage & 0b00000010) != 0)
        encode_close(enchandle);
    if ((stage & 0b00000001) != 0)
        convert_close(cvthandle);
    capture_close(caphandle);

    if (outfd)
        fclose(outfd);
}
