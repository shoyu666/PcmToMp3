package com.shoyu666.pcmtomp3;


import java.io.File;

public class PCMToMp3 {
    public static final String TAG = "PCMToMp3";
    public File pcm;
    public File mp3;
    FormatInfo formatInfo;

    /**
     * @param pcm        pcm file data to convert
     * @param mp3        convert result
     * @param formatInfo pcm data info
     */
    public PCMToMp3(File pcm, File mp3, FormatInfo formatInfo) {
        this.pcm = pcm;
        this.mp3 = mp3;
        this.formatInfo = formatInfo;
    }

    LameJni mp3Tool = new LameJni();

    /**
     * @return 0 if success
     */
    public int pcmToMp3() {
        int result;
        try {
            String mp3Path = pcm.getAbsolutePath().replace(".pcm", ".mp3");
            result = mp3Tool.pcmToMp3(formatInfo.numChannels, formatInfo.sampleRate, formatInfo.sampleRate/*resample if need*/, pcm.getAbsolutePath(), mp3Path);
            if (BuildConfig.DEBUG) {
                String mp3PathDebug = pcm.getAbsolutePath().replace(".pcm", "_debug.mp3");
                result = mp3Tool.pcmToMp3(formatInfo.numChannels, formatInfo.sampleRate, 0, pcm.getAbsolutePath(), mp3PathDebug);
            }
        } catch (Exception e) {
            result = -2;
        }
        return result;
    }
}
