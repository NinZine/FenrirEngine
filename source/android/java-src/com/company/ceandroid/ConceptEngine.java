package com.company.ceandroid;

public class ConceptEngine {

    static {
        System.loadLibrary("concept-engine");
    }
    
    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init(int width, int height);
     public static native void step();
}
