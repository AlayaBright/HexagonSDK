package com.example.calculator;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    public static final String RESULT_MESSAGE = "com.example.calculator.MESSAGE";

    // Used to load the stub library 'libcalculator.so' during application startup.
    static {
        System.loadLibrary("calculator");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    /** Called when the user taps the Calculate button */
    public void sendMessage(View view) {
        Intent intent = new Intent(this, DisplayResult.class);
        EditText editText = findViewById(R.id.editText);
        String message;
        try {
            int len = Integer.valueOf(editText.getText().toString());
            int[] vec = new int[len];
            for (int i = 0; i < len; i++) {
                vec[i] = i;
            }
            message = "Result: The sum of " + String.valueOf(len) + " numbers is " + String.valueOf(sum(vec, len));
        } catch (Exception e) {
            message = "Please retry with a valid number !";
        }
        intent.putExtra(RESULT_MESSAGE, message);
        startActivity(intent);
    }

    /**
     * The native method implemented by the 'calculator' native library,
     * which is packaged with this application.
     * sum: returns sum of elements of vector, len being length of vector vec
     */
    public native long sum(int[] vec, int len);
}
