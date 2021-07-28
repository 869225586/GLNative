package com.syy.video.danmu.widget;
/*
 * Copyright 2018 xujiaji
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.TextureView;

import com.kwai.video.danmuimpl.R;
import com.syy.video.danmu.DM;
import com.syy.video.danmu.Direction;
import com.syy.video.danmu.Util;
import com.syy.video.danmu.proxy.SurfaceControlProxy;
import com.syy.video.danmu.proxy.SurfaceProxy;


public class BrrageTextureView extends TextureView implements TextureView.SurfaceTextureListener, DM, SurfaceControlProxy.OnDrawListener {
    private Surface mSurface;
    private SurfaceControlProxy mSurfaceControlProxy;
    private final SurfaceControlProxy.Builder builder;

    public BrrageTextureView(Context context) {
        this(context, null);
    }

    public BrrageTextureView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public BrrageTextureView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        this.setSurfaceTextureListener(this);
        setOpaque(false);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.DMTextureView, defStyleAttr, 0);

        final Direction direction = Direction.getType(a.getInt(R.styleable.DMTextureView_dm_direction, Direction.RIGHT_LEFT.value));
        final int span = a.getDimensionPixelOffset(R.styleable.DMTextureView_dm_span, Util.dp2px(context, 2));
        final int sleep = a.getInteger(R.styleable.DMTextureView_dm_sleep, 0);
        final int spanTime = a.getInteger(R.styleable.DMTextureView_dm_span_time, 0);
        final int vSpace = a.getDimensionPixelOffset(R.styleable.DMTextureView_dm_v_space, Util.dp2px(context, 10));
        final int hSpace = a.getDimensionPixelOffset(R.styleable.DMTextureView_dm_h_space, Util.dp2px(context, 10));

        builder = new SurfaceControlProxy.Builder()
                .setDirection(direction)
                .setSpan(span)
                .setSleep(sleep)
                .setSpanTime(spanTime)
                .sethSpace(hSpace)
                .setvSpace(vSpace);
        a.recycle();
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mSurface = new Surface(surface);
        mSurfaceControlProxy =
                builder.setSurfaceProxy(new SurfaceProxy(mSurface))
                .setWidth(width)
                .setHeight(height)
                .build();
        mSurfaceControlProxy.start();
        mSurfaceControlProxy.setOnDrawListener(this);
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        mSurfaceControlProxy.setSize(width, height);
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        if (mSurface != null) mSurface.release();
        mSurface = null;
        mSurfaceControlProxy.destroy();
        return true;
    }

    @Override
    public void onWindowFocusChanged(boolean hasWindowFocus) {
        super.onWindowFocusChanged(hasWindowFocus);
        if (mSurface == null) return;
        if (hasWindowFocus) {
            mSurfaceControlProxy.resume();
        } else {
            mSurfaceControlProxy.pause();
        }
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }

    @Override
    public SurfaceControlProxy getController() {
        return mSurfaceControlProxy;
    }

    @Override
    public void onPreDraw(Canvas canvas) {

    }
}
