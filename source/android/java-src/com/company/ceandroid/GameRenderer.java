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

   public boolean forwardCalls;
   public String filename;

   public GameRenderer()
   {
      filename = "";
      forwardCalls = false;
   }

   public void onSurfaceCreated(GL10 gl, EGLConfig config)
   {
      Log.d("ConceptEngine", "SurfaceCreated");
      if(filename.length() > 0)
      {
         ConceptEngine.run(filename);
         forwardCalls = true;
      }
   }

   public void onDrawFrame(GL10 gl)
   {
      if (forwardCalls)
         ConceptEngine.step();
   }

   public void onSurfaceChanged(GL10 gl, int width, int height)
   {
      Log.d("ConceptEngine", "onSurfaceChange(gl, " + width + "," + height + ")");
      if (forwardCalls)
         ConceptEngine.resize(width, height);
   }
}
