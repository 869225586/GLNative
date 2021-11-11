package com.sunyeyu.video.uikit.anative

import android.content.Context
import android.content.pm.ActivityInfo
import android.content.res.Configuration
import android.graphics.Color
import android.media.AudioManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.widget.ImageView
import android.widget.RelativeLayout
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import android.widget.TextView
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.blankj.utilcode.util.BrightnessUtils
import com.blankj.utilcode.util.ScreenUtils
import com.sunyeyu.video.uikit.opengl.MyTextureView
import com.sunyeyu.video.uikit.opengl.NativeOpengl
import com.sunyeyu.video.uikit.util.MyClickListener
import com.sunyeyu.video.uikit.util.MyClickListener.MyClickCallBack
import com.sunyeyu.video.uikit.widget.CustomSeekBar
import com.syy.video.danmu.widget.BrrageTextureView
import kotlinx.android.synthetic.main.layout_play.*
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

https://v2.dious.cc/20201022/0FQ3EJxy/index.m3u8
 */
class PlayerActivity : AppCompatActivity() {
    var isClick = true
    lateinit var fis: FileInputStream;
    lateinit var nativeOpengl: NativeOpengl
    lateinit var ll_window: RelativeLayout //窗口容器
    lateinit var rl_player: RelativeLayout
    lateinit var myTextureView: MyTextureView
    lateinit var  brrageView :BrrageTextureView
    lateinit var seekbar: CustomSeekBar;
    lateinit var iv_pause: ImageView
    lateinit var toolbar:View
    var isexit = false
    private var startX = 0;
    private var startY = 0;
    private var handler: Handler = object : Handler() {

    }
    var thread: Thread = object : Thread() {
        override fun run() {
            nativeOpengl.start()
        }
    }
    public var runnable: Runnable = object : Runnable {
        override fun run() {
            var totalduration = nativeOpengl.duration
            var currentpos = nativeOpengl.currentPos
            seekbar.setTvleft(currentpos.toLong())
            seekbar.setTvRight(totalduration)
            seekbar.progress = (currentpos / (totalduration * 1.0f / 100)).toInt()
            handler.postDelayed(this, 1000)
        }

    }
    private val menuRunnable = Runnable { rl_player.visibility = View.GONE }
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    override fun onCreate(savedInstanceState: Bundle?) {
        if (Build.VERSION_CODES.KITKAT <= Build.VERSION.SDK_INT) {
            window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_LAYOUT_STABLE)
            window.statusBarColor = Color.TRANSPARENT
            window.navigationBarColor = Color.TRANSPARENT
        } else {
            window.statusBarColor = Color.TRANSPARENT
            window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
        }
        super.onCreate(savedInstanceState)
        setContentView(R.layout.layout_play)
        initView()
        initPlayer()
        startBrrage()//开启弹幕
    }

    private var currentSystemMills: Long = 0
    var count = 0
    var isRun = true
    private fun startBrrage() {
        object : Thread() {
            override fun run() {
                while (isRun) {
                    if (brrageView.controller == null) continue
                    if (currentSystemMills == 0L) {
                        currentSystemMills = System.currentTimeMillis()
                        handler.post(Runnable {
                            addBrrage(
                                "你好",
                                "今天是什么新闻",
                                "https://ss0.bdstatic.com/70cFuHSh_Q1YnxGkpoWK1HF6hhy/it/u=3512331237,2033775251&fm=27&gp=0.jpg"
                            )
                        })
                        count++
                    } else {
                        if (System.currentTimeMillis() - currentSystemMills > 500 && count <= 100) {
                            handler.post(Runnable {
                                addBrrage(
                                    "哈哈",
                                    "主持人很好看啊",
                                    "https://ss0.bdstatic.com/70cFuHSh_Q1YnxGkpoWK1HF6hhy/it/u=3512331237,2033775251&fm=27&gp=0.jpg"
                                )
                            })
                            currentSystemMills = System.currentTimeMillis()
                            count++
                        } else {
                            if (count > 100) {
                                isRun = false
                            }
                        }
                    }
                }
            }
        }
            .start()
    }
    private fun addBrrage(name: String, msg: String, imgUrl: String) {
        val templateView: View = LayoutInflater.from(this).inflate(R.layout.barrage, null)
        var tvBarrageName = templateView.findViewById<TextView>(R.id.tvBarrageName);
        var tvBarrageMsg = templateView.findViewById<TextView>(R.id.tvBarrageMsg);
        tvBarrageName.text = name
        tvBarrageMsg.text = msg
        brrageView.controller.add(templateView)
    }

    internal fun initPlayer() {
        nativeOpengl = NativeOpengl()
        myTextureView.setNativeOpengl(nativeOpengl)
        nativeOpengl.playFromFFmpeg("")
        //直接通过 ip 地址 播放 可减少起播时间
        nativeOpengl.setUrl("http://stream.iqilu.com/vod_bag_2016//2020/02/16/903BE158056C44fcA9524B118A5BF230/903BE158056C44fcA9524B118A5BF230_H264_mp4_500K.mp4")
        nativeOpengl.setPreparedListner {
            thread.start();
            handler.post(
                runnable
            )
        }

    }

    private fun initView() {
        ll_window = findViewById(R.id.ll_window)
        seekbar = findViewById(R.id.seekbar)
        rl_player = findViewById(R.id.rl_player)
        brrageView = findViewById(R.id.dmView)
        myTextureView = MyTextureView(this);
        toolbar = findViewById(R.id.toobar)
        var lp = RelativeLayout.LayoutParams(
            RelativeLayout.LayoutParams.MATCH_PARENT,
            RelativeLayout.LayoutParams.MATCH_PARENT
        )

        ll_window.addView(myTextureView,0, lp)
        myTextureView.setOnTouchListener(MyClickListener(object : MyClickCallBack {
            override fun oneClick() {
                if (resources.configuration.orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) {
                    doubleClick()
                } else {
                    if (rl_player.visibility == View.VISIBLE) {
                        rl_player.visibility = View.GONE
                        setStatusBarVisible2(false);
                    } else {
                        rl_player.visibility = View.VISIBLE
                        setStatusBarVisible2(true)
                    }
                }
            }

            override fun doubleClick() {
                try {
                    if (iv_pause.getVisibility() === View.GONE) {
                        iv_pause.setVisibility(View.VISIBLE)
                        nativeOpengl.pause()
                    } else {
                        nativeOpengl.start()
                        iv_pause.setVisibility(View.GONE)
                    }
                } catch (e: java.lang.Exception) {
                }
            }
        }))
        seekbar.setSeekbarChange(object : OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
               removeMenuTask()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                var progress = seekbar.progress;
                var mills = (100 * 1.0f) / nativeOpengl.duration * progress;
                Log.i("native_E", "seek${nativeOpengl.duration * progress / 100}");
                nativeOpengl.seek((nativeOpengl.duration * progress / 100).toInt())
                startMenuTask()
            }
        })
    }
    
    override fun onDestroy() {
        super.onDestroy()

    }

    private fun startMenuTask() {
        if (resources.configuration.orientation == ActivityInfo.SCREEN_ORIENTATION_USER) {
            handler.postDelayed(menuRunnable, 5000)
        }
    }

    private fun removeMenuTask() {
        handler.removeCallbacks(menuRunnable)
    }

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        if (newConfig.orientation === ActivityInfo.SCREEN_ORIENTATION_USER) {
            startMenuTask()
            toolbar.visibility = View.GONE
//            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
        } else {
            toolbar.visibility = View.VISIBLE
            removeMenuTask()
        }

    }

    /**
     * 动态显示和隐藏状态栏
     */
    private fun setStatusBarVisible2(show: Boolean) {
        if (show) {
            var uiFlags = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            uiFlags = uiFlags or 0x00001000
            window.decorView.systemUiVisibility = uiFlags
        } else {
            var uiFlags = (View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_FULLSCREEN)
            uiFlags = uiFlags or 0x00001000
            window.decorView.systemUiVisibility = uiFlags
        }
    }
    /**
     * 开启全屏播放
     */
    fun fullScreen(view: View) {
        if (resources.configuration.orientation == ActivityInfo.SCREEN_ORIENTATION_USER) {
            ll_window.layoutParams.height = 900
            requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
        } else {
            requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
            ll_window.layoutParams.height = RelativeLayout.LayoutParams.MATCH_PARENT;
        }
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

    fun play(view: View) {
        thread.start()
    }

    fun stop(view: View) {
        handler.removeCallbacksAndMessages(null)
        nativeOpengl.surfaceDestroy()
    }

    @RequiresApi(Build.VERSION_CODES.M)
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        if (resources.configuration.orientation == ActivityInfo.SCREEN_ORIENTATION_USER) {
            val screenWidth: Int = ScreenUtils.getScreenWidth()
            when (event!!.action) {
                MotionEvent.ACTION_DOWN -> {
                    removeMenuTask()
                    startX = event!!.x.toInt()
                    startY = event!!.y.toInt()
                }
                MotionEvent.ACTION_MOVE -> {
                    val endY = event!!.y
                    val distanceY = startY - endY
                    if (startX > screenWidth / 2) {
                        // 右边
                        // 在这里处理音量
                        val FLING_MIN_DISTANCE = 30.0
                        val FLING_MIN_VELOCITY = 30.0
                        val am = getSystemService(Context.AUDIO_SERVICE) as AudioManager
                        if (distanceY > FLING_MIN_DISTANCE && Math.abs(distanceY) > FLING_MIN_VELOCITY) {
                            startY = endY.toInt()
                            am.adjustStreamVolume(
                                AudioManager.STREAM_MUSIC,
                                AudioManager.ADJUST_RAISE,
                                AudioManager.FLAG_SHOW_UI
                            )
                        }
                        if (distanceY < FLING_MIN_DISTANCE && Math.abs(distanceY) > FLING_MIN_VELOCITY) {
                            startY = endY.toInt()
                            am.adjustStreamVolume(
                                AudioManager.STREAM_MUSIC,
                                AudioManager.ADJUST_LOWER,
                                AudioManager.FLAG_SHOW_UI
                            )
                        }
                    } else {
                        // 屏幕左半部分上滑，亮度变大，下滑，亮度变小
                        val FLING_MIN_DISTANCE = 30
                        val FLING_MIN_VELOCITY = 30
                        if (distanceY > FLING_MIN_DISTANCE && Math.abs(distanceY) > FLING_MIN_VELOCITY) {
                            var brightness: Int = BrightnessUtils.getWindowBrightness(window)
                            brightness += 3
                            if (brightness < 255) {
                                BrightnessUtils.setWindowBrightness(window, brightness)
                            }
                        }
                        if (distanceY < FLING_MIN_DISTANCE && Math.abs(distanceY) > FLING_MIN_VELOCITY) {
                            var brightness: Int = BrightnessUtils.getWindowBrightness(window)
                            brightness -= 3
                            if (brightness > 0) {
                                BrightnessUtils.setWindowBrightness(window, --brightness)
                            }
                        }
                    }
                }
                MotionEvent.ACTION_UP -> {
                    startMenuTask()
                }
            }
        }
        return super.onTouchEvent(event)
    }
}