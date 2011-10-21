package com.dava.templateproject;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.KeyEvent;
import android.view.MotionEvent;


public class JNIGLSurfaceView extends GLSurfaceView 
{
	private JNIRenderer mRenderer = null;

    private static final int TOUCH_DELTA = 10;
    private Touch []previousTouches = null;
    private int allocatedTouches = 0;
    private int previousCount = 0;
    
    private native void nativeOnTouch(int action, int id, int x, int y, long time);
    private native void nativeOnKeyUp(int keyCode);
    private native void nativeOnResumeView();
    private native void nativeOnPauseView();

    public JNIGLSurfaceView(Context context) 
    {
        super(context);
        
        ReallocateTouches(TOUCH_DELTA);
     
        mRenderer = new JNIRenderer();
        setRenderer(mRenderer);
    }

    @Override
	public void onPause()
	{
    	queueEvent(new Runnable() 
    	{
    		public void run() 
    		{
    	    	nativeOnPauseView();
    		}
    	});
		super.onPause();
	}

    @Override
	public void onResume()
	{
		super.onResume();
		nativeOnResumeView();
	}
    
    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent event)
    {
//    	queueEvent(new Runnable() 
//    	{
//    		public void run() 
//    		{
//    		}
//    	});	
    	
    	return super.onKeyDown(keyCode, event);
    }
    
    @Override
    public boolean onKeyUp(final int keyCode, final KeyEvent event)
    {
    	queueEvent(new Runnable() 
    	{
    		public void run() 
    		{
    			int metaState = event.getMetaState(); 
    			int keyChar = event.getUnicodeChar(metaState);
    			if(0 != keyChar)
    			{
//    				Log.d(JNIConst.LOG_TAG, "Char = " + keyChar);
    				nativeOnKeyUp(keyChar);
    			}
    		}
    	});	

    	return super.onKeyUp(keyCode, event);
    }
    
    
    public boolean onTouchEvent(final MotionEvent event) 
    {
    	queueEvent(new Runnable()
    	{
    		public void run() 
    		{
    	    	int actionMasked = event.getActionMasked();
    	    	int count = event.getPointerCount();
    	    	long time = event.getEventTime();
    	    	int index = event.getActionIndex();

	    		boolean regularAction = true;
	    		if(		(MotionEvent.ACTION_POINTER_UP == actionMasked)
	    	    	||	(MotionEvent.ACTION_POINTER_DOWN == actionMasked))
	    		{
	    			regularAction = false;
	    		}
    	    	
    	    	if(		(MotionEvent.ACTION_MOVE == actionMasked)
    	    		||	(MotionEvent.ACTION_DOWN == actionMasked)
    	    		||	(MotionEvent.ACTION_OUTSIDE == actionMasked)
    	    		||	(MotionEvent.ACTION_CANCEL == actionMasked)
    	    		||	(MotionEvent.ACTION_POINTER_UP == actionMasked)
    	    		||	(MotionEvent.ACTION_POINTER_DOWN == actionMasked)
    	    		||	(MotionEvent.ACTION_UP == actionMasked))
    	    	{
    	    		if(allocatedTouches < count)
    	    		{
    	    			ReallocateTouches(count + TOUCH_DELTA);
    	    		}
    	    		
        	    	for(int i = 0; i < previousCount; ++i)
        	    	{
        	    		previousTouches[i].active = false;
        	    	}
    	    		
        	    	for(int i = 0; i < count; ++i)
        	    	{
        	    		int id = event.getPointerId(i);
        	    		int x = (int)event.getX(i);
        	    		int y = (int)event.getY(i);

        	    		Touch touch = FindPreviousTouch(id);
        	    		if(null == touch)
        	    		{
        	    			touch = GetNextFreeTouch();
        	    			touch.id = id;
        	    			
        	    			if(		(MotionEvent.ACTION_DOWN != actionMasked)
        	    				&& 	(MotionEvent.ACTION_POINTER_DOWN != actionMasked))
        	    			{
        	    				//start touch action
                    	    	nativeOnTouch(MotionEvent.ACTION_DOWN, touch.id, x, y, time);
        	    			}
        	    		}
        	    		
        	    		touch.active = true;
        	    		touch.action = (regularAction) ? actionMasked : (actionMasked - MotionEvent.ACTION_POINTER_DOWN);
    	    			touch.x = x;
    	    			touch.y = y;
        	    		
    	    			if(regularAction || (i == index))
    	    			{
                	    	nativeOnTouch(touch.action, touch.id, touch.x, touch.y, time);
    	    			}
        	    	}
        	    	
        	    	for(int i = 0; i < previousCount; ++i)
        	    	{
        	    		if(!previousTouches[i].active)
        	    		{
        	    			int touchaction = previousTouches[i].action; 
        	    			if(		(MotionEvent.ACTION_DOWN == touchaction) 
        	    				||	(MotionEvent.ACTION_MOVE == touchaction))
        	    			{
                    	    	nativeOnTouch(MotionEvent.ACTION_UP, previousTouches[i].id, previousTouches[i].x, previousTouches[i].y, time);
        	    			}
        	    		}
        	    	}

        	    	previousCount = count;
        	    	
        	    	ClearNotActiveTouches();
    	    	}
    		}
    	});
    	
        return true;
    }

    private void ReallocateTouches(int newCount)
    {
    	Touch []newTouches = new Touch[newCount];
    	if(null != newTouches)
    	{
        	for(int i = 0; i < newCount; ++i)
        	{
        		newTouches[i] = new Touch();
        		if((i < allocatedTouches) && (null != previousTouches))
        		{
        			newTouches[i].Copy(previousTouches[i]);
        		}
        	}
        	
        	allocatedTouches = newCount;
        	previousTouches = newTouches;
        	newTouches = null;
    	}
    }
    
    private Touch FindPreviousTouch(int id)
    {
    	Touch touch = null;
    	for(int i = 0; i < previousCount; ++i)
    	{
    		if(previousTouches[i].id == id)
    		{
    			touch = previousTouches[i];
    			break;
    		}
    	}
    	
    	return touch;
    }
    
    private Touch GetNextFreeTouch()
    {
    	Touch touch = null;
    	for(int i = previousCount; i < allocatedTouches; ++i)
    	{
    		if(!previousTouches[i].active)
    		{
    			touch = previousTouches[i];
    			break;
    		}
    	}
    	
    	if(null == touch)
    	{
    		ReallocateTouches(allocatedTouches + TOUCH_DELTA);
    		return GetNextFreeTouch();
    	}
    	
    	return touch;
    }
    
    private void ClearNotActiveTouches()
    {
    	for(int i = 0; i < allocatedTouches; ++i)
    	{
    		if(!previousTouches[i].active)
    		{
    			for(int j = i; j < allocatedTouches - 1; ++j)
    			{
    				previousTouches[j].Copy(previousTouches[j + 1]);
    			}
    		}
    	}
    }
    
    
    protected class Touch
    {
    	protected int id = 0;
    	protected int x = 0;
    	protected int y = 0;
    	protected int action = 0;
    	protected boolean active = false; 
    	
    	void Copy(Touch inTouch)
    	{
    		if(null != inTouch)
    		{
        		id = inTouch.id;
        		action = inTouch.action;
        		x = inTouch.x;
        		y = inTouch.y;
        		active = inTouch.active;
    		}
    	}
    	
    	void Clear()
    	{
    		id = 0;
    		x = 0;
    		y = 0;
    		action = 0;
    		active = false;
    	}
    }
}
