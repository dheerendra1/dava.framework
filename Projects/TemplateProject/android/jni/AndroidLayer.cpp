#ifndef _ANDROID_LAYER_
#define _ANDROID_LAYER_

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "Platform/TemplateAndroid/AndroidSpecifics.h"
#include "Platform/TemplateAndroid/CorePlatformAndroid.h"
#include "FileSystem/Logger.h"
#include "Utils/Utils.h"
#include "Input/AccelerometerAndroid.h"
#include "AndroidListener.h"

#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, androidLogTag, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, androidLogTag, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, androidLogTag, __VA_ARGS__)


extern "C"
{
	//JNIApplication
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIApplication_OnCreateApplication(JNIEnv* env, jobject classthis, jstring path, jstring apppath, jstring logTag, jstring packageName);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIApplication_OnConfigurationChanged(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIApplication_OnLowMemory(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIApplication_OnTerminate(JNIEnv * env, jobject classthis);

	//FrameworkTestProject
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIActivity_nativeOnCreate(JNIEnv * env, jobject classthis, jboolean isFirstRun);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIActivity_nativeOnStart(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIActivity_nativeOnStop(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIActivity_nativeIsFinishing(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIActivity_nativeOnDestroy(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIActivity_nativeOnAccelerometer(JNIEnv * env, jobject classthis, jfloat x, jfloat y, jfloat z);

	//JNIGLSurfaceView
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnTouch(JNIEnv * env, jobject classthis, jint action, jint id, jint x, jint y, jlong time);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnKeyUp(JNIEnv * env, jobject classthis, jint keyCode);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnResumeView(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnPauseView(JNIEnv * env, jobject classthis);

	//JNIRenderer
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIRenderer_nativeResize(JNIEnv * env, jobject classthis, jint w, jint h);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIRenderer_nativeRender(JNIEnv * env, jobject classthis);
	JNIEXPORT void JNICALL Java_com_dava_templateproject_JNIRenderer_nativeRenderRecreated(JNIEnv * env, jobject classthis);
};


DAVA::CoreAndroidPlatform *core = NULL;

#define MAX_PATH_SZ 260
char documentsFolderPath[MAX_PATH_SZ];
char folderDocuments[MAX_PATH_SZ];
char assetsFolderPath[MAX_PATH_SZ];
char androidLogTag[MAX_PATH_SZ];
char androidPackageName[MAX_PATH_SZ];

AndroidListener *androidListener;

bool CreateDocumentsPath(JNIEnv* env, jstring path)
{
	bool ret = false;

	const char* utfString = env->GetStringUTFChars(path, NULL);
	if (utfString)
	{
		strcpy(documentsFolderPath, utfString);
		env->ReleaseStringUTFChars(path, utfString);

		strcpy(folderDocuments, documentsFolderPath);
		strcat(folderDocuments, "/Documents");

		DIR *folder = opendir(folderDocuments);
		if(folder)
		{
			ret = true;
		}
		else
		{
// 				S_IRGRP - Read permission for the file's group.
// 				S_IROTH - Read permission for users other than the file owner.
// 				S_IRUSR - Read permission for the file owner.
// 				S_IRWXG - Read, write, and search or execute permission for the file's group. S_IRWXG is the bitwise inclusive-OR of S_IRGRP, S_IWGRP, and S_IXGRP.
// 				S_IRWXO - Read, write, and search or execute permission for users other than the file owner. S_IRWXO is the bitwise inclusive-OR of S_IROTH, S_IWOTH, and S_IXOTH.
// 				S_IRWXU - Read, write, and search, or execute, for the file owner; S_IRWXG is the bitwise inclusive-OR of S_IRUSR, S_IWUSR, and S_IXUSR.
// 				S_ISGID - Privilege to set group ID (GID) for execution. When this file is run through an exec function, the effective group ID of the process is set to the group ID of the file. The process then has the same authority as the file owner, rather than the authority of the actual invoker.
// 				S_ISUID - Privilege to set the user ID (UID) for execution. When this file is run through an exec function, the effective user ID of the process is set to the owner of the file. The process then has the same authority as the file owner, rather than the authority of the actual invoker.
// 				S_ISVTX - Indicates shared text. Keep loaded as an executable file in storage.
// 				S_IWGRP - Write permission for the file's group.
// 				S_IWOTH - Write permission for users other than the file owner.
// 				S_IWUSR - Write permission for the file owner.
// 				S_IXGRP - Search permission (for a directory) or execute permission (for a file) for the file's group.
// 				S_IXOTH - Search permission for a directory, or execute permission for a file, for users other than the file owner.
// 				S_IXUSR - Search permission (for a directory) or execute permission (for a file) for the file owne
			int retMake = mkdir(folderDocuments, S_IRWXG|S_IRWXO|S_IRWXU );//S_IRWXU|S_IRGRP|S_IXGRP);
			ret = (0 == retMake);
		}
	}
	else
	{
		LOGE("[CreateDocumentsPath] Can't create utf-string from path");
	}

	return ret;
}

bool CreateStringFromJni(JNIEnv* env, jstring jniString, char *generalString)
{
	bool ret = false;

	generalString[0] = 0;
	const char* utfString = env->GetStringUTFChars(jniString, NULL);
	if (utfString)
	{
		strcpy(generalString, utfString);
		env->ReleaseStringUTFChars(jniString, utfString);
		ret = true;
	}
	else
	{
		LOGE("[CreateStringFromJni] Can't create utf-string from jniString");
	}

	return ret;
}


//bool CreateAssetsPath(JNIEnv* env, jstring path)
//{
//	bool ret = false;
//
//	const char* utfString = env->GetStringUTFChars(path, NULL);
//	if (utfString)
//	{
//		strcpy(assetsFolderPath, utfString);
//		env->ReleaseStringUTFChars(path, utfString);
//		ret = true;
//	}
//	else
//	{
//		LOGE("[CreateDocumentsPath] Can't create utf-string from path");
//	}
//
//	return ret;
//}
//
//bool CreateLogTag(JNIEnv* env, jstring logTag)
//{
//	bool ret = false;
//
//	const char* utfString = env->GetStringUTFChars(logTag, NULL);
//	if (utfString)
//	{
//		strcpy(androidLogTag, utfString);
//		env->ReleaseStringUTFChars(logTag, utfString);
//		ret = true;
//	}
//	else
//	{
//		LOGE("[CreateLogTag] Can't create utf-string from path");
//	}
//
//	return ret;
//}
//
//bool CreatePackageName(JNIEnv* env, jstring packageName)
//{
//	bool ret = false;
//
//	const char* utfString = env->GetStringUTFChars(packageName, NULL);
//	if (utfString)
//	{
//		strcpy(androidPackageName, utfString);
//		env->ReleaseStringUTFChars(packageName, utfString);
//		ret = true;
//	}
//	else
//	{
//		LOGE("[CreatePackageName] Can't create utf-string from path");
//	}
//
//	return ret;
//}


void InitApplication(JNIEnv * env)
{
	if(!core)
	{
		//TODO: VK: think about
// 		char *argv[] =
// 		{
// 			(char *)documentsFolderPath,
// 			(char *)assetsFolderPath
// 		}
// 		int argc = 2;
// 		DAVA::Core::Run(argc, argv, 0);


		core = new DAVA::CoreAndroidPlatform();
		if(core)
		{
			androidListener = new AndroidListener(env);
			core->CreateAndroidWindow(documentsFolderPath, assetsFolderPath, androidLogTag, androidListener);
		}
		else
		{
			LOGE("[InitApplication] Can't allocate space for CoreAndroidPlatform");
		}
	}
	else
	{
		DAVA::Logger::Warning("[InitApplication] CoreAndroidPlatform has been created");
	}
}

void DeinitApplication()
{
	if(core)
	{
		core->Quit();
		core->ReleaseSingletons();
		core = NULL;
	}

	SafeDelete(androidListener);
}

// CALLED FROM JNIApplication
// private static native void OnCreateApplication();
// private static native void OnConfigurationChanged();
// private static native void OnLowMemory();
// private static native void OnTerminate()

void Java_com_dava_templateproject_JNIApplication_OnCreateApplication(JNIEnv* env, jobject classthis, jstring path, jstring apppath, jstring logTag, jstring packageName)
{
	bool retCreateLogTag = CreateStringFromJni(env, logTag, androidLogTag);
	LOGI("___ OnCreateApplication __ %d", classthis);

	bool retCreatedDocuments = CreateDocumentsPath(env, path);
	bool retCreatedAssets = CreateStringFromJni(env, apppath, assetsFolderPath);
	bool retCreatePackageName = CreateStringFromJni(env, packageName, androidPackageName);

	InitApplication(env);
	if(androidListener)
	{
		androidListener->SetApplication(classthis, androidPackageName);
	}
}

void Java_com_dava_templateproject_JNIApplication_OnConfigurationChanged(JNIEnv * env, jobject classthis)
{
	if(core)
	{
		DAVA::Logger::Info("__ CONFIGURATION CHANGED ___  %p", env);
	}
}
void Java_com_dava_templateproject_JNIApplication_OnLowMemory(JNIEnv * env, jobject classthis)
{
	if(core)
	{
		DAVA::Logger::Info("__ LOW MEMORY ___  %p", env);
	}

//	DAVA::Logger::Info("-------- DEINIT APPLICATION --------");
//	DeinitApplication();
}
void Java_com_dava_templateproject_JNIApplication_OnTerminate(JNIEnv * env, jobject classthis)
{
	LOGI("___ ON TERMINATE ___");

//	DeinitApplication();
}
// END OF JNIApplication

// CALLED FROM JNIActivity
#include "Utils/HttpDownloader.h"
void Java_com_dava_templateproject_JNIActivity_nativeOnCreate(JNIEnv * env, jobject classthis, jboolean isFirstRun)
{
	LOGI("___ ON CREATE ___ %p, %d;  isFirstRun = %d", env, classthis, isFirstRun);
	if(core)
	{
		if(androidListener)
		{
			androidListener->SetActivity(classthis);
		}

		core->OnCreateActivity();

//		DAVA::DownloadFileFromURLToDocuments("http://seriouswheels.com/pics-2011/def/2011-Edo-Competition-Mercedes-Benz-SLR-Black-Arrow-Exhaust-1024x768.jpg", "~doc:/device.yaml");
	}
}

void Java_com_dava_templateproject_JNIActivity_nativeOnStart(JNIEnv * env, jobject classthis)
{
	LOGI("___ ON START ___ %p, %d", env, classthis);

	if(core)
	{
		core->StartVisible();
	}
}

void Java_com_dava_templateproject_JNIActivity_nativeOnStop(JNIEnv * env, jobject classthis)
{
	LOGI("___ ON STOP ___ %p, %d", env, classthis);

	if(core)
	{
		core->StopVisible();
	}
}

void Java_com_dava_templateproject_JNIActivity_nativeIsFinishing(JNIEnv * env, jobject classthis)
{
	LOGI("___ ON FINISHING ___");
	DeinitApplication();
}


void Java_com_dava_templateproject_JNIActivity_nativeOnDestroy(JNIEnv * env, jobject classthis)
{
	LOGI("___ ON DESTROY ___");
	if(core)
	{
		core->OnDestroyActivity();
//		DeinitApplication();
	}
}

void Java_com_dava_templateproject_JNIActivity_nativeOnAccelerometer(JNIEnv * env, jobject classthis, jfloat x, jfloat y, jfloat z)
{
//	LOGI("___ ON ACC ___ env = %p, %0.4f, %0.4f, %0.4f", env, x,y,z);
	DAVA::AccelerometerAndroidImpl *accelerometer = (DAVA::AccelerometerAndroidImpl *)DAVA::Accelerometer::Instance();
	if(accelerometer)
	{
		accelerometer->SetAccelerationData(x, y, z);
	}
}


// CALLED FROM JNIGLSurfaceView
void Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnTouch(JNIEnv * env, jobject classthis, jint action, jint id, jint x, jint y, jlong time)
{
	if(core)
	{
		core->OnTouch(action, id, x, y, time);
	}
}

void Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnKeyUp(JNIEnv * env, jobject classthis, jint keyCode)
{
	if(core)
	{
		core->KeyUp(keyCode);
	}
}

void Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnResumeView(JNIEnv * env, jobject classthis)
{
	if(core)
	{
		core->StartForeground();
	}
}
void Java_com_dava_templateproject_JNIGLSurfaceView_nativeOnPauseView(JNIEnv * env, jobject classthis)
{
	if(core)
	{
		core->StopForeground();
	}
}


// END OF JNIGLSurfaceView



// CALLED FROM JNIRenderer
// private static native void nativeResize(int w, int h);
// private static native void nativeRender();
//#include "Sound/Sound.h"

void Java_com_dava_templateproject_JNIRenderer_nativeResize(JNIEnv * env, jobject classthis, jint w, jint h)
{
	if(core)
	{
		DAVA::Logger::Debug("__ NATIVE RESIZE ___ %d, %d", w, h);
		core->ResizeView(w, h);

// 		DAVA::Sound *s = DAVA::Sound::Create("~res:/Sound/lake.wav", DAVA::Sound::TYPE_STATIC);
// 		if(s)
// 		{
// 			DAVA::Logger::Debug("sound created");
// 			s->Play();
// 		}
	}
}

void Java_com_dava_templateproject_JNIRenderer_nativeRender(JNIEnv * env, jobject classthis)
{
	if(core)
	{
		core->RepaintView();
	}
}

void Java_com_dava_templateproject_JNIRenderer_nativeRenderRecreated(JNIEnv * env, jobject classthis)
{
	if(core)
	{
		core->RenderRecreated();
	}
}


//END OF activity

#endif //#ifndef _ANDROID_LAYER_
