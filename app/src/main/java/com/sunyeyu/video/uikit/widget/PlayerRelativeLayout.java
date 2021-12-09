package com.sunyeyu.video.uikit.widget;

import android.content.Context;
import android.media.AudioManager;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.RelativeLayout;
import android.widget.SeekBar;

/**
 * 全屏手势 支持
 * 左右滑动检测 ，
 * 上下滑动检测  包含左侧上下滑 与右侧上下滑
 */
public class PlayerRelativeLayout extends RelativeLayout {
    public PlayerRelativeLayout(Context context) {
        super(context);
    }

    public PlayerRelativeLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public PlayerRelativeLayout(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    float startX;
    float startY;
    float originx;
    float originy;
    boolean isLeftRightmove = false;
    boolean isUpDownMove = false;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        final int screenWidth = com.blankj.utilcode.util.ScreenUtils.getScreenWidth();
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                startX = event.getX();
                startY = event.getY();
                originx = startX;
                originy = startY;
                break;
            case MotionEvent.ACTION_MOVE:
                float endY = event.getY();
                float distanceY = endY - startY;
                float distanceX = event.getX() - startX;
                if (Math.abs(distanceX) > 3 && Math.abs(distanceY) < 3 && !isUpDownMove) {
                    isLeftRightmove = true;
                    //左右滑
                    if (touchGestor != null) {
                        touchGestor.onLeftRightMove(event, distanceX < 0);
                    }

                } else if (Math.abs(distanceY) > 3 && Math.abs(distanceX) < 3 && !isLeftRightmove) {
                    isUpDownMove = true;
                    //上下滑
                    if (startX < screenWidth * 1.0 / 4) {
                        //左侧
                        if (touchGestor != null) {
                            touchGestor.onLeftUpDownMove(distanceY > 0);
                        }
                    } else if (startX > screenWidth * 3.0 / 4f) {
                        //右侧
                        if (touchGestor != null) {
                            touchGestor.onRightUpDwonMove(distanceY > 0);
                        }
                    }
                }
                startX = event.getX();
                startY = event.getY();
                break;
            case MotionEvent.ACTION_UP:
                if (Math.abs(event.getX() - originx) < 5 && Math.abs(event.getY() - originy) < 5) {
                    //点击事件
                    if (touchGestor != null) {
                        touchGestor.onClick();
                    }
                } else {
                    if (isLeftRightmove) {
                        if (touchGestor != null) {
                            touchGestor.onLeftRightMoveTouchUp();
                        }
                        isLeftRightmove = false;
                    } else if (isUpDownMove) {
                        if (touchGestor != null) {
                            touchGestor.onUpDownMoveTouchUp();
                        }
                        isLeftRightmove = false;
                    }
                }
                break;
        }
        return true;
    }

    /**
     * 改变声音的例子 配合seekbar
     *
     * @param down 是否向下滑动
     */
    public void changeVoice(boolean down) {
        SeekBar mVoiceSeekBar = null;
        AudioManager am = (AudioManager) getContext().getSystemService(Context.AUDIO_SERVICE);
        int maxVolume = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        int currentVolume = am.getStreamVolume(AudioManager.STREAM_MUSIC);
        mVoiceSeekBar.setProgress((int) (currentVolume * (100.0f / maxVolume)));
        int progress = mVoiceSeekBar.getProgress();
        if (down) {
            progress -= 3;
            mVoiceSeekBar.setProgress(progress < 0 ? 0 : progress);
        } else {
            progress += 3;
            mVoiceSeekBar.setProgress(progress > 100 ? 100 : progress);
        }
        am.setStreamVolume(AudioManager.STREAM_MUSIC, (int) ((maxVolume * 1.0f / 100) * mVoiceSeekBar.getProgress()), 0);
    }

    public interface onTouchGestor {
        void onLeftUpDownMove(boolean down);

        void onRightUpDwonMove(boolean down);

        void onLeftRightMove(MotionEvent event, boolean left);

        void onLeftRightMoveTouchUp();

        void onUpDownMoveTouchUp();

        void onClick();
    }

    onTouchGestor touchGestor;

    public void setTouchGestror(onTouchGestor touchGestror) {
        this.touchGestor = touchGestror;
    }

}
