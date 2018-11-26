package com.shoyu666.pcmtomp3

import android.media.MediaFormat
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import java.io.File

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        var pcmFile = File("you pcm file")
        var mp3File = File("mp3 file to save")
        //formatInfo pcm file info
        val info =MediaFormat()
        info.setInteger(MediaFormat.KEY_SAMPLE_RATE,16000)
        info.setInteger(MediaFormat.KEY_CHANNEL_COUNT,2)
        var formatInfo = FormatInfo(info)
        var pcmTest = PCMToMp3(pcmFile, mp3File, formatInfo)
        pcmTest.pcmToMp3()
    }
}
