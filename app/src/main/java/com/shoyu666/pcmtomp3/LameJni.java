package com.shoyu666.pcmtomp3;


public class LameJni {

    public static final String TAG="LameJni";

    static {
        System.loadLibrary("lamejni");
    }

    public native int pcmToMp3(int numChannels, int sampleRate, int out_samplerate, String sourcePath, String targetPath);
}
