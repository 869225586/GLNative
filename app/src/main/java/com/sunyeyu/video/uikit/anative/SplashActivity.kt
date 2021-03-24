package com.sunyeyu.video.uikit.anative

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler

class SplashActivity : AppCompatActivity() {
    private var handler :Handler = object : Handler(){

    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)
        handler.postDelayed(Runnable {
            startActivity(Intent(this@SplashActivity,PlayerActivity::class.java))
            this@SplashActivity.finish()
                                     },1000)

    }
}