#include <android/log.h>
#include <jni.h>
#include <string.h>
#include <signal.h>
#include <string>

//globals persisting between calls from javaland
static JavaVM* vm = NULL;
static jobject activity = NULL;
static jmethodID callback = NULL;

//gets called first when a signal is sent to the running pid
static void signal_handler(int signal, siginfo_t*, void*) {
    //get an env so we can call back to java
    JNIEnv* env;
    if(vm->AttachCurrentThread(&env, NULL) != JNI_OK)
        return;

    //call back to java with a message
    __android_log_print(ANDROID_LOG_ERROR, "native-lib.signal_handler", "Calling with signal %d", signal);
    std::string message = "Got signal " + std::to_string(signal);
    jstring msg = env->NewStringUTF(message.c_str());
    env->CallVoidMethod(activity, callback, msg);
    __android_log_print(ANDROID_LOG_ERROR, "native-lib.signal_handler", "Called with signal %d", signal);
}

extern "C" JNIEXPORT void JNICALL
Java_com_kevinkreiser_crashtest_CrashActivity_sendSegv(JNIEnv*, jobject) {
    raise(SIGSEGV);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_kevinkreiser_crashtest_CrashActivity_handleSegv(JNIEnv* env, jobject obj) {
    //get java hooks we need to make the callback
    env->GetJavaVM(&vm);
    activity = env->NewGlobalRef(obj);
    if (!activity)
        return false;
    jclass activity_class = env->GetObjectClass(activity);
    if (!activity_class)
        return false;
    callback = env->GetMethodID(activity_class, "callback", "(Ljava/lang/String;)V");
    if (!callback)
        return false;

    //try calling back to java with a message
    jstring message = env->NewStringUTF("No signal yet");
    env->CallVoidMethod(activity, callback, message);

    //register for SIGSEGV
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_sigaction = signal_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &action, NULL);

    return true;
}
