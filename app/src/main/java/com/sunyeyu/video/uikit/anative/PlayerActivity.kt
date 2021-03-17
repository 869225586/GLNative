package com.sunyeyu.video.uikit.anative

import android.content.res.Configuration
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.View
import android.widget.LinearLayout
import android.widget.RelativeLayout
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.os.postDelayed
import com.sunyeyu.video.uikit.opengl.MyTextureView
import com.sunyeyu.video.uikit.opengl.NativeOpengl
import com.sunyeyu.video.uikit.opengl.WlSurfaceView
import com.sunyeyu.video.uikit.util.LogUtils
import com.sunyeyu.video.uikit.util.TimeUtil
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
class PlayerActivity : AppCompatActivity() {
    var isClick = true
    lateinit var fis: FileInputStream;
    lateinit var nativeOpengl: NativeOpengl
    lateinit var ll_window:LinearLayout //窗口容器
    lateinit var ll_full_screen:LinearLayout //全屏容器
    lateinit var myTextureView: MyTextureView
    lateinit var tv_time :TextView;
    lateinit var seekbar:SeekBar;
    lateinit var mTvMin : TextView
    var isexit = false
    private var handler :Handler = object : Handler(){

    }
    public var runnable :Runnable = object :Runnable {
        override fun run() {
            var totalduration = nativeOpengl.duration
            var currentpos = nativeOpengl.currentPos
            tv_time.text=TimeUtil.getMinute(currentpos.toLong())+"/"+TimeUtil.getMinute(totalduration)
            seekbar.progress = (currentpos/(totalduration*1.0f/100)).toInt()
            handler.postDelayed(this,1000)
        }

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_yuv)
        initView()
        initPlayer()
    }

    internal fun initPlayer() {
        nativeOpengl = NativeOpengl()
        myTextureView.setNativeOpengl(nativeOpengl)

        nativeOpengl.playFromFFmpeg("http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp4");
        nativeOpengl.setPreparedListner {
            Log.i("player", "duration" + nativeOpengl.duration)
            runOnUiThread {

                tv_time.text = TimeUtil.getMinute(nativeOpengl.duration)
            }
        }
        handler.postDelayed(
            runnable
        ,1000)

    }

    private fun initView() {
        ll_window = findViewById(R.id.ll_window)
        ll_full_screen = findViewById(R.id.ll_full_screen)
        tv_time = findViewById(R.id.duration)
        seekbar = findViewById(R.id.seekbar)
        mTvMin  = findViewById(R.id.tv_mini)

        myTextureView = MyTextureView(this);
        var lp = LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.MATCH_PARENT
        )
        ll_window.addView(myTextureView, lp)
        mTvMin.setOnClickListener{
              mini()
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
        seekbar.setOnSeekBarChangeListener(object: OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                var progress = seekbar.progress;
                var mills = 100*1.0f/nativeOpengl.duration*progress;
                nativeOpengl.seek(mills.toLong())
            }

        })
    }
    override fun onDestroy() {
        super.onDestroy()
        nativeOpengl.surfaceDestroy();

    }

    fun fullScreen(view: View) {
        ll_window.layoutParams.height = RelativeLayout.LayoutParams.MATCH_PARENT;
        myTextureView.startFullScreen();
//        ll_window.removeView(myTextureView)
//        myTextureView.startFullScreen()
//        var lp = LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT)
//        ll_full_screen.addView(myTextureView,lp)
    }

    fun mini(){
        ll_window.layoutParams.height = 300
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

}