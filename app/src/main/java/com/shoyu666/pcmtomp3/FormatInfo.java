package com.shoyu666.pcmtomp3;

import android.media.AudioFormat;
import android.media.MediaFormat;

import java.io.Serializable;

public class FormatInfo implements Serializable {

    public int sampleRate;
    public int numChannels;
    public int pcm_encoding;

    public FormatInfo(MediaFormat format) {
        sampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);
        numChannels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
        if (format.containsKey(MediaFormat.KEY_PCM_ENCODING)) {
            pcm_encoding = format.getInteger(MediaFormat.KEY_PCM_ENCODING);
        } else {
            pcm_encoding = AudioFormat.ENCODING_PCM_16BIT;
        }
    }

    @Override
    public String toString() {
        return "FormatInfo{" +
                "sampleRate=" + sampleRate +
                ", numChannels=" + numChannels +
                ", pcm_encoding=" + pcm_encoding +
                '}';
    }
}
