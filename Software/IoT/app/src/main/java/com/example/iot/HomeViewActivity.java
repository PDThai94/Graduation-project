package com.example.iot;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.view.View;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.cardview.widget.CardView;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class HomeViewActivity extends AppCompatActivity implements View.OnClickListener {

    public CardView getRoom_1, getRoom_2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.home_view);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

// Greeting mess
        TextView greetingTextView = findViewById(R.id.greetingTextView);
        String greetingMessage = getGreetString();
        // Hiển thị lời chào lên TextView
        greetingTextView.setText(greetingMessage);
// Date & time
        TextView datetextView = findViewById(R.id.date_home_show);
        Date date = new Date();
        @SuppressLint("SimpleDateFormat") SimpleDateFormat s = new SimpleDateFormat("dd/MM/yyyy");
        String dateString = s.format(date);
        datetextView.setText(dateString);

// Card view into activate
//      Card view room 1 setup
        getRoom_1 = findViewById(R.id.room_1);
        getRoom_1.setOnClickListener(this);
//      Card view room 2 setup
        getRoom_2 = findViewById(R.id.room_2);
        getRoom_2.setOnClickListener(this);

    }

    @NonNull
    private static String getGreetString() {
        Calendar calendar = Calendar.getInstance();
        int currentHour = calendar.get(Calendar.HOUR_OF_DAY);
        String greetingMessage;
        if (currentHour >= 5 && currentHour < 12) {
            greetingMessage = "Chào buổi sáng!";
        } else if (currentHour >= 12 && currentHour < 18) {
            greetingMessage = "Chào buổi chiều!";
        } else if (currentHour >= 18 && currentHour < 23) {
            greetingMessage = "Chào buổi tối!";
        } else {
            greetingMessage = "Chúc ngủ ngon!";
        }
        return greetingMessage;
    }

    @SuppressLint("NonConstantResourceId")
    @Override
    public void onClick(View v) {
        Intent i;

        if (v.getId() == R.id.room_1) {
            i = new Intent(this, Infor_room1.class);
            startActivities(new Intent[]{i});
        } else if (v.getId() == R.id.room_2) {
            i = new Intent(this, infor_room2.class);
            startActivities(new Intent[]{i});
        }
    }
}