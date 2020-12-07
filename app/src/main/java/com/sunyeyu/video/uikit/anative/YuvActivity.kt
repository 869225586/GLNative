package com.sunyeyu.video.uikit.anative

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.sunyeyu.video.uikit.opengl.MyTextureView
import com.sunyeyu.video.uikit.opengl.NativeOpengl
import com.sunyeyu.video.uikit.opengl.WlSurfaceView
import java.io.File
import java.io.FileInputStream

/**
 * 驯龙高手 http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4



《紧急救援》

http://vfx.mtime.cn/Video/2019/03/21/mp4/190321153853126488.mp4



玩具总动员

http://vfx.mtime.cn/Video/2019/03/19/mp4/190319222227698228.mp4



《叶问4》先行预告甄子丹过招美

http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4



预告刘德华对决古天

http://vfx.mtime.cn/Video/2019/03/18/mp4/190318231014076505.mp4




http://vfx.mtime.cn/Video/2019/03/18/mp4/190318214226685784.mp4

http://vfx.mtime.cn/Video/2019/03/19/mp4/190319104618910544.mp4

http://vfx.mtime.cn/Video/2019/03/19/mp4/190319125415785691.mp4

http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp4

http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4

http://vfx.mtime.cn/Video/2019/03/14/mp4/190314102306987969.mp4

http://vfx.mtime.cn/Video/2019/03/13/mp4/190313094901111138.mp4

http://vfx.mtime.cn/Video/2019/03/12/mp4/190312143927981075.mp4

http://vfx.mtime.cn/Video/2019/03/12/mp4/190312083533415853.mp4

http://vfx.mtime.cn/Video/2019/03/09/mp4/190309153658147087.mp4
 */
class YuvActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_yuv)
        var wl = findViewById<WlSurfaceView>(R.id.wsurface);
        var textureview = findViewById<MyTextureView>(R.id.textureview)
        nativeOpengl = NativeOpengl()
        textureview.setNativeOpengl(nativeOpengl)
        nativeOpengl.playFromFFmpeg("http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4");
        textureview.setOnSurfaceListener {object : WlSurfaceView.OnSurfaceListener {
            override fun init() {
                Log.i("syy","prepareFrom Java")

//                 play();
            }
        }}
//        wl.setNativeOpengl(nativeOpengl);
//        wl.setOnSurfaceListener(object : WlSurfaceView.OnSurfaceListener {
//            override fun init() {
//                nativeOpengl.playFromFFmpeg("http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4");
////                 play();
//            }
//        });

    }

    lateinit var fis: FileInputStream;
    lateinit var nativeOpengl: NativeOpengl
    var isexit = false

    fun play() {
        Thread(Runnable {
            val w = 640
            val h = 360
            try {
                Log.i("syy", "解析yuv");
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
                    Log.i("syy", "解析yuv1");
                    if (ysize > 0 && usize > 0 && vsize > 0) {
                        Log.i("syy", "解析yuv2");
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