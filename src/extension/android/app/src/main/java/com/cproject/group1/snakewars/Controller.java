package com.cproject.group1.snakewars;

import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

public class Controller extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_controller);
        Context context = getApplicationContext();
        Intent current = getIntent();
        String IP = current.getStringExtra("IP");
        int num = current.getIntExtra("Number", -1);
        if (num == -1) {
            Toast toast = Toast.makeText(context, "Invalid number!", Toast.LENGTH_SHORT);
            toast.show();
            finishActivity(0);
        }
        TextView textView;
        String text = "You are Player " + num;
        textView  = findViewById(R.id.textView);
        textView.setText(text);
    }
}
