package com.sunyeyu.video.uikit.widget;

import android.content.Context;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import com.sunyeyu.video.uikit.anative.R;


public class CustomSeekBar extends RelativeLayout {

    private TextView mTvLeft;
    private TextView mTvRight;
    private SeekBar mSeekBar;
    private RelativeLayout mRlParent;

    public CustomSeekBar(Context context) {
        super(context);
        init();
    }

    public CustomSeekBar(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public CustomSeekBar(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        inflate(getContext(), R.layout.layout_custom_seekbar,this);
        mTvLeft = findViewById(R.id.tv_time_left);
        mTvRight = findViewById(R.id.tv_time_right);
        mSeekBar = findViewById(R.id.seek_bar);
        mRlParent = findViewById(R.id.rl_seek);

        mRlParent.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                Rect seekRect = new Rect();
                mSeekBar.getHitRect(seekRect);
                float y = seekRect.top + seekRect.height() / 2;
                // seekBar only accept relative x
                float x = motionEvent.getX() - seekRect.left;
                if (x < 0) {
                    x = 0;
                } else if (x > seekRect.width()) {
                    x = seekRect.width();
                }
                MotionEvent me = MotionEvent.obtain(motionEvent.getDownTime(), motionEvent.getEventTime(),
                        motionEvent.getAction(), x, y, motionEvent.getMetaState());

                return mSeekBar.onTouchEvent(me);
            }
        });
    }


    public void setTvleft(long left){
        mTvLeft.setText(getMinute(left));
    }

    public void setTvRight(long right){
        mTvRight.setText(getMinute(right));
    }

    public void setSeekbarChange(SeekBar.OnSeekBarChangeListener onSeekBarChangeListener){
        mSeekBar.setOnSeekBarChangeListener(onSeekBarChangeListener);
    }

    public void setProgress(int progress){
        mSeekBar.setProgress(progress);
    }

    public int getProgress(){
      return   mSeekBar.getProgress();
    }
    private String getMinute(long mSeconds) {
        if (mSeconds < 60) {
            return "00:" + calcTimeFormat(mSeconds);
        } else {
            int second = (int) (mSeconds % 60);
            int minute = (int) (mSeconds / 60);
            return calcTimeFormat(minute) + ":" + calcTimeFormat(second);
        }

    }

    private String calcTimeFormat(long mSeconds) {
        return mSeconds >= 10 ? mSeconds + "" : "0" + mSeconds;
    }


}
