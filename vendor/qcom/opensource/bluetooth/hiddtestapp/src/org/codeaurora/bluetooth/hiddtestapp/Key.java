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

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.Button;

public class Key extends Button {

    private final KeyAttributes mKeyAttributes;
    private KeyListener mKeyListener;

    public static class KeyAttributes {
        public String keyFunction;
        public String mainLabel;
        public String shiftLabel;
        public byte keyCode;
    }

    public interface KeyListener {
        public void onKeyDown(String keyFunction, byte keyCode);

        public void onKeyUp(String keyFunction, byte keyCode);
    }

    public Key(Context context, KeyAttributes keyAttributes) {
        this(context, keyAttributes, null);
        setDefaultConfiguration();
    }

    public Key(Context context, KeyAttributes keyAttributes, AttributeSet attrs) {
        this(context, keyAttributes, attrs, android.R.attr.buttonStyleSmall);
    }

    public Key(Context context, KeyAttributes keyAttributes, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        mKeyAttributes = keyAttributes;
        setText(mKeyAttributes.mainLabel);
    }

    private void setDefaultConfiguration() {
        setPadding(1, 1, 1, 1);
    }

    public void setShiftState(boolean shiftOn) {
        String label = null;
        if (shiftOn) {
            label = mKeyAttributes.shiftLabel;
        }
        else {
            label = mKeyAttributes.mainLabel;
        }

        if (label != null && label.length() > 0) {
            setText(label);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                dispatchOnKeyDown();
                break;
            case MotionEvent.ACTION_UP:
                dispatchOnKeyUp();
                break;
        }
        return super.onTouchEvent(event);
    }

    private void dispatchOnKeyDown() {
        if (mKeyListener != null) {
            mKeyListener.onKeyDown(mKeyAttributes.keyFunction, mKeyAttributes.keyCode);
        }
    }

    private void dispatchOnKeyUp() {
        if (mKeyListener != null) {
            mKeyListener.onKeyUp(mKeyAttributes.keyFunction, mKeyAttributes.keyCode);
        }
    }

    public void setKeyListener(KeyListener keyListener) {
        mKeyListener = keyListener;
    }
}
