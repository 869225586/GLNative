package com.kwai.video.uikit.anative

import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.kwai.video.uikit.opengl.NativeOpengl
import com.kwai.video.uikit.opengl.WlSurfaceView
import java.io.File
import java.io.FileInputStream

class YuvActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_yuv)
        var wl = findViewById<WlSurfaceView>(R.id.wsurface);
        nativeOpengl = NativeOpengl()
        wl.setNativeOpengl(nativeOpengl);
        wl.setOnSurfaceListener(object : WlSurfaceView.OnSurfaceListener {
            override fun init() {
               play();
            }
        });

    }

    lateinit var fis: FileInputStream;
    lateinit var nativeOpengl: NativeOpengl
    var   isexit = false

    fun play() {
            Thread(Runnable {
                val w = 640
                val h = 360
                try {
                    Log.i("syy","解析yuv");
                    fis = FileInputStream(File("/sdcard/Download/sintel_640_360.yuv"))
                    val y = ByteArray(w * h)
                    val u = ByteArray(w * h / 4)
                    val v = ByteArray(w * h / 4)
                    while (true) {
                        if (isexit) {
                            break
                        }
                        val ysize: Int = fis.read(y)
                        val usize: Int = fis.read(u)
                        val vsize: Int = fis.read(v)
                        Log.i("syy","解析yuv1");
                        if (ysize > 0 && usize > 0 && vsize > 0) {
                            Log.i("syy","解析yuv2");
                            nativeOpengl.setYuvData(y, u, v, w, h)
                            Thread.sleep(40)
                        } else {
                            isexit = true
                        }
                    }
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }).start()
    }

}