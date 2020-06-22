package io.datamachines.faiss;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("faiss");
    }

    Button StartTest;
    TextView tv;
    String data;
    int currentValue;

    void StartTesting() {
        File file = new File("/sdcard/tmp/");
        if (!file.exists()) {
            file.mkdirs();
        }
        StartTest.setEnabled(false);
        tv.setText("test...");
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    data = stringFromJNI(currentValue);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            UpdateGUI();
                        }
                    });

                } catch (Exception e) {

                }
            }
        }).start();

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        tv.setText("Testing for now - Build time");


        StartTest = findViewById(R.id.button);
        StartTest.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                StartTesting();
            }
        });

    }

    public void UpdateGUI() {
        tv.setText(data);
        StartTest.setEnabled(true);
    }


    public static native String stringFromJNI(int a);

}
