#include <jni.h>
#include <string>

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <sys/time.h>

#include "faiss/IndexIVFPQ.h"
#include "faiss/IndexFlat.h"
#include "faiss/index_io.h"

#include "log.h"

int64_t getCurrentMillTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((int64_t) tv.tv_sec * 1000 + (int64_t) tv.tv_usec / 1000);//毫秒
}


#define FEATURE_COUNT 256
extern "C" JNIEXPORT jstring

JNICALL stringFromJNI(JNIEnv *env, jclass clazz, jint number) {
    std::string result = "0";
    std::string name_faiss = "/sdcard/tmp/1.index";
    faiss::IndexFlatL2 *index;


    index = new faiss::IndexFlatL2(FEATURE_COUNT);
    float data[FEATURE_COUNT];//random data
    for (int i = 0; i < FEATURE_COUNT; i++) {
        data[i] = 0.0001f * i + 0.00001f;
    }
    LOGI("index->add。。。");
    for (int i = 0; i < (100); i++) {

        index->add(1, data);
        //Problems may occur in 32-bit
        LOGD("SIZE= %lld", index->ntotal * index->d);
    }
    LOGI("index->add over");


    LOGI("save...");
    //please apply for sdcard permission before writing
    faiss::write_index(index, name_faiss.c_str());
    LOGI("save ");
    delete index;


    LOGI("read ... ");
    faiss::Index *tmp = faiss::read_index(name_faiss.c_str(), faiss::IO_FLAG_MMAP);
    LOGI("read ok");
    //null point check
    index = (faiss::IndexFlatL2 *) tmp;

    LOGI("index->d=%d", index->d);


    float read[FEATURE_COUNT] = {0};
    index->reconstruct(index->ntotal - 1, read);

    for (int i = 0; i < FEATURE_COUNT; i++) {
        LOGI("read[%d] %f %f", i, data[i], read[i]);
    }

    float data2[FEATURE_COUNT];//random data
    for (int i = 0; i < FEATURE_COUNT; i++) {
        data2[i] = 0.0001f * FEATURE_COUNT * 2 + 0.00002f;
    }
    const int64_t destCount = 3;
    int64_t *listIndex = (int64_t *) malloc(sizeof(int64_t) * destCount);
    float *listScore = (float *) malloc(sizeof(float) * destCount);
    LOGI("index->search。。。");
    index->search(1, data2, destCount, listScore, listIndex);
    LOGI("index->search");
    for (int i = 0; i < destCount; i++) {
        LOGI("index->search[%lld]=%f", listIndex[i], listScore[i]);
    }
    free(listIndex);
    free(listScore);

    LOGI("read");
    result = std::to_string(index->ntotal);
    return env->NewStringUTF(result.c_str());
}

#define JNIREG_CLASS_BASE "io/datamachines/faiss/MainActivity"
static JNINativeMethod gMethods_Base[] = {
        {"stringFromJNI", "(I)Ljava/lang/String;", (void *) stringFromJNI},
};

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = (*env).FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    if ((*env).RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


static int registerNatives(JNIEnv *env) {
    if (!registerNativeMethods(env, JNIREG_CLASS_BASE, gMethods_Base,
                               sizeof(gMethods_Base) / sizeof(gMethods_Base[0]))) {
        return JNI_FALSE;
    }


    return JNI_TRUE;
}


JNIEXPORT jint

JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("JNI_OnLoad");
    JNIEnv *env = nullptr;
    jint result = -1;

    if ((*vm).GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    assert(env != nullptr);

    if (!registerNatives(env)) { //注册
        return -1;
    }

    result = JNI_VERSION_1_4;
    return result;

}


JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {


}


