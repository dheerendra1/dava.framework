package com.dava.templateproject;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class JNIRenderer implements GLSurfaceView.Renderer 
{
    private native void nativeResize(int w, int h);
    private native void nativeRender();
    private native void nativeRenderRecreated();

    public void onSurfaceCreated(GL10 gl, EGLConfig config) 
    {
    	Log.w(JNIConst.LOG_TAG, "_________onSurfaceCreated_____!!!!_____");
    	nativeRenderRecreated();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) 
    {
    	Log.w(JNIConst.LOG_TAG, "_________onSurfaceChanged");
        nativeResize(w, h);
    }

    public void onDrawFrame(GL10 gl) 
    {
    	nativeRender();
    }
}
