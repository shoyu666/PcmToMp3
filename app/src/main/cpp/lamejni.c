#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include "libmp3lame/lame.h"

lame_t lame;

#define LOG_TAG "lamejni.c"
#define LOGD(format, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, format, ##args);
#define BUFFER_SIZE 8192
#define be_short(s) ((short) ((unsigned short) (s) << 8) | ((unsigned short) (s) >> 8))

long getFileSize(const char *__path) {
    FILE *pcm = fopen(__path, "rb");
    fseek(pcm, 0, SEEK_END);
    long fileSize = ftell(pcm);
    fclose(pcm);
    return fileSize;
}

int read_samples(FILE *input_file, short *input) {
    int nb_read;
    nb_read = fread(input, 1, sizeof(short), input_file) / sizeof(short);

    int i = 0;
    while (i < nb_read) {
        input[i] = be_short(input[i]);
        i++;
    }

    return nb_read;
}

void Java_com_shoyu666_pcmtomp3_LameJni__destroyEncoder(
        JNIEnv *env, jobject jobj) {
    int res = lame_close(lame);
    LOGD("Deinit returned: %d", res);
}
void Java_com_shoyu666_pcmtomp3_LameJni_initEncoder(JNIEnv *env,
                                                           jobject jobj,
                                                           jint in_num_channels,
                                                           jint in_samplerate,
                                                           jint out_samplerate,
                                                           jint in_brate,
                                                           jint in_mode, jint in_quality) {
    lame = lame_init();

    LOGD("Init parameters:");
    lame_set_num_channels(lame, in_num_channels);
    LOGD("Number of channels: %d", in_num_channels);
    lame_set_in_samplerate(lame, in_samplerate);
    LOGD("Sample rate: %d", in_samplerate);

    lame_set_brate(lame, in_brate);
    LOGD("Bitrate: %d", in_brate);
    lame_set_quality(lame, in_quality);
    LOGD("Quality: %d", in_quality);

    int res = lame_init_params(lame);
//	lame_set_out_samplerate(lame,16);
    LOGD("Init returned: %d", res);
}


jint Java_com_shoyu666_pcmtomp3_LameJni_pcmToMp3(JNIEnv *env,
                                                        jobject thiz,
                                                        jint in_num_channels,
                                                        jint in_samplerate,
                                                        jint out_samplerate,
                                                        jstring in_source_path,
                                                        jstring in_target_path) {


    const char* lameVersion = get_lame_version();
    LOGD("lame version %s",lameVersion);
    //
    jclass jSdkClass = (*env)->GetObjectClass(env, thiz);
    if (jSdkClass == 0) {
        LOGD("Unable to find class");
        return -1;
    }
    //找到需要调用的方法ID
    jmethodID javaCallback = (*env)->GetMethodID(env, jSdkClass,
                                                 "onProgressCallBack", "(JJ)V");


    //
    const char *source_path, *target_path;
    source_path = (*env)->GetStringUTFChars(env, in_source_path, NULL);
    target_path = (*env)->GetStringUTFChars(env, in_target_path, NULL);

    long fileSize = getFileSize(source_path);

    int read, write;
    FILE *pcm = fopen(source_path, "rb");
    FILE *mp3 = fopen(target_path, "wb");

    const int PCM_SIZE = 8192;
    //2
    int shortIntSize = sizeof(short int);
    long size = in_num_channels * shortIntSize;

    short int pcm_buffer[PCM_SIZE * size];


    const int MP3_SIZE = 8192;
    unsigned char mp3_buffer[MP3_SIZE];

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, in_samplerate);
    if (out_samplerate > 0) {
        lame_set_out_samplerate(lame, out_samplerate);
    }
    lame_set_num_channels(lame,in_num_channels);
//    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);
    long readCount = 0;

    LOGD("xxxshortIntSize %d  -- %d", shortIntSize,size)
    do {
        read = fread(pcm_buffer, size, PCM_SIZE, pcm);
        LOGD("fread %d", read)
        readCount = readCount + read * size;
        if (read == 0) {
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        } else {
            if (in_num_channels == 1) {
                write = lame_encode_buffer(lame, pcm_buffer, pcm_buffer, read, mp3_buffer,
                                           MP3_SIZE);
                (*env)->CallVoidMethod(env, thiz, javaCallback, (jlong)fileSize, (jlong)readCount);
                LOGD("lame_encode_buffer 1: %ld -x- %d", (jlong)fileSize, (jlong)readCount);
            } else if (in_num_channels == 2) {
                write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer,
                                                       MP3_SIZE);
                (*env)->CallVoidMethod(env, thiz, javaCallback, (jlong)fileSize, (jlong)readCount);
                LOGD("lame_encode_buffer 2: %ld -x- %d", (jlong)fileSize, (jlong)readCount);
            }
        }
        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);
    lame_mp3_tags_fid(lame,mp3);
    lame_close(lame);
    fclose(mp3);
    fclose(pcm);
    return 0;

}

void Java_com_shoyu666_pcmtomp3_LameJni_encodeFile(JNIEnv *env,
                                                          jobject jobj,
                                                          jstring in_source_path,
                                                          jstring in_target_path) {
    const char *source_path, *target_path;
    source_path = (*env)->GetStringUTFChars(env, in_source_path, NULL);
    target_path = (*env)->GetStringUTFChars(env, in_target_path, NULL);

    FILE *input_file, *output_file;
    input_file = fopen(source_path, "rb");
    output_file = fopen(target_path, "wb");

    short input[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    int nb_read = 0;
    int nb_write = 0;
    int nb_total = 0;

    LOGD("Encoding started");
    while (nb_read = read_samples(input_file, input)) {
        nb_write = lame_encode_buffer(lame, input, input, nb_read, output,
                                      BUFFER_SIZE);
        fwrite(output, nb_write, 1, output_file);
        nb_total += nb_write;
    }
    LOGD("Encoded %d bytes", nb_total);

    nb_write = lame_encode_flush(lame, output, BUFFER_SIZE);
    fwrite(output, nb_write, 1, output_file);
    LOGD("Flushed %d bytes", nb_write);

    fclose(input_file);
    fclose(output_file);
}