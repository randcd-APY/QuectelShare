/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in the
 *            documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *            the names of its contributors may be used to endorse or promote
 *            products derived from this software without specific prior written
 *            permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bluetooth.hiddtestapp;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ToggleButton;

public class MouseFragment extends Fragment {

    private MainActivity mActivity;

    private View mTouchpad;

    private View mScrollZone;

    private int mSpeed = 3;

    private int mScrollSpeed = 3;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.mouse_fragment, container, false);

        // setup touchpad
        mTouchpad = view.findViewById(R.id.touchpad);
        mTouchpad.setOnTouchListener(new OnTouchListener() {

            private int mPrevX;

            private int mPrevY;

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        mPrevX = (int) (event.getX() * mSpeed);
                        mPrevY = (int) (event.getY() * mSpeed);
                        break;

                    case MotionEvent.ACTION_MOVE:
                        int x = (int) (event.getX() * mSpeed);
                        int y = (int) (event.getY() * mSpeed);

                        mActivity.getHidWrapper().mouseMove(x - mPrevX, y - mPrevY);

                        mPrevX = x;
                        mPrevY = y;
                        break;
                }

                return true;
            }
        });

        // setup scroll
        mScrollZone = view.findViewById(R.id.scrollzone);
        mScrollZone.setOnTouchListener(new OnTouchListener() {

            private int mPrevY;

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        mPrevY = (int) (event.getY() * mScrollSpeed);
                        break;

                    case MotionEvent.ACTION_MOVE:
                        int y = (int) (event.getY() * mScrollSpeed);

                        mActivity.getHidWrapper().mouseScroll((byte) (mPrevY - y));

                        mPrevY = y;
                        break;
                }

                return true;
            }
        });

        // setup buttons
        ViewGroup bar = (ViewGroup) view.findViewById(R.id.buttons);
        int count = bar.getChildCount();

        for (int i = 0; i < count; i++) {
            View child = bar.getChildAt(i);

            try {
                Button button = (Button) child;
                button.setOnTouchListener(new OnTouchListener() {

                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        int which = Integer.valueOf((String) v.getTag());

                        switch (event.getAction()) {
                            case MotionEvent.ACTION_DOWN:
                                mActivity.getHidWrapper().mouseButtonDown(which);
                                break;

                            case MotionEvent.ACTION_UP:
                                mActivity.getHidWrapper().mouseButtonUp(which);
                                break;
                        }

                        return false;
                    }

                });
            } catch (ClassCastException e) {
                // not a button :)
            }
        }

        // setup speed controls
        bar = (ViewGroup) view.findViewById(R.id.speed_control);
        count = bar.getChildCount();

        for (int i = 0; i < count; i++) {
            View child = bar.getChildAt(i);

            try {
                ToggleButton button = (ToggleButton) child;
                button.setOnClickListener(new OnClickListener() {

                    @Override
                    public void onClick(View v) {
                        ToggleButton button = (ToggleButton) v;

                        // do not allow to uncheck button
                        if (!button.isChecked()) {
                            button.setChecked(true);
                            return;
                        }

                        updateSpeed(Integer.parseInt((String) button.getTag()));
                    }

                });
            } catch (ClassCastException e) {
                // not a button :)
            }
        }

        // setup scroll speed controls
        bar = (ViewGroup) view.findViewById(R.id.scroll_speed_control);
        count = bar.getChildCount();

        for (int i = 0; i < count; i++) {
            View child = bar.getChildAt(i);

            try {
                ToggleButton button = (ToggleButton) child;
                button.setOnClickListener(new OnClickListener() {

                    @Override
                    public void onClick(View v) {
                        ToggleButton button = (ToggleButton) v;

                        // do not allow to uncheck button
                        if (!button.isChecked()) {
                            button.setChecked(true);
                            return;
                        }

                        updateScrollSpeed(Integer.parseInt((String) button.getTag()));
                    }

                });
            } catch (ClassCastException e) {
                // not a button :)
            }
        }

        return view;
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        try {
            mActivity = (MainActivity) activity;
        } catch (ClassCastException e) {
            throw new ClassCastException(this.getClass().getSimpleName()
                    + " can be only attached to " + MainActivity.class.getSimpleName());
        }
    }

    @Override
    public void onStart() {
        super.onStart();

        // intial value
        updateSpeed(3);
        updateScrollSpeed(3);
    }

    private void updateSpeed(int newSpeed) {
        // note: we assume at least button have proper speed-tag so this will
        // check what it should

        mSpeed = newSpeed;

        ViewGroup bar = (ViewGroup) getView().findViewById(R.id.speed_control);
        int count = bar.getChildCount();

        for (int i = 0; i < count; i++) {
            View child = bar.getChildAt(i);

            try {
                ToggleButton button = (ToggleButton) child;

                int speed = Integer.parseInt((String) button.getTag());

                button.setChecked(speed == newSpeed);
            } catch (ClassCastException e) {
                // not a button :)
            }
        }
    }

    private void updateScrollSpeed(int newSpeed) {
        // note: we assume at least button have proper speed-tag so this will
        // check what it should

        mScrollSpeed = newSpeed;

        ViewGroup bar = (ViewGroup) getView().findViewById(R.id.scroll_speed_control);
        int count = bar.getChildCount();

        for (int i = 0; i < count; i++) {
            View child = bar.getChildAt(i);

            try {
                ToggleButton button = (ToggleButton) child;

                int speed = Integer.parseInt((String) button.getTag());

                button.setChecked(speed == newSpeed);
            } catch (ClassCastException e) {
                // not a button :)
            }
        }
    }
}
