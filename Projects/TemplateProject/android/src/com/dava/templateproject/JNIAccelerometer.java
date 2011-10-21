package com.dava.templateproject;

import java.util.List;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;


public class JNIAccelerometer
{
    private static Sensor sensor = null;
    private static SensorManager sensorManager = null;

    private boolean isActive = false;
    private static Boolean isSupported = null;

    private JNIAccelerometerListener jniListener = null;
    private JNISensorEventListener sensorEventListener = null;

    public JNIAccelerometer(JNIAccelerometerListener listener, SensorManager smanager)
    {
    	sensorEventListener = new JNISensorEventListener();
    	SetListener(listener);
    	SetManager(smanager);
    }
    
    private void SetListener(JNIAccelerometerListener listener)
    {
    	jniListener = listener;
    }
    
    private void SetManager(SensorManager smanager)
    {
    	sensorManager = smanager;
    }

    public boolean IsActive()
    {
    	return isActive;
    }
    
    public boolean IsSupported()
    {
        if (null == isSupported) 
        {
            if (null != sensorManager) 
            {
                List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
                isSupported = new Boolean(sensors.size() > 0);
            } 
            else 
            {
            	isSupported = Boolean.FALSE;
            }
        }
    	
    	return isSupported;
    }
    
    public void Start() 
	{
    	if(null != sensorManager)
    	{
            List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
            if (sensors.size() > 0) 
            {
                sensor = sensors.get(0);
                isActive = sensorManager.registerListener(sensorEventListener, sensor, SensorManager.SENSOR_DELAY_GAME);
            }
    	}
    }

	public void Stop()
	{
		isActive = false;
        try 
        {
            if (sensorManager != null && sensorEventListener != null) 
            {
                sensorManager.unregisterListener(sensorEventListener);
            }
        } 
        catch (Exception e) 
        {
        	Log.e(JNIConst.LOG_TAG, "[JNIAccelerometer.Stop] exception = " + e.getMessage());
        }
	}
    
	public interface JNIAccelerometerListener 
	{
		public void onAccelerationChanged(float x, float y, float z);
	}
	
	public class JNISensorEventListener implements SensorEventListener
	{

		public void onAccuracyChanged(Sensor sensor, int accuracy) 
		{
		}

		public void onSensorChanged(SensorEvent event) 
		{
			if(null != jniListener)
			{
				float x = event.values[0];
				float y = event.values[1];
				float z = event.values[2];
	 
	            // trigger change event
	            jniListener.onAccelerationChanged(x, y, z);				
			}
		}
	}
}
