package com.kwai.video.uikit.anative

import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.kwai.video.uikit.opengl.NativeOpengl
import com.kwai.video.uikit.opengl.WlSurfaceView
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.nio.ByteBuffer


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        var wl = findViewById<WlSurfaceView>(R.id.wsurface);
        val nativeOpengl = NativeOpengl();
        wl.setNativeOpengl(nativeOpengl);
        wl.setOnSurfaceListener(object : WlSurfaceView.OnSurfaceListener {
            override fun init() {
                val bitmap = BitmapFactory.decodeResource(
                    resources,
                    R.drawable.mingren
                )
                val fcbuffer: ByteBuffer = ByteBuffer.allocate(bitmap.height * bitmap.width * 4)
                bitmap.copyPixelsToBuffer(fcbuffer)
                fcbuffer.flip()
                val pixels: ByteArray = fcbuffer.array()
                nativeOpengl.imgData(bitmap.width, bitmap.height, pixels.size, pixels)
            }
        });

    }

    private fun testLaunch() {
        var job = GlobalScope.launch {
            delay(600)
            var result1 = GlobalScope.async {
                getResult1()
            }
            var result2 = GlobalScope.async {
                getResult2()
            }
            var result = result1.await() + result2.await();
            Log.e("syy", "协程+$result");
        }
    }

    private suspend fun getResult1(): Int {
        delay(3000)
        return 1
    }

    private suspend fun getResult2(): Int {
        delay(4000)
        return 2
    }

    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
