package com.robot.wireless.fouliex.androidwirelessrobot;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;

public class MyVideo extends Activity implements View.OnTouchListener {
    private  final int STOP_COMMAND= 0x00 ;
    private  final int FORWARD_COMMAND =0x01;
    private  final int BACKWARD_COMMAND = 0x02;
    private  final int LEFT_COMMAND = 0x03;
    private final int  RIGHT_COMMAND = 0x04;
    URL videoUrl;
    public static String CameraIp;
    public static String CtrlIp;
    public static String CtrlPort;
    MySurfaceView r;
    private Socket socket;
    OutputStream socketWriter;

    boolean forward = false;
    boolean backward = false;
    boolean left = false;
    boolean right = false;
    boolean stop = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.myvideo);
        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }
        r = (MySurfaceView) findViewById(R.id.mySurfaceViewVideo);
        Intent intent = getIntent();
        CameraIp = intent.getStringExtra("CameraIp");
        CtrlIp = intent.getStringExtra("ControlUrl");
        CtrlPort = intent.getStringExtra("Port");
        Log.d("wifirobot", "control is :++++" + CtrlIp);
        Log.d("wifirobot", "CtrlPort is :++++" + CtrlPort);
        r.GetCameraIP(CameraIp);
        InitSocket();
        setMotorCommandsOnTouchListener();
    }

    /**
     *
     */
    private void setMotorCommandsOnTouchListener() {
        findViewById(R.id.button_forward).setOnTouchListener(this);
        findViewById(R.id.button_backward).setOnTouchListener(this);
        findViewById(R.id.button_left).setOnTouchListener(this);
        findViewById(R.id.button_right).setOnTouchListener(this);
        findViewById(R.id.button_stop).setOnTouchListener(this);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (v.getId()) {
            case R.id.button_forward:
                if (event.getAction() == MotionEvent.ACTION_DOWN) forward = true;
                else if (event.getAction() == MotionEvent.ACTION_UP) forward = false;
                break;
            case R.id.button_backward:
                if (event.getAction() == MotionEvent.ACTION_DOWN) backward = true;
                else if (event.getAction() == MotionEvent.ACTION_UP) backward = false;
                break;
            case R.id.button_left:
                if (event.getAction() == MotionEvent.ACTION_DOWN) left = true;
                else if (event.getAction() == MotionEvent.ACTION_UP) left = false;
                break;
            case R.id.button_right:
                if (event.getAction() == MotionEvent.ACTION_DOWN) right = true;
                else if (event.getAction() == MotionEvent.ACTION_UP) right = false;
                break;
            case R.id.button_stop:
                if (event.getAction() == MotionEvent.ACTION_DOWN) stop = true;
                else if (event.getAction() == MotionEvent.ACTION_UP) stop = false;
                break;
        }
        if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction()==MotionEvent.ACTION_UP)
            command();
        return false;
    }

    private void command() {
        if (stop) {
            send(STOP_COMMAND);
        } else if (forward) {
            if (left && !right) send(LEFT_COMMAND);
            else if (right) send(RIGHT_COMMAND);
            else if (!backward) send(FORWARD_COMMAND);
            else send(STOP_COMMAND);
        } else if (backward) {
            if (!forward) send(BACKWARD_COMMAND);
            else send(STOP_COMMAND);
        } else if (left && !right) send(LEFT_COMMAND);
        else if (right) send(RIGHT_COMMAND);
        else send(STOP_COMMAND);
    }
    private void send(int command){
        try {
            socketWriter.write(new byte[]{(byte) 0xff, (byte) 0x00, (byte) command, (byte) 0x00, (byte) 0xff});
            socketWriter.flush();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }

    public void InitSocket() {
        new Thread(new Runnable() {
            public void run() {
                try {
                    socket = new Socket(InetAddress.getByName(CtrlIp), Integer.parseInt(CtrlPort));
                } catch (UnknownHostException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                try {
                    socketWriter = socket.getOutputStream();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }


            }
        }).start();
    }

    public void onDestroy() {
        super.onDestroy();

    }

    private long exitTime = 0;

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getAction() == KeyEvent.ACTION_DOWN) {

            if ((System.currentTimeMillis() - exitTime) > 2500) {
                exitTime = System.currentTimeMillis();
            } else {
                finish();
                System.exit(0);
            }

            return true;
        }
        return super.onKeyDown(keyCode, event);
    }


}


