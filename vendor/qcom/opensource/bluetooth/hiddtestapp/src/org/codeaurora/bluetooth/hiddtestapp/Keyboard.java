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
import android.content.res.XmlResourceParser;
import android.util.AttributeSet;
import android.widget.LinearLayout;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.xmlpull.v1.XmlPullParserException;

public class Keyboard extends LinearLayout implements Key.KeyListener {

    private static final String XML_TAG_LAYOUT = "Layout";
    private static final String XML_TAG_KEY = "Key";
    private int mShiftCount = 0;
    private KeyboardListener mKeyboardListener;
    private final List<Key> mKeysWithShiftLabel = new ArrayList<Key>();

    public interface KeyboardListener {
        public void onKeyDown(byte keyCode);

        public void onKeyUp(byte keyCode);
    }

    public Keyboard(Context context, int xmlResourceID) {
        super(context);

        setDefaultLayoutConfiguration();
        loadFromXmlResource(context, xmlResourceID);
    }

    public Keyboard(Context context, AttributeSet attrs, int xmlResourceID) {
        super(context, attrs);

        setDefaultLayoutConfiguration();
        loadFromXmlResource(context, xmlResourceID);
    }

    public Keyboard(Context context, AttributeSet attrs, int defStyle, int xmlResourceID) {
        super(context, attrs, defStyle);

        setDefaultLayoutConfiguration();
        loadFromXmlResource(context, xmlResourceID);
    }

    private void setDefaultLayoutConfiguration() {
        setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        setOrientation(LinearLayout.VERTICAL);
    }

    private void loadFromXmlResource(Context context, int xmlResourceID) {
        try {
            XmlResourceParser xmlParser = context.getResources().getXml(xmlResourceID);

            while (xmlParser.getEventType() != XmlResourceParser.END_DOCUMENT) {
                if (xmlParser.getEventType() == XmlResourceParser.START_TAG
                        && xmlParser.getName().equals(XML_TAG_LAYOUT)) {
                    addView(parseKeyLayout(context, xmlParser));
                }
                xmlParser.next();
            }
            xmlParser.close();
        } catch (XmlPullParserException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private LinearLayout parseKeyLayout(Context context, XmlResourceParser xmlParser)
            throws XmlPullParserException, IOException {
        LinearLayout linearLayout = new LinearLayout(context);
        linearLayout.setLayoutParams(new LayoutParams(
                xmlParser.getAttributeIntValue(null, "width", LayoutParams.MATCH_PARENT),
                xmlParser.getAttributeIntValue(null, "height", 0),
                xmlParser.getAttributeFloatValue(null, "weight", 1.0f)));
        linearLayout.setOrientation(xmlParser.getAttributeIntValue(null, "orientation",
                LinearLayout.HORIZONTAL));

        String tag;
        do {
            xmlParser.next();
            tag = xmlParser.getName();

            if (xmlParser.getEventType() == XmlResourceParser.START_TAG) {
                if (tag.equals(XML_TAG_LAYOUT)) {
                    linearLayout.addView(parseKeyLayout(context, xmlParser));
                } else if (tag.equals(XML_TAG_KEY)) {
                    Key.KeyAttributes attrs = new Key.KeyAttributes();
                    attrs.keyFunction = getStringAttributeValue(xmlParser, "keyFunc", "");
                    attrs.mainLabel = getStringAttributeValue(xmlParser, "keyLabel", "");
                    attrs.shiftLabel = getStringAttributeValue(xmlParser, "shiftLabel", "");
                    attrs.keyCode = (byte) xmlParser.getAttributeIntValue(null, "keyCode", 0);

                    Key key = new Key(context, attrs);
                    key.setLayoutParams(new LayoutParams(
                            xmlParser.getAttributeIntValue(null, "width", 0),
                            xmlParser.getAttributeIntValue(null, "height",
                                    LayoutParams.MATCH_PARENT),
                            xmlParser.getAttributeFloatValue(null, "weight", 1)));
                    key.setVisibility(xmlParser.getAttributeBooleanValue(null, "visible", true) ?
                        VISIBLE : INVISIBLE);
                    key.setKeyListener(this);

                    if (attrs != null && attrs.shiftLabel != null &&
                        attrs.shiftLabel.length() > 0) {
                        mKeysWithShiftLabel.add(key);
                    }

                    linearLayout.addView(key);
                }
            }
        } while (xmlParser.getEventType() != XmlResourceParser.END_TAG
                || !tag.equals(XML_TAG_LAYOUT));

        return linearLayout;
    }

    private String getStringAttributeValue(XmlResourceParser xmlParser, String attributeName,
            String defaultValue) {
        String value = xmlParser.getAttributeValue(null, attributeName);
        if (value != null)
            return value;
        return defaultValue;
    }

    private void updateShiftlabel(boolean shiftOn) {
        for (Key key : mKeysWithShiftLabel) {
            key.setShiftState(shiftOn);
        }
    }

    private void dispatchOnKeyDown(byte keyCode) {
        if (mKeyboardListener != null) {
            mKeyboardListener.onKeyDown(keyCode);
        }
    }

    private void dispatchOnKeyUp(byte keyCode) {
        if (mKeyboardListener != null) {
            mKeyboardListener.onKeyUp(keyCode);
        }
    }

    public void setKeyboardListener(KeyboardListener keyboardListener) {
        mKeyboardListener = keyboardListener;
    }

    @Override
    public void onKeyDown(String keyFunction, byte keyCode) {
        dispatchOnKeyDown(keyCode);
        if (keyFunction.toLowerCase().contains("shift")) {
            if (mShiftCount == 0) {
                updateShiftlabel(true);
            }
            mShiftCount++;
        }
    }

    @Override
    public void onKeyUp(String keyFunction, byte keyCode) {
        dispatchOnKeyUp(keyCode);
        if (mShiftCount > 0 && keyFunction.toLowerCase().contains("shift")) {
            mShiftCount--;
            if (mShiftCount == 0) {
                updateShiftlabel(false);
            }
        }
    }
}
