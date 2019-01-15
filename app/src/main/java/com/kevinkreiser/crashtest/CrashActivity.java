package com.kevinkreiser.crashtest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

public class CrashActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_crash);

        //setup segv handler
        handleSegv();
        //cause a segv
        sendSegv();
    }

    /**
     * Sets up signal handler for SIGSEGV which will call the callback function below
     * @return true if the handler was set
     */
    public native boolean handleSegv();

    /**
     * Raises the SIGSEGV signal which will cause the handler to be called
     */
    public native void sendSegv();

    /**
     * A function that the native code will call back when it receives SIGSEGV
     * as an illustration it just logs
     *
     * @param message  The message coming back from c++
     */
    public void callback(String message) {
        Log.e("CrashActivity.callback", message);
    }
}
