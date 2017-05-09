package com.robot.wireless.fouliex.androidwirelessrobot;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    EditText CameraIP,ControlIP,Port;
    Button Button_go;
    String videoUrl,controlUrl,port;
    public static String CameraIp;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        CameraIP = (EditText) findViewById(R.id.editIP);
        ControlIP = (EditText) findViewById(R.id.ip);
        Port = (EditText) findViewById(R.id.port);

        Button_go = (Button) findViewById(R.id.button_go);

        videoUrl = CameraIP.getText().toString();
        controlUrl = ControlIP.getText().toString();
        port = Port.getText().toString();

        Button_go.requestFocusFromTouch();


        Button_go.setOnClickListener(new Button.OnClickListener()
        {
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.putExtra("CameraIp", videoUrl);
                intent.putExtra("ControlUrl", controlUrl);
                intent.putExtra("Port", port);
                intent.putExtra("Is_Scale", true);
                intent.setClass(MainActivity.this, MyVideo.class);
                MainActivity.this.startActivity(intent);
                finish();
                //System.exit(0);
            }
        });

    }

    /**
     *
     */
    private long exitTime = 0;
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if(keyCode == KeyEvent.KEYCODE_BACK && event.getAction() == KeyEvent.ACTION_DOWN)
        {

            if((System.currentTimeMillis()-exitTime) > 2000)
            {
                exitTime = System.currentTimeMillis();
            }
            else
            {
                finish();
                System.exit(0);
            }

            return true;
        }
        return super.onKeyDown(keyCode, event);
    }





}


