#include <stdio.h>

#include <string.h>
#include <stdlib.h>
//#include <LIMITS.H>

//src_fileName out_fileName src_width src_height frameNo
int main(int argc, char *argv[])
{
    char src_fileName[255];
    char out_fileName[255];

    int src_width, src_height;
    src_width = src_height = 0;
    int frameNum = 65535;
    int src_size, out_size, tem_size;
    src_size = out_size = tem_size = 0;

    unsigned char *src_buf, *out_buf, *tem_buf;

    FILE *in_file, *out_file;

    unsigned char *Y, *U, *V;
    unsigned char *Y2, *U2, *V2;

    unsigned char *p = NULL;

    if (argc > 5) //判断输入的参数是否为5个。
    {
        strcpy(src_fileName, argv[1]);
        strcpy(out_fileName, argv[2]);
        src_width = atoi(argv[3]);
        src_height = atoi(argv[4]);
        frameNum = atoi(argv[5]);
    }
    else
    {
        strcpy(src_fileName, argv[1]);
        strcpy(out_fileName, argv[2]);
        src_width = atoi(argv[3]);
        src_height = atoi(argv[4]);
    }
    //源图片,格式为YUY2，大小src_width *src_height * 2 src_buf = (unsigned char *)malloc(src_size * sizeof(char));
    src_size = (src_width * src_height) << 1; 
    memset(src_buf, 0, src_size);

    tem_size = (src_width * src_height) << 1;
    //此处开辟一块与源图大小相同的一块区域。本文中首先将packed形式的YUY2格式数据，转变
    //为planar形式的术据，并将这些数据存放在此处开辟的内存中�总体而言，此处区域起临时存储作用。
    //为后序构建YUV420数据服务。
    tem_buf = (unsigned char *)malloc(tem_size * sizeof(char));
    memset(tem_buf, 0, tem_size);

    out_size = src_width * src_height * 1.5; //开辟src_width*src_height * 1.5大小区域，用于保存YUV420数据。 out_buf = (unsigned char *)malloc(out_size * sizeof(char));
    memset(out_buf, 0, out_size);

    in_file = fopen(src_fileName, "rb");
    if (!in_file)
    {
        printf("cannot open input file.");
        return 0;
    }

    out_file = fopen(out_fileName, "wb");
    if (!out_file)
    {
        printf("cannot write 264 file./n");
        return 0;
    }

    while (frameNum > 0 && !feof(in_file))
    {
        //读出一帧数据
        if (fread(src_buf, src_size, 1, in_file) <= 0)
            printf("cannot read from input file.");
        p = src_buf; 
        //源图数据首地址

        //将临时存储区域中Y、U、V各个分量的首地址确定。
        Y = tem_buf;
        U = Y + src_width * src_height;
        V = U + (src_width * src_height >> 1); //Y  U  V  =4 : 2 ; 2
        //将YUV420存储区域中Y、U、V各个分量的首地址确定。
        Y2 = out_buf;
        U2 = Y2 + src_width * src_height; //长与宽反映象素的各数，每个像素中都一定包含Y分量，所以要预留长 *宽的空间给Y分量存储。 V2 = U2 + (src_width * src_height >> 2);
        /*由打包YUYV变成平板YUV*/
        int k, j;
        for (k = 0; k < src_height; ++k)
        {
            for (j = 0; j < (src_width >> 1); ++j)
            {
                Y[j * 2] = p[4 * j];
                U[j] = p[4 * j + 1];
                Y[j * 2 + 1] = p[4 * j + 2];
                V[j] = p[4 * j + 3];
            }
            p = p + src_width * 2;

            Y = Y + src_width;
            U = U + (src_width >> 1);
            V = V + (src_width >> 1);
        }
        // packed格式 源图（以6*8图像为例）
        //经过for循环后变为planar形式
        //复位
        Y = tem_buf;
        U = Y + src_width * src_height;
        V = U + (src_width * src_height >> 1);

        int l;
        for (l = 0; l < src_height / 2; ++l)
        {
            memcpy(U2, U, src_width >> 1);
            memcpy(V2, V, src_width >> 1);

            U2 = U2 + (src_width >> 1);
            V2 = V2 + (src_width >> 1);

            U = U + (src_width);
            V = V + (src_width);
        }

        memcpy(Y2, Y, src_width * src_height);

        //最终得到：

        fwrite(out_buf, sizeof(char), out_size, out_file);
        printf(".");

        frameNum--;
    }

    fflush(out_file);

    free(src_buf);
    src_buf = NULL;
    free(tem_buf);
    tem_buf = NULL;
    free(out_buf);
    out_buf = NULL;

    fclose(in_file);
    fclose(out_file);

    return 0;
}