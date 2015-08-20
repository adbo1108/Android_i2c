package com.darwinprecisions.tmp75_i2c;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.orhanobut.logger.Logger;

import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("ReadTempI2c");
    }


    public native byte[] i2cReadTemp();



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        set_GPIO_I2C();

        new Thread(new Runnable() {
            @Override
            public void run() {
                while(true) {
                    Logger.d("now temp is %d", getCurrentTemp());
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }}
        }).start();
    }

    private void set_GPIO_I2C() {
        Process pro_su;
        DataOutputStream os_cmd;

        try {
            pro_su = Runtime.getRuntime().exec("su");
            os_cmd = new DataOutputStream(pro_su.getOutputStream());
            if (!new File("/sys/class/gpio/gpio232").exists()) {

                String cmd = "echo 232 >  /sys/class/gpio/export";
                os_cmd.writeBytes(cmd + "\n");
                cmd = "echo out >  /sys/class/gpio/gpio232/direction";
                os_cmd.writeBytes(cmd + "\n");
                cmd = "chmod 777  /dev/i2c-2";
                os_cmd.writeBytes(cmd + "\n");
            }

        } catch (IOException e) {

            e.printStackTrace();
        }
    }

    private int getCurrentTemp() {
        int tmp = 0;
        byte[] buf = new byte[2];
        buf = i2cReadTemp();
        tmp = buf[0];
        return tmp;
    }


}
