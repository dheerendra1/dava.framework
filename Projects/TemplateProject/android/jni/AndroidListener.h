#ifndef __ANDROID_LISTENER_H__
#define __ANDROID_LISTENER_H__

#include <jni.h>
#include "Platform/TemplateAndroid/CorePlatformAndroid.h"

class AndroidListener: public DAVA::AndroidSystemListener
{
	JNIEnv* environment;

	jobject classApplication;
	jobject classActivity;

	enum eConst
	{
		MAX_PATH_SZ = 260
	};

	char activityName[MAX_PATH_SZ];
	char httpDownloaderName[MAX_PATH_SZ];

public:

	AndroidListener(JNIEnv* env);

	void SetApplication(jobject app, char *packageName);

	void SetActivity(jobject activity);

	virtual void ShowKeyboard();
	virtual void HideKeyboard();
	virtual bool DownloadHttpFile(const DAVA::String & url, const DAVA::String & documentsPathname);
};

#endif //#ifndef __ANDROID_LISTENER_H__
