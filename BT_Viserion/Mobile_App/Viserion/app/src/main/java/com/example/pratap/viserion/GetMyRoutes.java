package com.cmpe.pratap.viserion;

import android.graphics.Color;
import android.os.AsyncTask;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
import com.google.maps.android.PolyUtil;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * Created by pratap on 06-10-2017.
 */

public  class GetMyRoutes extends AsyncTask<Object,String,String>{
    public interface AsyncResponseTry {
        public void processFinishTry(List<LatLng> allGeoPointsresults);
    }

    public AsyncResponseTry delegatetry = null;

    public GetMyRoutes(AsyncResponseTry delegate){
        this.delegatetry = delegate;
    }

    public static final double mapAddContPoints = 0.00008;

    GoogleMap mMap;
    String url;
    String googleDirectionsData;
    LatLng latLng;
    Polyline polyline = null;
    List<LatLng> allGeoPoints = new ArrayList<LatLng>();
    List<LatLng> allGeoPointsfinal = new ArrayList<LatLng>();
    List<LatLng> allGeoPointsresults = new ArrayList<LatLng>();
    List<Polyline> polylinelist = new ArrayList<Polyline>();

    @Override
    protected String doInBackground(Object... objects) {
        mMap = (GoogleMap) objects[0];
        url = (String) objects[1];
        latLng = (LatLng) objects[2];

        GetDirectionUrl getDirectionUrl = new GetDirectionUrl();
        try {
            googleDirectionsData = getDirectionUrl.directionUrl(url);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return googleDirectionsData;
    }
    @Override
    protected void onPostExecute(String gDirectionData) {

        String[] routeList;
        ParseJasonData parser = new ParseJasonData();
        routeList = parser.parsingDirections(gDirectionData);
        displayDirection(routeList);


    }

    public void displayDirection(String[] directionsList) {
        int count = directionsList.length;
        for (int i = 0; i < count; i++) {
            PolylineOptions options = new PolylineOptions();
            options.color(Color.RED);
            options.width(10);
            options.addAll(PolyUtil.decode(directionsList[i]));
            AllGeoPoints(PolyUtil.decode(directionsList[i]));
            polyline = this.mMap.addPolyline(options);
            polylinelist.add(polyline);
        }
        int i = 0;
        allGeoPointsfinal.addAll(allGeoPoints);
        for (int j = 0; j < allGeoPointsfinal.size() - 1; j++) {
            LatLng temp1 = allGeoPointsfinal.get(j);
            LatLng temp2 = allGeoPointsfinal.get(j + 1);
            double la1 = temp1.latitude, la2 = temp2.latitude;
            double lo1 = temp1.longitude, lo2 = temp2.longitude;
            double l1, l2;
            if (la1 > la2 || lo1 < lo2) {
                l1 = la1 - la2;
                l2 = lo2 - lo1;
                if ((l1 > mapAddContPoints) || (l2 < (0 - mapAddContPoints))) {
                    LatLng newPointToAdd = new LatLng((la1 + la2) / 2, (lo1 + lo2) / 2);
                    allGeoPointsfinal.add(j + 1, newPointToAdd);
                    j--;
                }

            }

            if (la1 < la2 || lo1 > lo2) {
                l1 = la2 - la1;
                l2 = lo1 - lo2;
                if ((l1 > mapAddContPoints) || (l2 < (0 - mapAddContPoints))) {
                    LatLng newPointToAdd = new LatLng((la1 + la2) / 2, (lo1 + lo2) / 2);
                    allGeoPointsfinal.add(j + 1, newPointToAdd);
                    j--;
                }
            }

        }
        allGeoPointsresults.clear();
        allGeoPointsresults.addAll(allGeoPointsfinal);
         delegatetry.processFinishTry(allGeoPointsfinal);
        allGeoPointsfinal.removeAll(allGeoPoints);
    
    }

    public void AllGeoPoints(Iterable<LatLng> var1) {
        Iterator var2 = var1.iterator();

        while (var2.hasNext()) {
            LatLng var3 = (LatLng) var2.next();
            allGeoPoints.add(var3);
        }
    }



    public List<LatLng> getAllGeoPointsToBeSentToGps() {

        return allGeoPointsresults;
    }
}
