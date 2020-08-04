package com.kwai.video.uikit.anative

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.Settings
import android.util.Log
import android.widget.ImageView
import android.widget.Toast
import com.kwai.video.uikit.opengl.NativeOpengl
import com.kwai.video.uikit.opengl.WlSurfaceView
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        var wl=findViewById<WlSurfaceView>(R.id.wsurface);
        wl.setNativeOpengl(NativeOpengl());
        // Example of a call to a native method

        var job=GlobalScope.launch {
            delay(600)
            var result1 = GlobalScope.async {
                    getResult1()
            }
            var result2= GlobalScope.async {
                    getResult2()
            }
            var result= result1.await() + result2.await();
            Log.e("syy","协程+$result");
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
    fun  aw() {

    }
    external fun sum():Int;
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
