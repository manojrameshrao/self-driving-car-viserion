package com.example.pratap.viserion;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by pratap on 06-10-2017.
 */

public class ParseJasonData {

    public String[] parsingDirections(String jsonData)
    {
        JSONArray jsonArray = null;
        JSONObject jsonObject;

        try {
            jsonObject = new JSONObject(jsonData);
            jsonArray = jsonObject.getJSONArray("routes").getJSONObject(0).getJSONArray("legs").getJSONObject(0).getJSONArray("steps");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return getRoutes(jsonArray);
    }

    private String[] getRoutes(JSONArray googleStepsJson )
    {

        String[] routes = new String[googleStepsJson.length()];

        for(int i = 0;i<googleStepsJson.length();i++)
        {
            try {
                routes[i] = getRoute(googleStepsJson.getJSONObject(i));
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        return routes;
    }

    private String getRoute(JSONObject googlePathJson)
    {
        String route = "";
        try {
            route = googlePathJson.getJSONObject("polyline").getString("points");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return route;
    }

}
