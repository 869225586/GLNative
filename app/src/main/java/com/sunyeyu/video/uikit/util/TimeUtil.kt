package com.sunyeyu.video.uikit.util

class TimeUtil {
    companion object{
        public fun getMinute(mSeconds: Long): String? {
            return if (mSeconds < 60) {
                "00:" + calcTimeFormat(mSeconds)
            } else {
                val second = (mSeconds % 60).toInt()
                val minute = (mSeconds / 60).toInt()
                calcTimeFormat(minute.toLong()) + ":" + calcTimeFormat(second.toLong())
            }
        }

        public fun calcTimeFormat(mSeconds: Long): String {
            return if (mSeconds >= 10) mSeconds.toString() + "" else "0$mSeconds"
        }
    }
}