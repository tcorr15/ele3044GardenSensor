package com.example.smartyard;

import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import com.example.smartyard.databinding.ActivityMainBinding;
import com.google.android.material.bottomnavigation.BottomNavigationView;

import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    /*String JSON_STRING = HttpUtils.getContents("https://192.168.0.20/db_view.php"); This should get data from php server*/
    String JSON_STRING = "{\"10101\":{\"ID\":\"18\",\"tstamp\":\"2022-01-27 16:32:30\",\"score\":\"25\",\"temp\":\"22.4\",\"moisture\":\"44\", \"uv\":\"27\"}}";
    String tstamp, score, temp, moisture, uv;
    TextView Tstamp, Score, Tempd, Moisture, Uv, Info;
    ImageView Mulchie;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        BottomNavigationView navView = findViewById(R.id.nav_view);
        // Passing each menu ID as a set of Ids because each
        // menu should be considered as top level destinations.
        AppBarConfiguration appBarConfiguration = new AppBarConfiguration.Builder(
                R.id.navigation_home, R.id.navigation_dashboard, R.id.navigation_notifications)
                .build();
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_activity_main);
        NavigationUI.setupActionBarWithNavController(this, navController, appBarConfiguration);
        NavigationUI.setupWithNavController(binding.navView, navController);

        // Print Values From Json
        Tstamp = (TextView) findViewById(R.id.tstamp);
        Score = (TextView) findViewById(R.id.score);
        Temp = (TextView) findViewById(R.id.temp);
        Moisture = (TextView) findViewById(R.id.moisture);
        Uv = (TextView) findViewById(R.id.uv);
        Info = (TextView) findViewById(R.id.info);
        Mulchie = (ImageView) findViewById(R.id.mulchie);

        try {
            // get JSONObject from JSON file
            JSONObject obj = new JSONObject(JSON_STRING);
            // fetch JSONObject named input
            JSONObject input = obj.getJSONObject("10101");
            // get values
            this.tstamp = input.getString("tstamp");
            score = input.getString("score");
            temp = input.getString("temp");
            moisture = input.getString("moisture");
            uv = input.getString("uv");
            // set value in TextView's
            Tstamp.setText("Last Refreshed: "+ tstamp);
            Score.setText("Score: "+ score +"%");
            Temp.setText("Temp: "+ temp +"°C");
            Moisture.setText("Moisture: "+ moisture);
            Uv.setText("Uv: "+ uv);

            int scoreInt = Integer.parseInt(score);
            if (scoreInt >= 90) {
                Mulchie.setImageResource(R.drawable.joy);
                Info.setText("Mulchie is feeling great,\nGood joy!");
            }
            else if (scoreInt < 50) {
                Mulchie.setImageResource(R.drawable.sad);
                Info.setText("Mulchie is feeling awful,\nYour garden needs urgent care!");
            }
            else {
                Mulchie.setImageResource(R.drawable.mid);
                Info.setText("Mulchie is feeling Okay,\nSome work is required...");
            }

        } catch (JSONException e) {
            e.printStackTrace();
        }

    }

}