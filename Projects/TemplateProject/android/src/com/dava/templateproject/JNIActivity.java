package com.dava.templateproject;


import java.io.IOException;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

public class JNIActivity extends Activity implements JNIAccelerometer.JNIAccelerometerListener
{
	private static int errorState = 0;

	private GLSurfaceView mGLView = null;
    private JNIAccelerometer accelerometer = null;

	public boolean keyboardShowed = false;
    
    private native void nativeOnCreate(boolean isFirstRun);
    private native void nativeOnStart();
    private native void nativeOnStop();
    private native void nativeOnDestroy();
    private native void nativeIsFinishing();
    private native void nativeOnAccelerometer(float x, float y, float z);
    
    private boolean isFirstRun = true;
    
    private String dataDirString = "";
    private String apkFileString = "";
    
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
    	// The activity is being created.
        Log.i(JNIConst.LOG_TAG, "[Activity::onCreate]");

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        
        super.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // initialize accelerometer
        SensorManager sensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
        accelerometer = new JNIAccelerometer(this, sensorManager);

        // initialize GL VIEW
        mGLView = new JNIGLSurfaceView(this);
        mGLView.setFocusableInTouchMode(true);
        mGLView.setClickable(true);
        mGLView.setFocusable(true);
        mGLView.requestFocus();
        setContentView(mGLView);
    
        if(0 != errorState)
        {

        }

        boolean pathFound = FindSystemPath();
        
        if(null != savedInstanceState)
        {
        	isFirstRun = savedInstanceState.getBoolean("isFirstRun");
        }
        
        Log.i(JNIConst.LOG_TAG, "[Activity::onCreate] isFirstRun is " + isFirstRun); 
        nativeOnCreate(isFirstRun);
    }
    
    private boolean FindSystemPath()
    {
    	boolean pathFound = false;
    	
    	java.io.File dataDir = getFilesDir(); 
        Log.i(JNIConst.LOG_TAG, String.format("[Activity::FindSystemPath] external files dir is %s", dataDir.toString())); 
        
        try
        {
        	dataDirString = dataDir.toString();
        	
            String []strs = getAssets().list(dataDirString);
            int count = strs.length;
            for(int i = 0; i < count; ++i)
            {
                Log.i(JNIConst.LOG_TAG, String.format("[Activity::FindSystemPath] AL[%d]:  %s", i, strs[i])); 
            }
            
    		PackageManager packMgmr = getPackageManager();
    		ApplicationInfo appInfo = packMgmr.getApplicationInfo("com.dava.templateproject", 0);
    		apkFileString = appInfo.sourceDir;
    		
    		pathFound = true;
    		
            Log.i(JNIConst.LOG_TAG, String.format("[Activity::FindSystemPath] dataDirString is %s", dataDirString));
            Log.i(JNIConst.LOG_TAG, String.format("[Activity::FindSystemPath] apkFileString is %s", apkFileString));
        }
        catch(Exception e)
        {
            Log.e(JNIConst.LOG_TAG, "[Activity::FindSystemPath] Exc: " + e.getMessage()); 
        }
        
    	return pathFound;
    }
    
    
    @Override
    protected void onStart()
    {
    	super.onStart();
    	// The activity is about to become visible.
    	
        Log.i(JNIConst.LOG_TAG, "[Activity::onStart]");
        
        //call native method
        nativeOnStart();
    }
    
    @Override
    protected void onRestart()
    {
    	super.onRestart();
    	// The activity is about to become visible.
    	
        Log.i(JNIConst.LOG_TAG, "[Activity::onRestart]");
        
        //TODO: VK: may be usefull
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) 
    {
        Log.i(JNIConst.LOG_TAG, "[Activity::onSaveInstanceState]");

        outState.putBoolean("isFirstRun", isFirstRun);
    	
    	super.onSaveInstanceState(outState);
    }
    
    @Override
    protected void onResume() 
    {
        super.onResume();
        // The activity has become visible (it is now "resumed").
		Log.i(JNIConst.LOG_TAG, "[Activity::onResume] start");

        // activate accelerometer
        if(null != accelerometer)
        {
        	if(accelerometer.IsSupported())
        	{
        		accelerometer.Start();
        	}
        }
        
        // activate GLView 
        if(null != mGLView)
        {
        	mGLView.onResume();
        }

        Log.i(JNIConst.LOG_TAG, "[Activity::onResume] finish");
    }

    
    @Override
    protected void onPause() 
    {
        super.onPause();
        // Another activity is taking focus (this activity is about to be "paused").

        Log.i(JNIConst.LOG_TAG, "[Activity::onPause] start");

        // deactivate accelerometer
        if(null != accelerometer)
        {
        	if(accelerometer.IsActive())
        	{
        		accelerometer.Stop();
        	}
        }

        // deactivate GLView 
        if(null != mGLView)
        {
        	mGLView.onPause();
        }
        
        boolean isActivityFinishing = isFinishing();
        Log.i(JNIConst.LOG_TAG, "[Activity::onPause] isActivityFinishing is " + isActivityFinishing);
        if(isActivityFinishing)
        {
        	nativeIsFinishing();
        }
        
        Log.i(JNIConst.LOG_TAG, "[Activity::onPause] finish");
    }

    
    @Override
    protected void onStop()
    {
        Log.i(JNIConst.LOG_TAG, "[Activity::onStop] start");
        
        //call native method
        nativeOnStop();

        Log.i(JNIConst.LOG_TAG, "[Activity::onStop] finish");
        
        super.onStop();
    	// The activity is no longer visible (it is now "stopped")
    }
    
    
    @Override
    protected void onDestroy()
    {
        Log.i(JNIConst.LOG_TAG, "[Activity::onDestroy] start");

        //call native method
        nativeOnDestroy();

        Log.i(JNIConst.LOG_TAG, "[Activity::onDestroy] finish");

        super.onDestroy();
    	// The activity is about to be destroyed.
    }

    @Override
    protected void onPostResume() 
    {
        Log.i(JNIConst.LOG_TAG, "[Activity::onPostResume] ****");
        
    	super.onPostResume();
    }
    
    
    public void onAccelerationChanged(float x, float y, float z)
	{
		nativeOnAccelerometer(x, y, z);
	}
	
	public void ShowKeyboard()
	{
//		if(!keyboardShowed)
//		{
//			InputMethodManager im = ((InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE));
//			im.showSoftInput(mGLView, 0);
//			keyboardShowed = true;
//		}
	}
	public void HideKeyboard()
	{
//		if(keyboardShowed)
//		{
//			InputMethodManager im = ((InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE));
//			im.hideSoftInputFromWindow(mGLView.getWindowToken(), 0);
//			keyboardShowed = false;
//		}
	}
}
