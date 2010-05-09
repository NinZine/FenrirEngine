package com.company.ceandroid;
import com.company.ceandroid.R;

import android.content.ContextWrapper;
import android.content.Context;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;

/**
 *
 * @author Daniel Dimovski
 */
public class ContentManager
{

   private Context mContext;

   public ContentManager(Context context)
   {
      mContext = context;
   }

   void unpackData() throws FileNotFoundException, IOException
   {
      final int BUFFER = 8192;

      ContextWrapper cw = new ContextWrapper(mContext);
      String privateFilesPath = cw.getFilesDir().getAbsolutePath();

      InputStream datafile = mContext.getResources().openRawResource(R.raw.data);
      if (datafile == null)
         return;

      ZipInputStream zin = new ZipInputStream(new BufferedInputStream(datafile, BUFFER));
      ZipEntry entry;
      while ((entry = zin.getNextEntry()) != null)
      {
         if (!entry.isDirectory())
         {
            String x[] = entry.getName().split("/");

            String filename = x[x.length - 1];
            String filepath = entry.getName().replace(filename, "");

            Log.v("ConceptEngine", "Unpacking " + entry.getName());
            createFile(zin, privateFilesPath + "/" + filepath, filename);
         }
      }
      zin.close();
   }

   private void createFile(InputStream in, String path, String filename) throws IOException
   {
      final int BUFFER = 8192;

      File f = new File(path);
      f.mkdirs();
      f = new File(f, filename);
      f.createNewFile();

      OutputStream out = new FileOutputStream(f);
      BufferedOutputStream dest = new BufferedOutputStream(out, BUFFER);

      int count;
      byte data[] = new byte[BUFFER];
      while ((count = in.read(data, 0, BUFFER)) != -1)
      {
         dest.write(data, 0, count);
      }
      dest.flush();
      dest.close();
   }
}
