package com.syy.video.danmu.proxy;
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

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.util.SparseArray;
import android.view.View;

import com.syy.video.danmu.Direction;
import com.syy.video.danmu.LogUtil;
import com.syy.video.danmu.bean.DMBaseModel;
import com.syy.video.danmu.listener.OnAddBrrageListener;
import com.syy.video.danmu.listener.ViewCreator;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


/**
 * 动画路径绘制登帮助类
 */
public class SurfaceControlProxy {
    private Direction mDirection = Direction.RIGHT_LEFT;
    // 新弹幕
    private Queue<DMBaseModel> mNewDMQueue = new ConcurrentLinkedQueue<>();
    // 已添加到屏幕的弹幕
    private Queue<DMBaseModel> mAddedMDList = new ConcurrentLinkedQueue<>();
    private int mWidth, mHeight;
    private float offset;
    private int hSpace = 20;// 水平间距
    private int vSpace = 20;// 垂直间距
    private float span = 5F;// 刷新一次的跨度
    private int spanTime = 0; // 一个跨度需要多少时间
    private float speed = 0F; //速度
    private boolean isH; // 是否是横向跑的
    private ExecutorService exec = Executors.newCachedThreadPool();
    private OnAddBrrageListener mOnAddBrrageListener;
    private Handler mMainHandler;
    private DrawThread mDrawThread;
    private Path mClipPath;
    private OnDrawListener mOnDrawListener;

    private SurfaceControlProxy() {

    }

    public void setSurfaceProxy(SurfaceProxy surfaceProxy) {
        mDrawThread = new DrawThread(surfaceProxy);
    }

    public void setSize(int width, int height) {
        this.mWidth = width;
        this.mHeight = height;
        initOffset();
    }

    public void initOffset() {
        switch (mDirection) {
            case RIGHT_LEFT:
                offset = mWidth;
                if (span > 0) span = -span;
                break;
            case LEFT_RIGHT:
            case UP_DOWN:
                offset = 0;
                if (span < 0) span = -span;
                break;
            case DOWN_UP:
                offset = mHeight;
                if (span > 0) span = -span;
                break;
        }
        updateSpeed();
    }

    public void start() {
        if (mDrawThread.isRun()) return;
        mDrawThread.setOnDrawListener(new DrawThread.OnDrawListener() {
            @Override
            public void onDraw(Canvas canvas) {
                runTask(canvas);
            }
        });
        mDrawThread.start();
    }

    private Handler getMainHandler() {
        if (mMainHandler == null) {
            return new Handler(Looper.getMainLooper());
        }
        return mMainHandler;
    }

    private long lastTime = 0L;
    private void runTask(Canvas canvas) {
        if (spanTime > 0) {
            final long nowTime = SystemClock.uptimeMillis();
            final long disTime = nowTime - lastTime;
            if (lastTime != 0L && disTime < 100) { // 第一次进入时lastTime=0，同时暂停后的时间差比较大如果大于100ms就可以判断为暂停过，需要从新计时
                offset += speed * disTime;
            }
            LogUtil.i("disTime = " + disTime + ", offset = " + offset + ", speed = " + speed);
            lastTime = nowTime;
        } else {
            offset += span;
        }

        drawDM(canvas, offset, false);
        if (addDMInQueue()) {

        } else if (mAddedMDList.size() == 0) {
            mDrawThread.setDraw(false);
            if (mOnAddBrrageListener != null) {
                getMainHandler().post(new Runnable() {
                    @Override
                    public void run() {
                        mOnAddBrrageListener.addedAll();
                    }
                });
            }
        }
    }

    private void drawDM(Canvas canvas, float value, boolean isOnlyClear) {

        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        if (isOnlyClear) return;
        canvas.save();
        if (mOnDrawListener != null) {
            mOnDrawListener.onPreDraw(canvas);
        }
        if (isH) {
            canvas.translate(value, 0);
        }
        else {
            canvas.translate(0, value);
        }

        Iterator<DMBaseModel> iterator = mAddedMDList.iterator();
        while (iterator.hasNext()) {
            DMBaseModel entity = iterator.next();
            boolean removeThisEntity = false;
            switch (mDirection) {
                case RIGHT_LEFT:
                    removeThisEntity = offset < -entity.rect.right;
                    break;
                case LEFT_RIGHT:
                    removeThisEntity = offset > mWidth + entity.rect.right;
                    break;
                case DOWN_UP:
                    removeThisEntity = offset < -entity.rect.bottom;
                    break;
                case UP_DOWN:
                    removeThisEntity = offset > mHeight + entity.rect.bottom;
                    break;

            }
            if (removeThisEntity) {
                iterator.remove();
            }
            switch (mDirection) {
                case RIGHT_LEFT:
                case DOWN_UP:
                    canvas.drawBitmap(entity.bitmap, entity.rect.left, entity.rect.top, null);
                    break;
                case LEFT_RIGHT:
                    canvas.drawBitmap(entity.bitmap, -entity.rect.left - entity.rect.width(), entity.rect.top, null);
                    break;
                case UP_DOWN:
                    canvas.drawBitmap(entity.bitmap, entity.rect.left, -entity.rect.top - entity.rect.height(), null);
                    break;
            }
        }
        if (mClipPath != null) {
            canvas.clipPath(mClipPath);
        }
        canvas.restore();
    }

