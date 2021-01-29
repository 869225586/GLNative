package com.sunyeyu.video.uikit.anative

import android.content.res.Configuration
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.LinearLayout
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
    var isClick = true
    lateinit var fis: FileInputStream;
    lateinit var nativeOpengl: NativeOpengl
    lateinit var ll_window:LinearLayout //窗口容器
    lateinit var ll_full_screen:LinearLayout //全屏容器
    lateinit var myTextureView: MyTextureView
    var isexit = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_yuv)
        ll_window =findViewById(R.id.ll_window)
        ll_full_screen = findViewById(R.id.ll_full_screen)
        myTextureView = MyTextureView(this);
        var lp = LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT)
        ll_window.addView(myTextureView,lp)
        nativeOpengl = NativeOpengl()
        myTextureView.setNativeOpengl(nativeOpengl)
        nativeOpengl.playFromFFmpeg("http://vfx.mtime.cn/Video/2019/03/09/mp4/190309153658147087.mp4");
        myTextureView.setOnSurfaceListener {
            WlSurfaceView.OnSurfaceListener {
                Log.i(
                    "syy",
                    "prepareFrom Java"
                )
            }
        }
        myTextureView.setOnClickListener {
            if (isClick) {
                isClick = false;
                nativeOpengl.pause()
            } else {
                isClick = true;
                nativeOpengl.resume()
            }
        }
    }


    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        }

    override fun onDestroy() {
        super.onDestroy()
//        nativeOpengl.surfaceDestroy();

    }
    /**
     * 解析本地yuv 视频
     */
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

    fun fullScreen(view: View) {
        ll_window.removeView(myTextureView)
        myTextureView.startFullScreen()
        var lp = LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT)
        ll_full_screen.addView(myTextureView,lp)
    }
}