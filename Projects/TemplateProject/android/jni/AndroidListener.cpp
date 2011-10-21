#include "AndroidListener.h"
#include "FileSystem/Logger.h"

AndroidListener::AndroidListener(JNIEnv* env)
{
	environment = env;
	classActivity = NULL;
	classApplication = NULL;
}

void AndroidListener::SetApplication(jobject app, char *packageName)
{
	classApplication = app;

	strcpy(activityName, packageName);
	strcat(activityName, ".JNIActivity");

	strcpy(httpDownloaderName, packageName);
	strcat(httpDownloaderName, ".JNIHttpDownloader");
}

void AndroidListener::SetActivity(jobject activity)
{
	classActivity = activity;
}

void AndroidListener::ShowKeyboard()
{
	if(environment)
	{
		jclass cls = environment->FindClass(activityName);
		jmethodID mid = environment->GetMethodID(cls, "ShowKeyboard", "()V");
		if(mid && classActivity)
		{
			environment->CallVoidMethod(classActivity, mid);
		}
		else
		{
			DAVA::Logger::Error("[AndroidListener::ShowKeyboard] Can't find method");
		}

		environment->DeleteLocalRef(cls);
	}
}

void AndroidListener::HideKeyboard()
{
	if(environment)
	{
		jclass cls = environment->FindClass(activityName);
		jmethodID mid = environment->GetMethodID(cls, "HideKeyboard", "()V");

		if(mid && classActivity)
		{
			environment->CallVoidMethod(classActivity, mid);
		}
		else
		{
			DAVA::Logger::Error("[AndroidListener::HideKeyboard] Can't find method");
		}

		environment->DeleteLocalRef(cls);	
	}
}

bool AndroidListener::DownloadHttpFile(const DAVA::String & url, const DAVA::String & documentsPathname)
{
	DAVA::Logger::Debug("[AndroidListener::DownloadHttpFile] url=%s", url.c_str());
	DAVA::Logger::Debug("[AndroidListener::DownloadHttpFile] docpath=%s", documentsPathname.c_str());

	bool retValue = false;
	if(environment)
	{
		jclass cls = environment->FindClass(httpDownloaderName);
		if(cls)
		{
			jmethodID mid = environment->GetStaticMethodID(cls, "DownloadFileFromUrl", "(Ljava/lang/String;Ljava/lang/String;)Z");

			if(mid)
			{
				jstring jstrUrl = environment->NewStringUTF(url.c_str());
				jstring jstrPath = environment->NewStringUTF(documentsPathname.c_str());

				retValue = environment->CallStaticBooleanMethod(cls, mid, jstrUrl, jstrPath);
			}
			else
			{
				DAVA::Logger::Error("[AndroidListener::DownloadHttpFile] Can't find method");
			}

			environment->DeleteLocalRef(cls);	
		}
	}
	return retValue;
}
