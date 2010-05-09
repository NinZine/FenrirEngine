package com.company.ceandroid;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.opengl.GLSurfaceView;
import android.util.Log;

/**
 *
 * @author Daniel Dimovski
 */
public class MainActivity extends Activity
{

   static final int GET_FILENAME = 0;
   private GLSurfaceView glSurface;
   private GameRenderer gameRenderer;

   /** Called when the activity is first created. */
   @Override
   public void onCreate(Bundle savedInstanceState)
   {
      super.onCreate(savedInstanceState);

      // unpack the data
      try
      {
         ContentManager cm = new ContentManager(this);
         cm.unpackData();
      }
      catch (Exception e)
      {
         Log.v("ConceptEngine", "Unpacking data failed");
         e.printStackTrace();
      }

      // Setup render surface
      glSurface = new GameSurfaceView(this);
      gameRenderer = new GameRenderer();
      glSurface.setRenderer(gameRenderer);
      setContentView(glSurface);

      // Show filelist to user
      Intent fileList = new Intent(this, FileList.class);
      Bundle mBundle = new Bundle();
      fileList.putExtras(mBundle);
      startActivityForResult(fileList, GET_FILENAME);
   }

   @Override
   protected void onActivityResult(int requestCode, int resultCode, Intent data)
   {
      Log.i("ConceptEngine", "onActivityResult(" + requestCode + "," + resultCode + "," + data);
      if (requestCode == GET_FILENAME)
      {
         if (resultCode == RESULT_OK)
         {
            String filename = data.getStringExtra("filename");
            Log.i("ConceptEngine", "About to run: " + filename);
            //ConceptEngine.run(filename);
            gameRenderer.filename = filename;

         }
      }
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
