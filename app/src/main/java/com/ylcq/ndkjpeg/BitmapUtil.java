package com.ylcq.ndkjpeg;

import android.graphics.Bitmap;

public class BitmapUtil {

    static {
        System.loadLibrary("jpeg");
        System.loadLibrary("bitmap_util");
    }

    public native static String compress(Bitmap bitmap, int width, int height, int quality, byte[] filename, boolean optimize);
}
