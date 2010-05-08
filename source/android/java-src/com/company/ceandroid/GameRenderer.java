package com.company.ceandroid;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;

/**
 *
 * @author Daniel Dimovski
 */
public class GameRenderer implements Renderer
{
   public GameRenderer()
   {
   }

   public void onSurfaceCreated(GL10 gl, EGLConfig config)
   {
      //Thread t = new Thread() {

        //  public void run() {
		   ConceptEngine.init(300, 300);
			Log.i("ConceptEngine (Java)", "back in java code");
          //    }
      //};
      //t.start();
   }

   public void onDrawFrame(GL10 gl)
   {
        //gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
        ConceptEngine.step();
   }

   public void onSurfaceChanged(GL10 gl, int width, int height)
   {
        //gl.glViewport(0, 0, width, height);
   }
}
