package com.company.ceandroid;

public class ConceptEngine
{

   static
   {
      System.loadLibrary("concept-engine");
   }

   public static native void run(String filename);

   public static native void resize(int width, int height);

   public static native void step();
}
