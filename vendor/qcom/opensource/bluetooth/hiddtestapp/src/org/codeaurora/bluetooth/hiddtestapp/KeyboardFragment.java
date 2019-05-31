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
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.LinearLayout;

public class KeyboardFragment extends Fragment {

    public final static int KEYBOARD_TYPE_QWERTY = 0;
    public final static int KEYBOARD_TYPE_NAVIGATION_AND_NUMERIC = 1;

    private MainActivity mActivity;

    public KeyboardFragment() {
    }

    private int getType() {
        return getArguments().getInt("param");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                            Bundle savedInstanceState) {
        View view = null;
        switch (getType()) {
            case KEYBOARD_TYPE_NAVIGATION_AND_NUMERIC:
                view = createNavigationAndNumericKeyboard(getActivity());
                break;
            case KEYBOARD_TYPE_QWERTY:
            default:
                view = creatQwertyKeyboard(getActivity());
                break;
        }

        LinearLayout ll = (LinearLayout) inflater.inflate(R.layout.keyboard_fragment, container,
                false);
        ViewGroup keyboard = (ViewGroup) ll.findViewById(R.id.keyboard);
        keyboard.addView(view);

        return ll;
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

        mActivity.queryLeds(this);
    }

    public void updateLeds(boolean numLock, boolean capsLock, boolean scrollLock) {
        if (getView() != null) {
            updateLed(R.id.led_numlock, numLock);
            updateLed(R.id.led_capslock, capsLock);
            updateLed(R.id.led_scrolllock, scrollLock);
        }
    }

    private void updateLed(int resId, boolean active) {
        TextView led = (TextView) getView().findViewById(resId);

        if (led == null)
            return;

        led.setBackgroundColor(getResources().getColor(active ? R.color.led_on : R.color.led_off));
    }

    private Keyboard createKeyboard(Context context, int xmlResourceID) {

        Keyboard keyboard = new Keyboard(context, xmlResourceID);
        keyboard.setKeyboardListener(new Keyboard.KeyboardListener() {
            @Override
            public void onKeyUp(byte keyCode) {
                mActivity.getHidWrapper().keyboardKeyUp(keyCode);
            }

            @Override
            public void onKeyDown(byte keyCode) {
                mActivity.getHidWrapper().keyboardKeyDown(keyCode);
            }
        });
        return keyboard;
    }

    private View creatQwertyKeyboard(Context context) {
        return createKeyboard(context, R.xml.qwerty_keyboard);
    }

    private View createNavigationAndNumericKeyboard(Context context) {
        ViewGroup view = (ViewGroup) View.inflate(context, R.layout.numeric_keyboard, null);
        ViewGroup child;

        child = (ViewGroup) view.findViewById(R.id.navigation_keyboard);
        child.addView(createKeyboard(context, R.xml.navigation_keyboard));

        child = (ViewGroup) view.findViewById(R.id.numeric_keyboard);
        child.addView(createKeyboard(context, R.xml.numeric_keyboard));

        return view;
    }
}
