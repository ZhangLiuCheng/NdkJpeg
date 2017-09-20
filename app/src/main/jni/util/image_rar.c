//
// Created by zhangliucheng on 2017/9/19.
//

#include <android/bitmap.h>
#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include "../libjpeg-turbo/jpeglib.h"
#include "../libjpeg-turbo/jversion.h"
#include "../libjpeg-turbo/cdjpeg.h"

#define IMAGE_TAG "BitmapUtil"
#define LOGV(...) __android_log_print(ANDROID_LOG_INFO, IMAGE_TAG, __VA_ARGS__)

#define true 1
#define false 0

typedef uint8_t BYTE;
char *error;

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    LOGV("jpeg_message_table[%d]:%s", myerr->pub.msg_code, myerr->pub.jpeg_message_table[myerr->pub.msg_code]);

    longjmp(myerr->setjmp_buffer, 1);
}

//图片压缩方法
int generateJPEG(BYTE* data, int w, int h, int quality, const char* outfilename, jboolean optimize) {
    int nComponent = 3;
    struct jpeg_compress_struct jcs;
    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    //为JPEG对象分配空间并初始化
    jpeg_create_compress(&jcs);
    //获取文件信息
    FILE* f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    //指定压缩数据源
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;
    jcs.image_height = h;
    jcs.arith_code = false;
    jcs.input_components = nComponent;
    jcs.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;
    //为压缩设定参数，包括图像大小，颜色空间
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩
    jpeg_start_compress(&jcs, true);

    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //写入数据
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }
    if (jcs.optimize_coding) {
        LOGV("使用了哈夫曼算法完成压缩");
    } else {
        LOGV("未使用哈夫曼算法");
    }
    //压缩完毕
    jpeg_finish_compress(&jcs);
    //释放资源
    jpeg_destroy_compress(&jcs);
    fclose(f);

    return 1;
}

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb;

//将java string转换为char*
char* jstringTostring(JNIEnv* env, jbyteArray barr) {
    char* rtn = NULL;
    jsize alen = (*env)->GetArrayLength(env, barr);
    jbyte* ba = (*env)->GetByteArrayElements(env, barr, 0);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
    return rtn;
}


JNIEXPORT jstring JNICALL Java_com_ylcq_ndkjpeg_BitmapUtil_compress
        (JNIEnv *env, jclass jclass, jobject bitmap, jint width, jint height, jint quality, jbyteArray fileName, jboolean optimize) {
    AndroidBitmapInfo infoColor;
    int ret;
    BYTE *pixelColor;
    BYTE *data;
    BYTE *tempData;
    char *filename = jstringTostring(env,fileName);

    if((ret = AndroidBitmap_getInfo(env,bitmap, &infoColor)) < 0) {\
		LOGV("解析错误");
        return (*env)->NewStringUTF(env,"0");
    }

    if((ret = AndroidBitmap_lockPixels(env,bitmap, (void **)&pixelColor)) < 0) {
        LOGV("加载失败");
        return (*env)->NewStringUTF(env,"0");
    }

    BYTE r,g,b;
    int color;
    data = malloc(width* height * 3);
    tempData = data;
    int i;
    int j;
    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            color = *((int*)pixelColor);
            b = (color >> 16) & 0xFF;
            g = (color >> 8) & 0xFF;
            r = (color >> 0) & 0xFF;
            *data = r;
            *(data + 1) = g;
            *(data + 2) = b;

            data += 3;
            pixelColor += 4;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    int resultCode = generateJPEG(tempData, width, height, quality, filename, optimize);
    free(tempData);
    if(resultCode == 0) {
        jstring result = (*env)->NewStringUTF(env,"0");
        return result;
    }
    return (*env)->NewStringUTF(env,"1");
}