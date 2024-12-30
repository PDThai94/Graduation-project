package com.example.iot;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageButton;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.Objects;
public class infor_room2 extends AppCompatActivity {

    public ImageButton backButton;
    TextView temp_show_2, humi_show_2, flame_show_2, CO_gas_show_2;

    @SuppressLint({"DefaultLocale", "SetTextI18n"})
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.infor_room2);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        backButton = findViewById(R.id.back_2);
        backButton.setOnClickListener(v -> getOnBackPressedDispatcher().onBackPressed());

        temp_show_2 = findViewById(R.id.temp_show_2);
        humi_show_2 = findViewById(R.id.humi_show_2);
        flame_show_2 = findViewById(R.id.flame_show_2);
        CO_gas_show_2 = findViewById(R.id.CO_gas_show_2);

        // Truy xuất cơ sở dữ liệu Firebase
        DatabaseReference databaseReference = FirebaseDatabase.getInstance().getReference("/Node_/2");

        // Lắng nghe các giá trị
        listenForValue(databaseReference.child("temperature"), value -> {
            double temperature = Double.parseDouble(value);
            temp_show_2.setText(String.format("%.1f°C", temperature));
        });

        listenForValue(databaseReference.child("humidity"), value -> {
            double humidity = Double.parseDouble(value);
            humi_show_2.setText(String.format("%.1f%%", humidity));
        });

        listenForValue(databaseReference.child("flame"), value -> {
            int flameStatus = Integer.parseInt(value);
            if (flameStatus == 1) {
                flame_show_2.setText("🔥 Có cháy!");
                Log.i("FlameSensor", "🔥 Có cháy! Hành động khẩn cấp.");
            } else if (flameStatus == 0) {
                flame_show_2.setText("✅ An toàn");
                Log.i("FlameSensor", "✅ An toàn, không có cháy.");
            } else {
                Log.w("FlameSensor", "⚠️ Giá trị không hợp lệ: " + flameStatus);
            }
        });

        listenForValue(databaseReference.child("CO"), value -> {
            int COStatus = Integer.parseInt(value);
            if (COStatus == 1) {
                CO_gas_show_2.setText("⚠️ Cảnh báo nguy hiểm!");
                Log.i("CO_Sensor", "⚠️ Cảnh báo khí CO nguy hiểm.");
            } else if (COStatus == 0) {
                CO_gas_show_2.setText("✅ An toàn");
                Log.i("CO_Sensor", "✅ Không phát hiện khí CO.");
            } else {
                Log.w("CO_Sensor", "⚠️ Giá trị không hợp lệ: " + COStatus);
            }
        });
    }
    private void listenForValue(DatabaseReference reference, Infor_room1.OnValueChangedListener onValueChanged) {
        reference.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                try {
                    String value = Objects.requireNonNull(snapshot.getValue()).toString();
                    onValueChanged.onChange(value);
                } catch (Exception e) {
                    Log.e("FirebaseData", "Lỗi khi lấy dữ liệu từ Firebase", e);
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("FirebaseData", "Lỗi Firebase: " + error.getMessage());
            }
        });
    }

    /**
     * Interface để callback khi giá trị thay đổi
     */
    interface OnValueChangedListener {
        void onChange(String value);
    }
}