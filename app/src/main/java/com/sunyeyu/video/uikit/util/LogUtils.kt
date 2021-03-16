package com.sunyeyu.video.uikit.util

import android.util.Log

class LogUtils {
    companion object{
        var tag = "player"
        var debug = true
        public fun I(msg:String){
            if(debug){
                Log.i(tag,msg)
            }
        }
    }
}