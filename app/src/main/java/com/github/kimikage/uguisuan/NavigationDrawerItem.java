/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

public class NavigationDrawerItem {
    private int mLabelId = -1;
    private int mIconId = -1;

    public NavigationDrawerItem(int labelId, int iconId) {
        mLabelId = labelId;
        mIconId = iconId;
    }

    public int getLabelId() {
        return mLabelId;
    }

    public int getIconId() {
        return mIconId;
    }
}
