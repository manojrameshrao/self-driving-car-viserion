package com.example.pratap.viserion;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;

public class speedometer extends AppCompatActivity {
public static int RIGHT_SENSOR_VAL=30;
    public static int LEFT_SENSOR_VAL=30;
    public static int CENTER_SENSOR_VAL=30;
    public static int REAR_SENSOR_VAL=30;
    Mybluetoothconnector mybluetoothconnector=Mybluetoothconnector.getInstance();
    private TextView speedView;
    private TextView directionView;
    private Button leftbutton;
    private Button centerbutton;
    private Button rightbutton;
    private Button rearbutton;
    private Thread workerThread;
    byte[] readBuffer;
    private int readBufferPosition;
    private ImageView compass;
    private float currentDegree;
    private boolean stopWorker;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_speedometer);
        speedView=(TextView)findViewById(R.id.speed_view);
        directionView=(TextView)findViewById(R.id.direction_view);
        leftbutton=(Button) findViewById(R.id.button_left);
        centerbutton=(Button)findViewById(R.id.button_centre);
        rightbutton=(Button)findViewById(R.id.button_right);
        rearbutton=(Button)findViewById(R.id.button_rear);
        compass=(ImageView)findViewById(R.id.image_compass) ;
        currentDegree=0f;
        stopWorker=false;
        Toast.makeText(getApplicationContext(), "Starting stats", Toast.LENGTH_SHORT).show();
        receiveDataFromModule();
    }

    public void receiveDataFromModule() {
        final Handler handler1 = new Handler();
        final byte delimiter = 10;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        workerThread = new Thread(new Runnable() {
            public void run() {
                  while(!Thread.currentThread().isInterrupted() && !stopWorker) {
                    try {
                        if (mybluetoothconnector.getInputStream() != null) {
                            int bytesAvailable = mybluetoothconnector.getInputStream().available();
                            if (bytesAvailable > 10) {
                                byte[] packetBytes = new byte[bytesAvailable];
                                mybluetoothconnector.getInputStream().read(packetBytes);
                                for (int i = 0; i < bytesAvailable; i++) {
                                    byte b = packetBytes[i];
                                    if (b == delimiter) {
                                        byte[] encodedBytes = new byte[readBufferPosition];
                                        System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                        final String data = new String(encodedBytes, "US-ASCII");
                                        readBufferPosition = 0;

                                        handler1.post(new Runnable() {
                                            public void run() {
                                                if(data.contains("D")) {
                                                    try {
                                                        String dir=data.substring(1);
                                                        directionView.setText(dir + "º");
                                                        float degree_val_t_f = Float.parseFloat(dir);
                                                        RotateAnimation ra = new RotateAnimation(currentDegree, degree_val_t_f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
                                                        ra.setDuration(10);
                                                        ra.setFillAfter(true);
                                                        compass.startAnimation(ra);
                                                        currentDegree = degree_val_t_f;
                                                    } catch (NumberFormatException e) {
                                                    }
                                                }
                                                if(data.contains("S")) {
                                                    String speed=data.substring(1);
                                                    speedView.setText(speed + "mph");

                                                }
                                                if((data.contains("O")) && (data.contains(","))) {
                                                    String sensor_temp=data.substring(1);
                                                    String sensor[]=sensor_temp.split(",");
                                                    int right_sensor=Integer.parseInt(sensor[0]);
                                                    int left_sensor=Integer.parseInt(sensor[1]);
                                                    int centre_sensor=Integer.parseInt(sensor[2]);
                                                    int rear_sensor=Integer.parseInt(sensor[3]);
                                                    if(right_sensor<=RIGHT_SENSOR_VAL){
                                                        rightbutton.setVisibility(View.VISIBLE);
                                                    }
                                                    else{
                                                        rightbutton.setVisibility(View.INVISIBLE);
                                                    }
                                                    if(left_sensor<=LEFT_SENSOR_VAL){
                                                        leftbutton.setVisibility(View.VISIBLE);
                                                    }
                                                    else {
                                                        leftbutton.setVisibility(View.INVISIBLE);
                                                    }
                                                    if(centre_sensor<=CENTER_SENSOR_VAL){
                                                        centerbutton.setVisibility(View.INVISIBLE);
                                                    }
                                                    else {
                                                        centerbutton.setVisibility(View.INVISIBLE);
                                                    }
                                                    if(rear_sensor<=REAR_SENSOR_VAL){
                                                        rearbutton.setVisibility(View.INVISIBLE);
                                                    }
                                                    else {
                                                        rearbutton.setVisibility(View.INVISIBLE);
                                                    }

                                                }
                                            }
                                        });
                                    } else {
                                        readBuffer[readBufferPosition++] = b;
                                    }
                                }
                            }
                        }
                    }
                    catch (IOException ex) {
                    }
                }
            }
        });

        workerThread.start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopWorker=true;
        sendStopRequest();
    }
    void sendStopRequest(){
        try {
            if (mybluetoothconnector.getBluetoothdevice() != null) {
                String message = "SSTAT";
                byte[] send = message.getBytes();
                mybluetoothconnector.getOutputStream().write(send);
                mybluetoothconnector.getOutputStream().flush();

            }
        }
        catch (IOException e) {
            e.printStackTrace();
        } catch (RuntimeException e) {
            e.printStackTrace();
        }
    }


}
