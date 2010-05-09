package com.company.ceandroid;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.util.Log;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.os.Bundle;
import android.view.View;
import android.widget.ListView;
import android.widget.ArrayAdapter;

public class FileList extends ListActivity
{

   private enum DISPLAYMODE
   {

      ABSOLUTE, RELATIVE;
   }
   private final DISPLAYMODE displayMode = DISPLAYMODE.RELATIVE;
   private List<String> directoryEntries = new ArrayList<String>();
   private File currentDirectory = new File("/");

   @Override
   public void onCreate(Bundle savedInstanceState)
   {
      super.onCreate(savedInstanceState);

      ListView lv = new ListView(this);
      lv.setId(android.R.id.list);
      setContentView(lv);

      browseToRoot();
   }

   @Override
   protected void onListItemClick(ListView l, View v, int position, long id)
   {
      int selectionRowID = (int) id;

      String selectedFileString = this.directoryEntries.get(selectionRowID);
      if (selectedFileString.equals("."))
      {
         this.browseTo(this.currentDirectory);
      }
      else if (selectedFileString.equals(".."))
      {
         upOneLevel();
      }
      else
      {
         File clickedFile = null;
         switch (this.displayMode)
         {
            case RELATIVE:
               clickedFile = new File(this.currentDirectory.getAbsolutePath()
                       + this.directoryEntries.get(selectionRowID));
               break;
            case ABSOLUTE:
               clickedFile = new File(this.directoryEntries.get(selectionRowID));
               break;
         }
         if (clickedFile != null)
            browseTo(clickedFile);
      }
   }

   private void browseToRoot()
   {
      browseTo(new File("/data/data/com.company.ceandroid/files/"));
   }

   private void upOneLevel()
   {
      if (this.currentDirectory.getParent() != null)
         this.browseTo(this.currentDirectory.getParentFile());
   }

   private void browseTo(final File aDirectory)
   {
      if (aDirectory.isDirectory())
      {
         this.currentDirectory = aDirectory;
         fill(aDirectory.listFiles());
      }
      else
      {
         OnClickListener okButtonListener = new OnClickListener()
         {
            // @Override

            public void onClick(DialogInterface arg0, int arg1)
            {
               // Lets start an intent to View the file, that was clicked...
               //openFile(aDirectory);

               getIntent().putExtra("filename", aDirectory.getAbsolutePath());
               setResult(RESULT_OK, getIntent());
               finish();
            }
         };
         OnClickListener cancelButtonListener = new OnClickListener()
         {
            // @Override

            public void onClick(DialogInterface arg0, int arg1)
            {
               // Do nothing
            }
         };
         ShowConfirmation("Question",
                 "Open file: " + aDirectory.getName() + "?",
                 okButtonListener, cancelButtonListener);
      }
   }

   private void fill(File[] files)
   {
      this.directoryEntries.clear();

      this.directoryEntries.add(".");
      if (this.currentDirectory.getParent() != null)
         this.directoryEntries.add("..");

      switch (this.displayMode)
      {
         case ABSOLUTE:
            for (File file : files)
            {
               this.directoryEntries.add(file.getPath());
            }
            break;
         case RELATIVE: // On relative Mode, we have to add the current-path to the beginning
            int currentPathStringLenght = this.currentDirectory.getAbsolutePath().length();
            for (File file : files)
            {
               this.directoryEntries.add(file.getAbsolutePath().substring(currentPathStringLenght));
            }
            break;
      }

      setListAdapter(new ArrayAdapter<String>(this,
              android.R.layout.simple_list_item_1, this.directoryEntries));
   }

   private void ShowConfirmation(String title, String msg, OnClickListener ok, OnClickListener cancel)
   {
      AlertDialog a = new AlertDialog.Builder(this).create();
      a.setTitle(title);
      a.setMessage(msg);
      a.setButton("OK", ok);
      a.setButton2("Cancel", cancel);
      a.show();
   }
}
