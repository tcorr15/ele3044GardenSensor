package com.example.rpitest;

import com.example.rpitest.HttpsTrustManager;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;


import androidx.appcompat.app.AppCompatActivity;



public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button viewBtn = (Button) findViewById(R.id.viewButton);
        viewBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                /*On Click - will get data from HUB and display in dataView*/

                // Define required variables
                TextView dataView = (TextView) findViewById(R.id.dataView); // Output text view
                String output = HttpUtils.getContents("https://192.168.0.20/db_view.php");
                dataView.setText(output);
            }
        });
    }
}