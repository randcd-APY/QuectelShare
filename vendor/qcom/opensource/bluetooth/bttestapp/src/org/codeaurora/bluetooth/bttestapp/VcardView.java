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


package org.codeaurora.bluetooth.bttestapp;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.android.vcard.VCardEntry;
import org.codeaurora.bluetooth.bttestapp.R;

public class VcardView extends LinearLayout {

    private final OnLongClickListener mLongClickListener = new OnLongClickListener() {

        @Override
        public boolean onLongClick(View v) {
            String txt = ((TextView) v).getText().toString();

            ClipboardManager clipboard = (ClipboardManager) getContext().getSystemService(
                    Context.CLIPBOARD_SERVICE);

            clipboard.setPrimaryClip(ClipData.newPlainText(txt, txt));

            Toast.makeText(getContext(), "copied: " + txt, Toast.LENGTH_SHORT).show();

            return true;
        }
    };

    public VcardView(Context context) {
        super(context);
    }

    public VcardView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public VcardView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    private static VCardEntry mVCardEntry = null;

    public void setVCardEntry(VCardEntry vcard) {
        mVCardEntry = vcard;
        populateUi();
    }

    private void addToContainer(ViewGroup cont, String str) {
        TextView txt = new TextView(getContext());
        txt.setText(str);
        txt.setTextAppearance(getContext(), android.R.style.TextAppearance_Large);
        txt.setPadding(3, 3, 3, 3);
        txt.setOnLongClickListener(mLongClickListener);
        cont.addView(txt);
    }

    private void populateUi() {
        removeAllViewsInLayout();

        LayoutInflater inflater = (LayoutInflater) mContext.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        View view = inflater.inflate(R.layout.vcard_view, this);

        ((ImageView) view.findViewById(R.id.contact_photo))
                .setImageResource(R.drawable.ic_contact_picture);

        if (mVCardEntry.getPhotoList() != null && mVCardEntry.getPhotoList().size() > 0) {
            byte[] data = mVCardEntry.getPhotoList().get(0).getBytes();
            try {
                if (data != null) {
                    Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                    ((ImageView) view.findViewById(R.id.contact_photo)).setImageBitmap(bmp);
                }
            } catch (IllegalArgumentException e) {
                // don't care, will just show dummy image
            }
        }

        ((TextView) view.findViewById(R.id.contact_display_name)).setText(mVCardEntry
                .getDisplayName());

        if (mVCardEntry.getPhoneList() != null) {
            LinearLayout cont = (LinearLayout) view.findViewById(R.id.contact_phonelist);

            for (VCardEntry.PhoneData phone : mVCardEntry.getPhoneList()) {
                addToContainer(cont, phone.getNumber());
            }
        }

        if (mVCardEntry.getEmailList() != null) {
            LinearLayout cont = (LinearLayout) view.findViewById(R.id.contact_emaillist);

            for (VCardEntry.EmailData email : mVCardEntry.getEmailList()) {
                addToContainer(cont, email.getAddress());
            }
        }
    }
}
