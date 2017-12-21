package com.cmpe.pratap.viserion;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.speech.tts.TextToSpeech;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.Locale;

public class speedometer extends AppCompatActivity implements SensorEventListener {
public static int RIGHT_SENSOR_VAL=30;
    public static int LEFT_SENSOR_VAL=30;
    public static int CENTER_SENSOR_VAL=35;
    public static int REAR_SENSOR_VAL=30;
    public static String STOP_STAT="SSTAT";
    Mybluetoothconnector mybluetoothconnector=Mybluetoothconnector.getInstance();
    private TextView speedView;
    private TextView directionView;
    private TextView sensorView;
    private Button leftbutton;
    private Button centerbutton;
    private Button rightbutton;
    private Button rearbutton;
    private Thread workerThread;
    byte[] readBuffer;
    private int readBufferPosition;
    private ImageView compass;
    private ImageView compass_dir;
    private float currentDegree;
    private float compasscurrentDegree;
    private boolean stopWorker;
    private float degree_val_t_f;
    private float degree_val_t_f_temp;
    private SensorManager mSensorManager;
    private boolean change;
    private float change_val;
    private TextView locationView;
    private TextToSpeech texttospeech;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mSensorManager= (SensorManager) getSystemService(SENSOR_SERVICE);
        setContentView(R.layout.activity_speedometer);
        speedView=(TextView)findViewById(R.id.speed_view);
        locationView=(TextView)findViewById(R.id.location_view);
        sensorView=(TextView)findViewById(R.id.allsensorValues);
        directionView=(TextView)findViewById(R.id.direction_view);
        leftbutton=(Button) findViewById(R.id.button_left);
        centerbutton=(Button)findViewById(R.id.button_centre);
        rightbutton=(Button)findViewById(R.id.button_right);
        rearbutton=(Button)findViewById(R.id.button_rear);
        compass=(ImageView)findViewById(R.id.image_compass) ;
        compass_dir=(ImageView)findViewById( R.id.image_compass_dir);
        currentDegree=0f;
        compasscurrentDegree=0f;
        degree_val_t_f=0f;
        degree_val_t_f_temp=0f;
        stopWorker=false;
        Toast.makeText(getApplicationContext(), "Starting stats", Toast.LENGTH_SHORT).show();
        receiveDataFromModule();
        texttospeech= new TextToSpeech(getApplicationContext(), new TextToSpeech.OnInitListener(){
            @Override
            public void onInit(int status) {
                if(status != TextToSpeech.ERROR) {
                    texttospeech.setLanguage(Locale.US);
                }
            }
        });

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
                                                        degree_val_t_f_temp = Float.parseFloat(dir);
                                                        if(degree_val_t_f_temp!=degree_val_t_f){
                                                            degree_val_t_f=degree_val_t_f_temp;
                                                            degree_val_t_f=compasscurrentDegree+degree_val_t_f;
                                                            RotateAnimation ra = new RotateAnimation(currentDegree, degree_val_t_f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
                                                            ra.setDuration(25);
                                                            ra.setFillAfter(true);
                                                            compass.startAnimation(ra);
                                                            currentDegree = degree_val_t_f;

                                                        }
                                                       } catch (NumberFormatException e) {
                                                    }
                                                }
                                                if(data.contains("S")) {
                                                    String speed=data.substring(1);
                                                    speedView.setText(speed + "kph");

                                                }
                                                if((data.contains("O")) && (data.contains(","))) {
                                                    String sensor_temp=data.substring(1);
                                                    String sensor[]=sensor_temp.split(",");
                                                    int right_sensor=Integer.parseInt(sensor[0]);
                                                    int centre_sensor=Integer.parseInt(sensor[1]);
                                                    int left_sensor=Integer.parseInt(sensor[2]);

                                                    String rear_sen=sensor[3].trim();
                                                    int rear_sensor=Integer.parseInt(rear_sen  );
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
                                                        centerbutton.setVisibility(View.VISIBLE);
                                                    }
                                                    else {
                                                        centerbutton.setVisibility(View.INVISIBLE);
                                                    }
                                                    if(rear_sensor<=REAR_SENSOR_VAL){
                                                        rearbutton.setVisibility(View.VISIBLE);
                                                    }
                                                    else {
                                                        rearbutton.setVisibility(View.INVISIBLE);
                                                    }

                                                }
                                                if((data.contains("C")) && (data.contains(","))){
                                                    String loc=data.substring(1);
                                                    locationView.setText(loc);

                                                }
                                                if(data.contains("B") ){
                                                    String loc=data.substring(1);
                                                    sensorView.setText(loc);

                                                }
                                                if(data.contains("R")){
                                                    String reached="YOU Are at your Destination";
                                                    texttospeech.speak(reached, TextToSpeech.QUEUE_FLUSH, null);
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
        Toast.makeText(getApplicationContext(), "Destroying", Toast.LENGTH_SHORT).show();

    }
    void sendStopRequest(){
        try {
            if (mybluetoothconnector.getBluetoothdevice() != null) {
                String message = STOP_STAT;
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


    @Override
    protected void onResume() {
        super.onResume();

        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),
                SensorManager.SENSOR_DELAY_GAME);
    }
    @Override
    protected void onPause() {
        super.onPause();
        mSensorManager.unregisterListener(this);
    }


    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {

        float degree = Math.round(sensorEvent.values[0]);
        if(degree==(0-compasscurrentDegree)){
            change=true;
            change_val=degree+compasscurrentDegree;

        }
        RotateAnimation ra_compass_dir = new RotateAnimation(
                compasscurrentDegree,
                        -degree,
                Animation.RELATIVE_TO_SELF, 0.5f,
                Animation.RELATIVE_TO_SELF,
        0.5f);

        ra_compass_dir.setDuration(10);
        ra_compass_dir.setFillAfter(true);

        compass_dir.startAnimation(ra_compass_dir);
        compasscurrentDegree = -degree;
        if(change) {
            degree_val_t_f = compasscurrentDegree - change_val;
            RotateAnimation ra = new RotateAnimation(currentDegree, degree_val_t_f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
            ra.setDuration(25);
            ra.setFillAfter(true);
            compass.startAnimation(ra);
            currentDegree = degree_val_t_f;
change=false;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }

}
