package com.example.pratap.viserion;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;

/**
 * Created by pratap on 05-11-2017.
 */

class Mybluetoothconnector extends Application {

    private static final Mybluetoothconnector ourInstance = new Mybluetoothconnector();
    private BluetoothDevice bluetoothdevice;
    private BluetoothSocket bluetoothSocket;
    private OutputStream outputStream;
    private InputStream inputStream;

    static Mybluetoothconnector getInstance() {
        return ourInstance;
    }

    private Mybluetoothconnector() {

    }

    BluetoothSocket getBluetoothSocket(){
        return bluetoothSocket;
    }
    BluetoothDevice getBluetoothdevice(){
        return bluetoothdevice;
    }
    InputStream getInputStream(){
        return inputStream;
    }

    OutputStream getOutputStream(){
        return outputStream;
    }
    void setBluetoothSocket(BluetoothSocket bluetoothSocket){
        this.bluetoothSocket=bluetoothSocket;
    }
    void setBluetoothdevice(BluetoothDevice bluetoothdevice){
        this.bluetoothdevice= bluetoothdevice;
    }
    void setInputStream(InputStream inputStream){
        this.inputStream=inputStream;
    }

    void setOutputStream(OutputStream outputStream){
        this.outputStream=outputStream;
    }

}
