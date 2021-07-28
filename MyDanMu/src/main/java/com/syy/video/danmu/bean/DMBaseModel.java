package com.syy.video.danmu.bean;
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

import android.graphics.Bitmap;
import android.graphics.RectF;
import android.view.View;

import com.syy.video.danmu.Direction;
import com.syy.video.danmu.Util;


/**
 * 弹幕数据模型
 */

public class DMBaseModel {
    public final Bitmap bitmap;
    public final RectF rect = new RectF();
    public final int priority;

    public DMBaseModel(View itemView) {
        this(itemView, 0);
    }

    public DMBaseModel(View itemView, int priority) {
        bitmap = Util.convertViewToBitmap(itemView);
        this.priority = priority;
        this.rect.set(0, 0, bitmap.getWidth(), bitmap.getHeight());
    }

    /**
     * 是否需要绘制
     * @param direction 弹幕运动方向
     * @param displayDis 展示区域长度（相对于运动方向的长度）
     * @return 是否需要绘制该弹幕（如果没有展示出来，则不需要绘制）
     */
    public boolean isNeedDrawDM(Direction direction, int displayDis) {
        switch (direction) {
            case RIGHT_LEFT:
            case LEFT_RIGHT:
                return rect.left < displayDis;
            case DOWN_UP:
            case UP_DOWN:
                return rect.top < displayDis;
        }
        throw new RuntimeException("not direction " + direction.name() + " in 'isNeedDraw()'");
    }

//    @Override
//    public int compareTo(BaseDmEntity o) {
//        if (o == null) return 0;
//        if (this.priority > o.priority) {
//            return 1;
//        } else if (this.priority < o.priority) {
//            return -1;
//        } else {
//            return 0;
//        }
//    }

    @Override
    public boolean equals(Object obj) {
        return super.equals(obj);
    }

    @Override
    public int hashCode() {
        return super.hashCode();
    }
}
