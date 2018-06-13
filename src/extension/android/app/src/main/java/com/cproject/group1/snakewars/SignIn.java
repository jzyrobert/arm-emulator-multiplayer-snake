package com.cproject.group1.snakewars;

import android.content.Context;
import android.content.Intent;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLConnection;

public class SignIn extends AppCompatActivity {
    
    public static String IP;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sign_in);
    }

    public void sendMessage(View view) {
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);
        Intent intent = new Intent(this, Controller.class);
        EditText editText = findViewById(R.id.editText);
        IP = editText.getText().toString();
        Context context = getApplicationContext();
        CharSequence text = "";
        int duration = Toast.LENGTH_SHORT;

        if (IP.equals("")) {
            text = "No IP Address entered!";
            Toast toast = Toast.makeText(context, text, duration);
            toast.show();
        } else {
            if (IP.indexOf(':') > 0) {
                boolean tooMany = false;
                try {
                    IP = "http://" + IP + "/";
                    Toast toast = Toast.makeText(context, "Connecting to: " + IP, duration);
                    //toast.show();
                    URL host = new URL(IP);
                    URLConnection connection = host.openConnection();
                    connection.connect();
                    BufferedReader in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
                    String input;
                    boolean correctSite = false;
                    while (((input = in.readLine()) != null)) {

                        if (input.contains("Too many")) {
                            tooMany = true;
                            correctSite = false;
                            toast = Toast.makeText(context, "Too many players! You cannot join this round", duration);
                            toast.show();
                        }
                        if (!tooMany && input.contains("Snake Wars")) {
                            correctSite = true;
                            text = "Connection successfull";
                            toast = Toast.makeText(context, text, duration);
                            //toast.show();
                        }
                        if (input.contains("Your number is")) {
                            input = input.replaceAll("\\D+","");
                            intent.putExtra("Number", Integer.parseInt(input));
                            break;
                        }
                    }
                    in.close();
                    if (correctSite) {
                        startActivity(intent);
                    } else if(!tooMany){
                        text = "Incorrect site!";
                        toast = Toast.makeText(context, text, duration);
                        toast.show();
                    }
                } catch (IOException e) {
                    if(!tooMany){
                        text = "Invalid connection!";
                        Toast toast = Toast.makeText(context, text, duration);
                        toast.show();
                    }
                }
            } else {
                text = "No port specified!";
                Toast toast = Toast.makeText(context, text, duration);
                toast.show();
            }
        }
    }
}
