package com.syy.video.danmu.widget;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.kwai.video.danmuimpl.R;
import com.syy.video.danmu.DM;
import com.syy.video.danmu.Direction;
import com.syy.video.danmu.LogUtil;
import com.syy.video.danmu.Util;
import com.syy.video.danmu.listener.OnAddBrrageListener;
import com.syy.video.danmu.proxy.SurfaceControlProxy;
import com.syy.video.danmu.proxy.SurfaceProxy;


/**
 * 用SurfaceView实现弹幕
 * Created by jiaji on 2018/2/19.
 */

public class BrrageSurfaceView extends SurfaceView implements SurfaceHolder.Callback, DM, SurfaceControlProxy.OnDrawListener {
    private SurfaceHolder mSurfaceHolder;
    private SurfaceControlProxy mSurfaceControlProxy;
    private int mWidth;
    private int mHeight;
    private SurfaceControlProxy.Builder builder;

    public BrrageSurfaceView(Context context) {
        this(context, null);
    }

    public BrrageSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public BrrageSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initHolder();

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.DMSurfaceView, defStyleAttr, 0);

        final Direction direction = Direction.getType(a.getInt(R.styleable.DMSurfaceView_dm_direction, Direction.RIGHT_LEFT.value));
        final int span = a.getDimensionPixelOffset(R.styleable.DMSurfaceView_dm_span, Util.dp2px(context, 2));
        final int sleep = a.getInteger(R.styleable.DMSurfaceView_dm_sleep, 0);
        final int spanTime = a.getInteger(R.styleable.DMSurfaceView_dm_span_time, 0);
        final int vSpace = a.getDimensionPixelOffset(R.styleable.DMSurfaceView_dm_v_space, Util.dp2px(context, 10));
        final int hSpace = a.getDimensionPixelOffset(R.styleable.DMSurfaceView_dm_h_space, Util.dp2px(context, 10));

        builder = new SurfaceControlProxy.Builder()
                .setDirection(direction)
                .setSpan(span)
                .setSleep(sleep)
                .setSpanTime(spanTime)
                .sethSpace(hSpace)
                .setvSpace(vSpace);

        a.recycle();
    }

    private void initHolder() {
        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);
        setZOrderOnTop(true);
        mSurfaceHolder.setFormat(PixelFormat.TRANSPARENT);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (mWidth == width && mHeight == height) return;
        if (mSurfaceControlProxy != null) {
            mSurfaceControlProxy.destroy();
        }
        this.mWidth = width;
        this.mHeight = height;
        mSurfaceControlProxy =
                builder.setSurfaceProxy(new SurfaceProxy(mSurfaceHolder))
                        .setWidth(mWidth)
                        .setHeight(mHeight)
                        .build();
        mSurfaceControlProxy.setOnDrawListener(this);
        mSurfaceControlProxy.start();
    }

    @Override
    public void onWindowFocusChanged(boolean hasWindowFocus) {
        super.onWindowFocusChanged(hasWindowFocus);
        LogUtil.e("DMSurfaceView onWindowFocusChanged() - > " + hasWindowFocus);
        if (hasWindowFocus) {
            mSurfaceControlProxy.resume();
        } else {
            mSurfaceControlProxy.pause();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mSurfaceControlProxy.destroy();
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        mSurfaceControlProxy.pause();
    }

    public void setOnDMAddListener(OnAddBrrageListener l) {
        builder.setOnDMAddListener(l);
    }

    @Override
    public SurfaceControlProxy getController() {
        return mSurfaceControlProxy;
    }

    @Override
    public void onPreDraw(Canvas canvas) {

    }
}