    public void updatePath(Path maskPath){
        mClipPath = maskPath;
    }

    public void add(final ViewCreator viewCreater) {
        exec.execute(new Runnable() {
            @Override
            public void run() {
                DMBaseModel entity = new DMBaseModel(viewCreater.build());
                addToQueue(entity);
            }
        });
    }

    public void add(final View templateView) {
        exec.execute(new Runnable() {
            @Override
            public void run() {
                if (mAddedMDList.size() < 10) {
                    DMBaseModel entity = new DMBaseModel(templateView);
                    addToQueue(entity);
                } else {
                    LogUtil.e("over max wait danmu size 10");
                }
            }
        });
    }

    public void addToQueue(DMBaseModel entity) {
        if (entity == null) throw new RuntimeException("entity cannot null");
        mNewDMQueue.add(entity);
        if (!mDrawThread.isDraw()) {
            initOffset();
            mDrawThread.setDraw(true);
        }
    }

    private SparseArray<LinkedList<DMBaseModel>> hierarchy = new SparseArray<>();
    private boolean addDMInQueue() {
        DMBaseModel entity = mNewDMQueue.peek();

        if (entity == null) return false;
        final float minLimit;
        final float maxLimit;
        switch (mDirection) {
            case RIGHT_LEFT:
                minLimit = mWidth - offset;
                maxLimit = minLimit + mWidth;
                break;
            case LEFT_RIGHT:
                minLimit = offset;
                maxLimit = minLimit + mWidth;
                break;
            case DOWN_UP:
                minLimit = mHeight - offset;
                maxLimit = minLimit + mHeight;
                break;
            case UP_DOWN:
                minLimit = offset;
                maxLimit = minLimit + mHeight;
                break;
            default:
                minLimit = 0;
                maxLimit = 0;
                break;
        }
        // 没有添加过弹幕的时候
        if (mAddedMDList.size() == 0) {
            addToDisplay(entity);
            return true;
        }

        if (isH) {
            for (DMBaseModel addedDM: mAddedMDList) {
                if (hierarchy.get((int)addedDM.rect.top) == null) {// 没有初始化的情况
                    hierarchy.put((int)addedDM.rect.top, new LinkedList<DMBaseModel>());
                }
                hierarchy.get((int)addedDM.rect.top).addFirst(addedDM);
            }
        }
        else {
            for (DMBaseModel addedDM: mAddedMDList) {
                if (hierarchy.get((int)addedDM.rect.left) == null) {// 没有初始化的情况
                    hierarchy.put((int)addedDM.rect.left, new LinkedList<DMBaseModel>());
                }
                hierarchy.get((int)addedDM.rect.left).addFirst(addedDM);
            }
        }

        DMBaseModel lastDm = null;
        for (int i = 0; i < hierarchy.size(); i++) {
            LinkedList<DMBaseModel> linkedList = hierarchy.get(hierarchy.keyAt(i));
            if (linkedList.size() == 0) continue;
            DMBaseModel lastEntity = linkedList.getFirst();

            // 上面或左边被空起来的情况
            if (isH) {
                if (lastDm == null && lastEntity.rect.top >= lastEntity.rect.height() + vSpace) {
                    entity.rect.offsetTo(minLimit, 0);
                    addToDisplay(entity);
                    return true;
                }
            }
            else {
                if (lastDm == null && lastEntity.rect.left >= lastEntity.rect.width() + hSpace) {
                    entity.rect.offsetTo(0, minLimit);
                    addToDisplay(entity);
                    return true;
                }
            }

            // 中间有可能被空起来的情况
            if (isH) {
                if (lastDm != null && lastDm.rect.bottom + lastEntity.rect.height() < lastEntity.rect.top) {
                    entity.rect.offsetTo(minLimit, lastDm.rect.bottom + vSpace);
                    addToDisplay(entity);
                    return true;
                }
            }
            else {
                if (lastDm != null && lastDm.rect.right + lastEntity.rect.width() < lastEntity.rect.left) {
                    entity.rect.offsetTo(lastDm.rect.right + hSpace, minLimit);
                    addToDisplay(entity);
                    return true;
                }
            }

            lastDm = lastEntity;

            if (isH) {
                if (lastEntity.rect.right < maxLimit) {
                    entity.rect.offsetTo((Math.max(lastEntity.rect.right, minLimit)) + hSpace, lastEntity.rect.top);
                    addToDisplay(entity);
                    return true;
                }
            }
            else {
                if (lastEntity.rect.bottom < maxLimit) {
                    entity.rect.offsetTo(lastEntity.rect.left, (Math.max(lastEntity.rect.bottom, minLimit)) + vSpace);
                    addToDisplay(entity);
                    return true;
                }
            }

        }

        if (lastDm == null) throw new RuntimeException("lastDm can not null");

        if (isH) {
            if (lastDm.rect.bottom < mHeight - lastDm.rect.height()) {
                entity.rect.offsetTo(minLimit, lastDm.rect.bottom + vSpace);
                addToDisplay(entity);
                return true;
            }
        }
        else {
            if (lastDm.rect.right < mWidth - lastDm.rect.width()) {
                entity.rect.offsetTo(lastDm.rect.right + hSpace, minLimit);
                addToDisplay(entity);
                return true;
            }
        }


        return false;
    }

