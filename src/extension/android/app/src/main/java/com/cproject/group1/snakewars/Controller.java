package com.cproject.group1.snakewars;

import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;

public class Controller extends AppCompatActivity {

    private Context context;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_controller);
        context = getApplicationContext();
        Intent current = getIntent();
        int num = current.getIntExtra("Number", -1);
        if (num == -1) {
            Toast toast = Toast.makeText(context, "Invalid number!", Toast.LENGTH_SHORT);
            toast.show();
            finish();
        }
        TextView textView;
        String text = "You are Player " + num;
        textView  = findViewById(R.id.textView);
        textView.setText(text);
    }

    public void sendCommand(View view) {
        Button b = (Button)view;
        String command = b.getText().toString();
        try {
            URL url = new URL(SignIn.IP);
            HttpURLConnection http = (HttpURLConnection) url.openConnection();
            command = URLEncoder.encode("Direction", "UTF-8") + "=" + URLEncoder.encode(command, "UTF-8");
            byte[] output = command.getBytes(StandardCharsets.UTF_8);
            int length = output.length;
            http.setRequestMethod("POST");
            http.setDoOutput(true);
            http.setFixedLengthStreamingMode(length);
            http.setRequestProperty("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
            http.connect();
            OutputStream os = http.getOutputStream();
            os.write(output);
            os.flush();
            os.close();
        } catch (ProtocolException e) {
            Toast toast = Toast.makeText(context, "Could not initialise Post request!", Toast.LENGTH_SHORT);
            toast.show();
            finish();
        } catch (UnsupportedEncodingException e) {
            Toast toast = Toast.makeText(context, "Error encoding command!", Toast.LENGTH_SHORT);
            toast.show();
            finish();
        } catch (IOException e) {
            Toast toast = Toast.makeText(context, "Error: Connection failed!", Toast.LENGTH_SHORT);
            toast.show();
            finish();
        }
    }

    @Override
    public void onPause(){
        finish();
        super.onPause();
    }
}
