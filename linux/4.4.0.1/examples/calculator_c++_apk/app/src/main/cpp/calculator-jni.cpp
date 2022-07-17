#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <stddef/AEEStdErr.h>
#include "rpcmem.h"
#include "remote.h"
#include "calculator.h"
#include "os_defines.h"

using namespace std;
class Calculator {
public:
    int64 result;
    int nErr;
}; // object;

const char *TAG = "calculator";

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_calculator_MainActivity_sum(JNIEnv *env, jobject instance,
                                             jintArray vec_, jint len) {
    remote_handle64 handle;
    char* uri;
    int alloc_len = 0;

    jint *vec = env->GetIntArrayElements(vec_, NULL);
    Calculator *object = new Calculator();
    object->result = 0;
    object->nErr = 0;
    int *test = 0;
    int64 result = 0;

    rpcmem_init();

    alloc_len = sizeof(*test) * len;
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Allocating %d bytes", alloc_len);
    if (0 == (test = (int*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, alloc_len))) {
        object->nErr = 1;
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "rpcmem_alloc failed with nErr = 0x%x",
                            object->nErr);
        goto bail;
    }
    for (int i=0; i<len; i++) {
        test[i] = vec[i];
    }

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Opening handle");
    uri = (char*)calculator_URI "&_dom=cdsp";
    if(remote_session_control) {
        struct remote_rpc_control_unsigned_module data;
        data.enable = 1;
        data.domain = CDSP_DOMAIN_ID;
        if (0 != (object->nErr = remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE, (void*)&data, sizeof(data)))) {
            __android_log_print(ANDROID_LOG_DEBUG, TAG, "remote_session_control failed for CDSP, returned 0x%x", object->nErr);
            goto bail;
        }
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Requested signature-free dynamic module offload");
    } else {
        object->nErr = AEE_EUNSUPPORTED;
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "remote_session_control interface is not supported on this device, returned 0x%x", object->nErr);
        goto bail;
    }
    object->nErr = calculator_open(uri, &handle);
    if (object->nErr != 0) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Handle open failed, returned 0x%x", object->nErr);
        goto bail;
    }

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "Call calculator_sum");
    if (0 == (object->nErr = calculator_sum(handle, test, len, &object->result))) {
        result = object->result;
    }
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "calculator_sum call returned err 0x%x, result %lld", object->nErr, object->result);

bail:
    if (handle) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Closing handle");
        object->nErr = calculator_close(handle);
        if (object->nErr != 0) {
            __android_log_print(ANDROID_LOG_DEBUG, TAG, "Handle close failed, returned 0x%x", object->nErr);
        }
    }
    if (test) {
        rpcmem_free(test);
    }
    if (object) {
        delete object;
    }
    rpcmem_deinit();
    env->ReleaseIntArrayElements(vec_, vec, 0);
    return result;
}
