package com.company.ceandroid;

import android.app.Activity;
import android.os.Bundle;

import android.opengl.GLSurfaceView;

/**
 *
 * @author Daniel Dimovski
 */
public class MainActivity extends Activity
{

	private GLSurfaceView glSurface;

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        glSurface = new GameSurfaceView(this);
        glSurface.setRenderer(new GameRenderer());
	glSurface.setDebugFlags (GLSurfaceView.DEBUG_CHECK_GL_ERROR | GLSurfaceView.DEBUG_LOG_GL_CALLS);
	//glSurface.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        setContentView(glSurface);
    }
    

    @Override
    protected void onResume()
    {
       super.onResume();
       glSurface.onResume();
    }

    @Override
    protected void onPause()
    {
       super.onPause();
       glSurface.onPause();
    }
}
