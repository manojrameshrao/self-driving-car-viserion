package com.example.pratap.viserion;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationManager;
import android.os.Build;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.support.v4.app.NotificationManagerCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.NotificationCompat;
import android.support.v7.widget.SwitchCompat;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.GoogleMapOptions;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import java.io.IOException;
import java.nio.DoubleBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback,
        GoogleApiClient.OnConnectionFailedListener,
        GoogleMap.OnMarkerClickListener,
        GoogleMap.OnMarkerDragListener, GoogleApiClient.ConnectionCallbacks{
    public static final int MY_PERMISSIONS_REQUEST_LOCATION = 99;
    public static final int My_BLUETOOTH_REQUEST_ACCESS = 98;
    public static final String REQUEST_STATS = "STATS";
    public static final String REQUEST_CAR_LOCATION = "POINT";
    public static final String REQUEST_CAR_To_START = "START";
    public static final String REQUEST_CAR_To_STOP = "CSTOP";
    private String DEVICE_ADDRESS = "98:D3:32:20:FE:1C";
    private GoogleMap mMap;

    Mybluetoothconnector mybluetoothconnector = Mybluetoothconnector.getInstance();
    private GoogleApiClient mGoogleApiClient;
    private  Location mLastLocation;
    private Marker mCurrLocationMarker;
    private  LocationRequest mLocationRequest;
    private  double latitude, longitude;
    private double end_latitude, end_longitude;
    private boolean bluetoothDeviceFound;
    int countt = 0;
    private boolean checking = false;
    private BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    private Thread receiveLocationThread;
    private Thread connectionThread;
    byte[] readBuffer;
    private int readBufferPosition;
    private List<Double> directionPointsTobeSentonBluetooth = new ArrayList<Double>();
    private List<Double> testgpspoints = new ArrayList<>();
    private TextView datatextView;
    private  boolean stopReceiving,stopConnecting;
    private  SwitchCompat my_connection_switch;
    private Button sendcoordinate;
    //tesst tt;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        end_latitude = 0;
        end_longitude = 0;
        bluetoothDeviceFound = false;
        countt = 1;
        my_connection_switch=(SwitchCompat) findViewById(R.id.connect_switch);
        datatextView = (TextView) findViewById(R.id.dataidstr);
        sendcoordinate=(Button) findViewById(R.id.Send_data);
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            checkLocationPermission();
            checkBluetoothPermission();
        }
        if (!CheckGooglePlayServices()) {
            Log.d("onCreate", "Finishing test case since Google Play Services are not available");
            finish();
        } else {
            Log.d("onCreate", "Google Play Services available.");
        }

