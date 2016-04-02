/*
 * Copyright (c) 2016 Uguisu-Anne Project
 *
 * This software is distributed under an MIT-style license.
 * See LICENSE file for more information.
 */

package com.github.kimikage.uguisuan;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

public class MainActivity extends AppCompatActivity
        implements NavigationDrawerFragment.NavigationDrawerCallbacks,
        WaveReaderFragment.WaveReaderCallbacks,
        WavePlayerFragment.WavePlayerCallbacks {

    /**
     * Fragment managing the behaviors, interactions and presentation of the navigation drawer.
     */
    private NavigationDrawerFragment mNavigationDrawerFragment;
    private DrawerLayout mDrawerLayout;
    private CharSequence mTitle;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        mNavigationDrawerFragment = (NavigationDrawerFragment)
                getSupportFragmentManager().findFragmentById(R.id.navigation_drawer);
        mTitle = getTitle();

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);

        if (toolbar != null) {
            toolbar.setTitle(mTitle);

            toolbar.setNavigationIcon(R.drawable.ic_menu_white_24dp);
            toolbar.setNavigationOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    mDrawerLayout.openDrawer(GravityCompat.START);
                }
            });

            toolbar.inflateMenu(R.menu.global);
            toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
                @Override
                public boolean onMenuItemClick(MenuItem menuItem) {
                    switch (menuItem.getItemId()) {
                        case R.id.action_play_pause:
                            if (isPlaying()) {
                                pause();
                            } else {
                                play();
                            }
                            return true;
                    }
                    return false;
                }
            });
        }

        setPlayAndPauseButton();

        // Set up the drawer.
        mNavigationDrawerFragment.setUp(R.id.navigation_drawer, mDrawerLayout);
    }

    @Override
    public void finish() {
        super.finish();
    }

    @Override
    public void onNavigationDrawerItemSelected(long id) {
        switch ((int)id) {
            case R.string.action_load:
                load();
                return;
        }
    }

    private void setPlayAndPauseButton() {
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        if (toolbar == null) {
            return;
        }
        if (toolbar.getMenu() == null) {
            return;
        }
        boolean playing = isPlaying();

        MenuItem menuItem = (MenuItem) toolbar.getMenu().findItem(R.id.action_play_pause);
        menuItem.setTitle(getResources().getString(
                playing ? R.string.action_pause : R.string.action_play));
        menuItem.setIcon(
                playing ? R.drawable.ic_pause_circle_filled_white_48dp :
                        R.drawable.ic_play_circle_filled_white_48dp);
        View v = findViewById(R.id.action_play_pause);
        if (v != null) {
            v.setSoundEffectsEnabled(playing);
        }
    }

    private void load() {
        if (isPlaying()) {
            pause();
        }
        WaveReaderFragment f = WaveReaderFragment.newInstance("!");
        addFragment(f, WaveReaderFragment.TAG);
    }

    public void onSectionAttached(int number) {
        // TBD.
    }

    public void play() {
        WavePlayerFragment f = WavePlayerFragment.newInstance();
        addFragment(f, WavePlayerFragment.TAG);
    }

    public void pause() {
        FragmentManager fm = getSupportFragmentManager();
        Fragment f = fm.findFragmentByTag(WavePlayerFragment.TAG);
        if (f == null) {
            setPlayAndPauseButton();
            return;
        }
        ((WavePlayerFragment) f).stopPlay();
    }

    @Override
    public void onReadFinished() {
        removeFragment(WaveReaderFragment.TAG);
    }

    @Override
    public void onStartPlay() {
        setPlayAndPauseButton();
    }

    @Override
    public void onStopPlay() {
        removeFragment(WavePlayerFragment.TAG);
        setPlayAndPauseButton();
    }

    private void addFragment(Fragment fragment, String fragmentTag) {
        FragmentTransaction ft = this.getSupportFragmentManager().beginTransaction();
        ft.add(fragment, fragmentTag);
        ft.commit();
    }

    private void removeFragment(String fragmentTag) {
        FragmentManager fm = getSupportFragmentManager();
        Fragment f = fm.findFragmentByTag(fragmentTag);
        FragmentTransaction ft = fm.beginTransaction();
        ft.remove(f);
        ft.commit();
    }

    private boolean isPlaying() {
        FragmentManager fm = getSupportFragmentManager();
        Fragment f = fm.findFragmentByTag(WavePlayerFragment.TAG);
        if (f == null) {
            return false;
        }
        return ((WavePlayerFragment) f).isPlaying();
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {
        /**
         * The fragment argument representing the section number for this
         * fragment.
         */
        private static final String ARG_SECTION_NUMBER = "section_number";

        public PlaceholderFragment() {
        }

        /**
         * Returns a new instance of this fragment for the given section
         * number.
         */
        public static PlaceholderFragment newInstance(int sectionNumber) {
            PlaceholderFragment fragment = new PlaceholderFragment();
            Bundle args = new Bundle();
            args.putInt(ARG_SECTION_NUMBER, sectionNumber);
            fragment.setArguments(args);
            return fragment;
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            return rootView;
        }

        @Override
        public void onAttach(Context context) {
            super.onAttach(context);
            ((MainActivity) context).onSectionAttached(
                    getArguments().getInt(ARG_SECTION_NUMBER));
        }
    }
}
