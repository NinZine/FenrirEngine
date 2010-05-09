#include <jni.h>
#include "com_company_ceandroid_ConceptEngine.h"

#include "log.h"
#include "system.h"

static char *argv[] = {"conceptengine.apt", "/data/local/temp/image.lua"};
//static char *argv[] = {"conceptengine.apt", "/data/data/com.company.ceandroid/files/image.lua"};

static int runCalled = 0;

JNIEXPORT void JNICALL Java_com_company_ceandroid_ConceptEngine_run
  (JNIEnv *env, jclass obj, jstring filename)
{
	char* str;
	str = (char*)(*env)->GetStringUTFChars(env, filename, 0);
	if( str == 0)
	{
		log_print("Failed to turn filename into to char array");
		return;
	}

	char *argv[] = {"conceptengine.apt", str};
	sys_start(2, argv);
	runCalled = 1;

	(*env)->ReleaseStringUTFChars(env, filename, str);
}

JNIEXPORT void JNICALL Java_com_company_ceandroid_ConceptEngine_step
  (JNIEnv *env, jclass obj)
{
	if(runCalled != 0)
	{
		if( sys_update() != 0 )
			sys_quit();
	}
}

JNIEXPORT void JNICALL Java_com_company_ceandroid_ConceptEngine_resize
  (JNIEnv *env, jclass obj, jint width, jint height)
{
	// resize me!
}
