//
// Created by luojinlong on 2020/5/28.
//

#ifndef ANDROID_FAISS_LOG_H
#define ANDROID_FAISS_LOG_H

#include <android/log.h>

#define TAG "faiss"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#endif //ANDROID_FAISS_LOG_H
