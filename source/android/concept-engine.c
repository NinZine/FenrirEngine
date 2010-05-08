#include <jni.h>
#include "com_company_ceandroid_ConceptEngine.h"

#include "system.h"

static char *argv[] = {"conceptengine.apt", "/data/local/temp/image.lua"};

JNIEXPORT void JNICALL Java_com_company_ceandroid_ConceptEngine_step
  (JNIEnv *env, jclass obj)
{
	if( sys_update() != 0 ) 
		sys_quit();
}

JNIEXPORT void JNICALL Java_com_company_ceandroid_ConceptEngine_init
  (JNIEnv *env, jclass obj, jint width, jint height)
{
	sys_start(2, argv);
}
