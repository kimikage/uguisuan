/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.content.Context;
import android.content.res.Resources;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class NavigationDrawerListAdapter extends BaseExpandableListAdapter {
    private List<String> mGroups;
    private List<List<NavigationDrawerItem>> mChildren;
    private Context mContext = null;

    public NavigationDrawerListAdapter(Context context) {
        mContext = context;
        mGroups = new ArrayList<>();
        mChildren = new ArrayList<>();
    }

    public void addGroup(int labelId, List<NavigationDrawerItem> children) {
        Resources r = mContext.getResources();
        mGroups.add(r.getString(labelId));
        mChildren.add(children);
    }

    @Override
    public int getGroupCount() {
        return mGroups.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        return mChildren.get(groupPosition).size();
    }

    @Override
    public Object getGroup(int groupPosition) {
        return mGroups.get(groupPosition);
    }

    @Override
    public Object getChild(int groupPosition, int childPosition) {
        List<NavigationDrawerItem> children = mChildren.get(groupPosition);
        return children.get(childPosition);
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return (groupPosition << 16) | childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return true;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        View view = convertView;
        if (convertView == null) {
            view = LayoutInflater.from(mContext).inflate(R.layout.navigation_drawer_group, null);
        }

        TextView label = (TextView) view.findViewById(R.id.label);
        label.setText(mGroups.get(groupPosition));

        ImageView iconMore = (ImageView) view.findViewById(R.id.icon_more);
        ImageView iconLess = (ImageView) view.findViewById(R.id.icon_less);

        iconMore.setVisibility(isExpanded ? View.INVISIBLE : View.VISIBLE);
        iconLess.setVisibility(isExpanded ? View.VISIBLE : View.INVISIBLE);


        if (getChildrenCount(groupPosition) == 0) {
            iconMore.setVisibility(View.INVISIBLE);
            iconLess.setVisibility(View.INVISIBLE);
        }

        return view;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        View view = convertView;
        if (convertView == null) {
            view = LayoutInflater.from(mContext).inflate(R.layout.navigation_drawer_item, null);
        }

        NavigationDrawerItem item = (NavigationDrawerItem) getChild(groupPosition, childPosition);
        Resources r = mContext.getResources();

        TextView label = (TextView) view.findViewById(R.id.label);
        label.setText(r.getString(item.getLabelId()));

        int iconId = item.getIconId();
        if (iconId >= 0) {
            ImageView icon = (ImageView) view.findViewById(R.id.icon);
            icon.setImageResource(item.getIconId());
            icon.setVisibility(View.VISIBLE);
        }

        return view;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }
}