//bluetooth comes here
        if (mBluetoothAdapter == null) {

            Toast.makeText(getApplicationContext(), "This Device does'nt Support Bluetooth", Toast.LENGTH_SHORT).show();

        }
        if (!mBluetoothAdapter.isEnabled()) {
            Intent BluetootIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(BluetootIntent, My_BLUETOOTH_REQUEST_ACCESS);
            //getPairedBluetoothDevices();
        }
        try {

            if (mybluetoothconnector.getBluetoothdevice() != null) {
                if (!mBluetoothAdapter.isDiscovering()) {
                    mBluetoothAdapter.startDiscovery();
                }

                 } else {
                Toast.makeText(getApplicationContext(), "Please pair a Bluetooth Device to send", Toast.LENGTH_SHORT).show();
            }
        } catch (RuntimeException e) {
            e.printStackTrace();
        }


        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }
                public void initial_car_point(){
                    directionPointsTobeSentonBluetooth.clear();
                    mMap.clear();
                    end_latitude = 0;
                    end_longitude = 0;
                    checking=false;
                    sendcoordinate.setClickable(false);
                    if (latitude != 0 && longitude != 0) {
                        LatLng latLng = new LatLng(latitude, longitude);
                        MarkerOptions markerOptions = new MarkerOptions();
                        markerOptions.position(latLng);
                        markerOptions.draggable(true);
                        markerOptions.title("Drag Where You Want to be");
                        markerOptions.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_MAGENTA));
                        mCurrLocationMarker = mMap.addMarker(markerOptions);
                        mMap.moveCamera(CameraUpdateFactory.newLatLng(latLng));
                        mMap.animateCamera(CameraUpdateFactory.zoomTo(15));
                    }
                }
    public void change_destination (View view) {

        if (latitude != 0 && longitude != 0) {
            initial_car_point();
            Toast.makeText(MapsActivity.this, "Car Location", Toast.LENGTH_LONG).show();
        }
        else {
            Toast.makeText(MapsActivity.this, "Car Location Unknown", Toast.LENGTH_LONG).show();
        }
    }


    public void send_coordinates (View view){
        try {

            if (mybluetoothconnector.getBluetoothdevice() != null) {
                if (checking) {
                    String message = "";
                    int count = 1;
                    for (Double lg : directionPointsTobeSentonBluetooth) {
                        if (count % 2 != 0) {
                            message = message + lg;
                        } else {
                            message = message + "," + lg;
                            byte[] send = message.getBytes();
                            mybluetoothconnector.getOutputStream().write(send);
                            mybluetoothconnector.getOutputStream().flush();
                            message = "";
                            for (int i = 0; i < 1000; i++) {

                            }
                            if (count >= 24) {
                                for (int i = 0; i < 5000; i++) {

                                }

                            }

                        }
                        count++;
                    }
                    Toast.makeText(getApplicationContext(), "sending...", Toast.LENGTH_SHORT).show();
                }
            } else {
                Toast.makeText(getApplicationContext(), "Please pair for a Bluetooth Device to transmit", Toast.LENGTH_SHORT).show();
            }
        } catch (IOException e) {
            e.printStackTrace();
        } catch (RuntimeException e) {
            e.printStackTrace();
        }
    }
    public void statinfo(View view) {
        sendReceiveMsgRequest(REQUEST_STATS);
        Intent intent = new Intent(MapsActivity.this, speedometer.class);
        startActivity(intent);
    }

    public void recieve_car_cordinates(View view) {
        sendReceiveMsgRequest(REQUEST_CAR_LOCATION);
        receiveCarLocation();

    }
    public void stopcar(View view) {
        sendReceiveMsgRequest(REQUEST_CAR_To_STOP);
    }

    public void startcar(View view) {
        sendReceiveMsgRequest(REQUEST_CAR_To_START);
    }


    private String appendzeros(int n) {
        String Zeros = "";
        for (int i = 1; i < n - 1; i++) {
            Zeros = Zeros + "0";
        }
        Zeros = Zeros + "1";
        return Zeros;
    }

    void sendReceiveMsgRequest(String message){
        try {
            if (mybluetoothconnector.getBluetoothdevice() != null) {
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

    private String getRouteDirectionsUrl() {
        StringBuilder googleRouteDirectionsUrl = new StringBuilder("https://maps.googleapis.com/maps/api/directions/json?");
        googleRouteDirectionsUrl.append("origin=" + latitude + "," + longitude);
        googleRouteDirectionsUrl.append("&destination=" + end_latitude + "," + end_longitude);
        googleRouteDirectionsUrl.append("&mode=walking");
        googleRouteDirectionsUrl.append("&key=" + "AIzaSyDxKBS6PNcFk_4NrDkU0Re0agYysB*****");

        return googleRouteDirectionsUrl.toString();
    }


    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this,
                    Manifest.permission.ACCESS_FINE_LOCATION)
                    == PackageManager.PERMISSION_GRANTED) {
                buildGoogleApiClient();
                mMap.setMyLocationEnabled(true);
            }
        } else {
            buildGoogleApiClient();
            mMap.setMyLocationEnabled(true);
        }
        mMap.setOnMarkerDragListener(this);
        mMap.setOnMarkerClickListener(this);

    }

    protected synchronized void buildGoogleApiClient() {
        mGoogleApiClient = new GoogleApiClient.Builder(this)
                .addConnectionCallbacks(this)
                .addOnConnectionFailedListener(this)
                .addApi(LocationServices.API)
                .build();
        mGoogleApiClient.connect();
           }
		   
    @Override
    public void onConnected(@Nullable Bundle bundle) {
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mReceiver, filter);
        mLocationRequest = new LocationRequest();
        mLocationRequest.setInterval(100);
        mLocationRequest.setFastestInterval(100);
        mLocationRequest.setPriority(LocationRequest.PRIORITY_BALANCED_POWER_ACCURACY);
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_FINE_LOCATION)
                == PackageManager.PERMISSION_GRANTED) {
            //LocationServices.FusedLocationApi.requestLocationUpdates(mGoogleApiClient, mLocationRequest, (com.google.android.gms.location.LocationListener) this);
        }
        connectToCarBluetooth();
    }

    @Override
    public void onConnectionSuspended(int i) {
    }

    @Override
    public void onConnectionFailed(@NonNull ConnectionResult connectionResult) {

    }

    @Override
    public boolean onMarkerClick(Marker marker) {
       return false;
    }

    @Override
    public void onMarkerDragStart(Marker marker) {

    }

    @Override
    public void onMarkerDrag(Marker marker) {

    }

    @Override
    public void onMarkerDragEnd(Marker marker) {
        end_latitude = marker.getPosition().latitude;
        end_longitude = marker.getPosition().longitude;
        marker.setDraggable(false);
        marker.setTitle("Destination Position");
        marker.setIcon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_RED));//HUE_RED markerOptions.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_MAGENTA));
        Log.d("end_lat", "" + end_latitude);
        Log.d("end_lng", "" + end_longitude);
        Object dataTransfer[] = new Object[3];
        if ((end_latitude != 0) && (end_longitude != 0)) {
            String url = getRouteDirectionsUrl();
            dataTransfer[0] = mMap;
            dataTransfer[1] = url;
            dataTransfer[2] = new LatLng(end_latitude, end_longitude);

            GetMyRoutes getRouteDirection = (GetMyRoutes) new GetMyRoutes(new GetMyRoutes.AsyncResponseTry() {
                @Override
                public void processFinishTry(List<LatLng> allGeoPointsresults) {
                    directionPointsTobeSentonBluetooth.clear();

                    int count = 1;
                    for (LatLng lg : allGeoPointsresults) {
                        String lattem = Double.toString(lg.latitude);
                        if (lattem.length() >= 9) {
                            lattem = lattem.substring(0, 9);
                            String last_letter = Character.toString(lattem.charAt(8));
                            if (last_letter.equals("0")) {
                                String change = lattem.substring(0, 8) + "1";
                                lattem = change;
                            }
                        } else {
                            int n = 10 - lattem.length();
                            lattem = lattem + appendzeros(n);
                        }

                        directionPointsTobeSentonBluetooth.add(Double.parseDouble(lattem));
                        String longtem = Double.toString(lg.longitude);
                        if (longtem.length() >= 11) {
                            longtem = longtem.substring(0, 11);
                            String last_letter = Character.toString(longtem.charAt(10));
                            if (last_letter.equals("0")) {
                                String change = longtem.substring(0, 10) + "1";
                                longtem = change;
                            }
                        } else {
                            int n = 12 - longtem.length();
                            longtem = longtem + appendzeros(n);
                        }

                        directionPointsTobeSentonBluetooth.add(Double.parseDouble(longtem));

                        LatLng l = new LatLng(Double.parseDouble(lattem), Double.parseDouble(longtem));
                        MarkerOptions markerOptions = new MarkerOptions();
                        markerOptions.position(l);
                        markerOptions.title("Position= " + count + l.latitude + "," + l.longitude);
                        markerOptions.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_BLUE));
                        mMap.addMarker(markerOptions);
                        count++;
                    }

                }
            }).execute(dataTransfer);

            checking = true;
            sendcoordinate.setClickable(true);
        }



    }

    private boolean CheckGooglePlayServices() {
        GoogleApiAvailability googleAPI = GoogleApiAvailability.getInstance();
        int result = googleAPI.isGooglePlayServicesAvailable(this);
        if (result != ConnectionResult.SUCCESS) {
            if (googleAPI.isUserResolvableError(result)) {
                googleAPI.getErrorDialog(this, result,
                        0).show();
            }
            return false;
        }
        return true;
    }


    public boolean checkLocationPermission() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.ACCESS_FINE_LOCATION)) {
    ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                        MY_PERMISSIONS_REQUEST_LOCATION);


            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                        MY_PERMISSIONS_REQUEST_LOCATION);
            }
            return false;
        } else {
            return true;
        }
    }

    public boolean checkBluetoothPermission() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.BLUETOOTH_ADMIN)
                != PackageManager.PERMISSION_GRANTED) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.BLUETOOTH_ADMIN)) {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.BLUETOOTH_ADMIN},
                        My_BLUETOOTH_REQUEST_ACCESS);


            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.BLUETOOTH_ADMIN},
                        My_BLUETOOTH_REQUEST_ACCESS);
            }
            return false;
        } else {
            return true;
        }
    }


    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_LOCATION: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    if (ContextCompat.checkSelfPermission(this,
                            Manifest.permission.ACCESS_FINE_LOCATION)
                            == PackageManager.PERMISSION_GRANTED) {

                        if (mGoogleApiClient == null) {
                            buildGoogleApiClient();
                        }
                        mMap.setMyLocationEnabled(true);
                    }

                } else {
                    Toast.makeText(this, "permission denied", Toast.LENGTH_LONG).show();
                }
                break;
            }
            case My_BLUETOOTH_REQUEST_ACCESS: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[1] == PackageManager.PERMISSION_GRANTED) {

                    // permission was granted. Do the
                    // contacts-related task you need to do.
                    if (ContextCompat.checkSelfPermission(this,
                            Manifest.permission.BLUETOOTH)
                            == PackageManager.PERMISSION_GRANTED) {

                        if (mGoogleApiClient == null) {
                            buildGoogleApiClient();
                        }
                        if (!mBluetoothAdapter.isEnabled()) {
                            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                            startActivityForResult(enableBtIntent, My_BLUETOOTH_REQUEST_ACCESS);
                        }
                    }

                } else {

                    Toast.makeText(this, "permission denied", Toast.LENGTH_LONG).show();
                }

                return;
            }
        }
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice bt = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                mybluetoothconnector.setBluetoothdevice(bt);
                String deviceName = mybluetoothconnector.getBluetoothdevice().getName();
                String deviceHardwareAddress = mybluetoothconnector.getBluetoothdevice().getAddress();
            }
            else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                my_connection_switch.toggle();

            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
        try {
            if(mybluetoothconnector.getBluetoothdevice()!=null) {
                if (mybluetoothconnector.getBluetoothSocket().isConnected()) {
                    mybluetoothconnector.getBluetoothSocket().close();
                }
            }
        }
        catch (IOException e) {
            e.printStackTrace();
        }

    }

    void getPairedBluetoothDevices() {
        try {
            Set<BluetoothDevice> bondedDevices = mBluetoothAdapter.getBondedDevices();
            mybluetoothconnector.setBluetoothdevice(mBluetoothAdapter.getRemoteDevice(DEVICE_ADDRESS));
            if (bondedDevices.contains(mybluetoothconnector.getBluetoothdevice())) {
                bluetoothDeviceFound = true;
            }
        }catch (Exception e){
            e.printStackTrace();
        }


    }
    public void receiveCarLocation() {
        final Handler handler = new Handler();
        final byte delimiter = 10;
       stopReceiving = false;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        receiveLocationThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopReceiving) {
                        try {
                            if (mybluetoothconnector.getInputStream() != null) {
                                int bytesAvailable = mybluetoothconnector.getInputStream().available();
                                if (bytesAvailable > 0) {
                                    byte[] packetBytes = new byte[bytesAvailable];
                                    mybluetoothconnector.getInputStream().read(packetBytes);
                                    for (int i = 0; i < bytesAvailable; i++) {
                                        byte b = packetBytes[i];
                                        if (b == delimiter) {
                                            byte[] encodedBytes = new byte[readBufferPosition];
                                            System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                            final String data = new String(encodedBytes, "US-ASCII");
                                            readBufferPosition = 0;
                                            handler.post(new Runnable() {
                                                public void run() {

                                                    if((data.contains("L")) && (data.contains(","))){
                                                        String co_orrd_temp=data.substring(1);
                                                        String co_orrd[]=co_orrd_temp.split(",");
                                                        latitude= Double.parseDouble(co_orrd[0]);
                                                        longitude=Double.parseDouble(co_orrd[1]);
                                                        initial_car_point();
                                                        datatextView.setText(co_orrd_temp);
                                                        stopReceiving = true;
                                                    }


                                                }
                                            });
                                        } else {
                                            readBuffer[readBufferPosition++] = b;
                                        }
                                    }
                                }
                            }
                            } catch(IOException ex){
                            stopReceiving = true;
                            }


                }
            }
        });

        receiveLocationThread.start();
    }


    public void connectToCarBluetooth() {
        stopConnecting = false;
        final Handler handler1 = new Handler();
        connectionThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopConnecting) {
                    if (!bluetoothDeviceFound) {
                        getPairedBluetoothDevices();
                    }
                            if (mybluetoothconnector.getBluetoothdevice() != null) {
                                try {
                                    mybluetoothconnector.setBluetoothSocket(mybluetoothconnector.getBluetoothdevice().createRfcommSocketToServiceRecord(mybluetoothconnector.getBluetoothdevice().getUuids()[0].getUuid()));
                                    mybluetoothconnector.getBluetoothSocket().connect();
                                    mybluetoothconnector.setOutputStream(mybluetoothconnector.getBluetoothSocket().getOutputStream());
                                    mybluetoothconnector.setInputStream(mybluetoothconnector.getBluetoothSocket().getInputStream());
                                    stopConnecting = true;

                                    handler1.post(new Runnable() {
                                        public void run() {
                                            try{
                                                my_connection_switch.toggle();
                                              }catch(NumberFormatException e){
                                            }
                                        }
                                    });

                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                                catch(Exception e){
                                    e.printStackTrace();
                                }
                            }
                }
            }
        });

        connectionThread.start();
    }

}