    private synchronized void addToDisplay(final DMBaseModel entity) {
        if (entity == null) return;
        mNewDMQueue.remove(entity);
        mAddedMDList.add(entity);
        hierarchy.clear();
        if (mOnAddBrrageListener != null) {
            getMainHandler().post(new Runnable() {
                @Override
                public void run() {
                    mOnAddBrrageListener.added(entity);
                }
            });
        }
    }

    public void setOnDMAddListener(OnAddBrrageListener l) {
        this.mOnAddBrrageListener = l;
    }

    public void resume() {
        mDrawThread.setDraw(true);
    }

    public void pause() {
        mDrawThread.setDraw(false);
    }

    public void clean() {
        mNewDMQueue.clear();
        mAddedMDList.clear();
        mDrawThread.setDraw(false);
        initOffset();
    }

    public void destroy() {
        clean();
        mMainHandler = null;
        mDrawThread.setRun(false);
        mDrawThread.interrupt();
    }

    public void setDirection(Direction mDirection) {
        this.mDirection = mDirection;
        this.isH = mDirection == Direction.LEFT_RIGHT || mDirection == Direction.RIGHT_LEFT; // 是否是横向
    }

    public void sethSpace(int hSpace) {
        this.hSpace = hSpace;
    }

    public void setvSpace(int vSpace) {
        this.vSpace = vSpace;
    }

    public void setSpan(int span) {
        if (span == 0) span = 2;
        this.span = this.span < 0 ? -span : span;
        updateSpeed();
    }

    public void setSpanTime(int spanTime) {
        this.spanTime = spanTime;
        updateSpeed();
    }

    private void updateSpeed() {
        if (spanTime > 0L && span != 0) {
            speed = span / spanTime;
        }
    }

    public static class Builder {
        private SurfaceProxy surfaceProxy;
        private Direction direction;
        private int span;
        private int sleep;
        private int spanTime;
        private int vSpace;
        private int hSpace;
        private int width;
        private int height;
        private OnAddBrrageListener mOnAddBrrageListener;

        public Builder() {
        }

        public Builder setSurfaceProxy(SurfaceProxy surfaceProxy) {
            this.surfaceProxy = surfaceProxy;
            return this;
        }

        public Builder setDirection(Direction direction) {
            this.direction = direction;
            return this;
        }

        public Builder setSpan(int span) {
            this.span = span;
            return this;
        }

        public Builder setSleep(int sleep) {
            this.sleep = sleep;
            return this;
        }

        public Builder setSpanTime(int spanTime) {
            this.spanTime = spanTime;
            return this;
        }

        public Builder setvSpace(int vSpace) {
            this.vSpace = vSpace;
            return this;
        }

        public Builder sethSpace(int hSpace) {
            this.hSpace = hSpace;
            return this;
        }

        public Builder setWidth(int width) {
            this.width = width;
            return this;
        }

        public Builder setHeight(int height) {
            this.height = height;
            return this;
        }

        public void setOnDMAddListener(OnAddBrrageListener l) {
            this.mOnAddBrrageListener = l;
        }

        public SurfaceControlProxy build() {
            SurfaceControlProxy surfaceControlProxy = new SurfaceControlProxy();
            surfaceControlProxy.setSurfaceProxy(surfaceProxy);
            surfaceControlProxy.setDirection(direction);
            surfaceControlProxy.setSpan(span);
            surfaceControlProxy.setSpanTime(spanTime == 0 ? sleep : spanTime);
            surfaceControlProxy.setvSpace(vSpace);
            surfaceControlProxy.sethSpace(hSpace);
            surfaceControlProxy.setSize(width, height);
            surfaceControlProxy.setOnDMAddListener(mOnAddBrrageListener);
            return surfaceControlProxy;
        }
    }

    public void setOnDrawListener(OnDrawListener listener){
        mOnDrawListener = listener;
    }

    public interface OnDrawListener {
        void onPreDraw(Canvas canvas);
    }
}
