package com.ylcq.ndkjpeg;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void compress(View view) {
        ImageView imageView = (ImageView) findViewById(R.id.bitmap);
        try {
            InputStream is = getResources().getAssets().open("e.png");
            Bitmap bitmap = BitmapFactory.decodeStream(is);
//            File srcFile = new File("/storage/emulated/0/Android/data/com.ylcq.ndkjpeg/cache", "a.png");
//            Bitmap bitmap = BitmapFactory.decodeFile(srcFile.getAbsolutePath());
            Log.v("BitmapUtil", "bitmap  " + bitmap);

            Log.v("BitmapUtil", "Environment.getExternalStorageDirectory():   " + Environment.getExternalStorageDirectory().getAbsolutePath());
            Log.v("BitmapUtil", "Environment.getDownloadCacheDirectory():   " + Environment.getDownloadCacheDirectory().getAbsolutePath());
            Log.v("BitmapUtil", "getExternalCacheDir():   " + getExternalCacheDir().getAbsolutePath());

            File dest = new File("/sdcard/aaaaa", "a1.png");
            String result = BitmapUtil.compress(bitmap, bitmap.getWidth(), bitmap.getHeight(), 10, dest.getAbsolutePath().getBytes(), true);
            Log.v("BitmapUtil", "压缩完成  " + result);
            imageView.setImageBitmap(BitmapFactory.decodeFile(dest.getAbsolutePath()));
        } catch (Exception e) {
            e.printStackTrace();
            Log.v("BitmapUtil", e.toString());
        }
    }
}
