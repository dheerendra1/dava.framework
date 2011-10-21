package com.dava.templateproject;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

import org.apache.http.util.ByteArrayBuffer;

import android.util.Log;

public class JNIHttpDownloader
{
	public static boolean DownloadFileFromUrl(String fileUrl, String fileName)
	{
        Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] fileUrl is " + fileUrl);
        Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] fileName is " + fileName);

        boolean ret = false;
        try 
        {
            URL url = new URL(fileUrl); 
            File file = new File(fileName);

            long startTime = System.currentTimeMillis();
            Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] download begining");
            Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] download url:" + url);
            Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] downloaded file name:" + fileName);
            
            /* Open a connection to that URL. */
            URLConnection ucon = url.openConnection();

            /*
             * Define InputStreams to read from the URLConnection.
             */
            InputStream is = ucon.getInputStream();
            BufferedInputStream bis = new BufferedInputStream(is);

            /*
             * Read bytes to the Buffer until there is nothing more to read(-1).
             */
            ByteArrayBuffer baf = new ByteArrayBuffer(50);
            
            int current = 0;
            while ((current = bis.read()) != -1) 
            {
            	baf.append((byte) current);
            }

            /* Convert the Bytes read to a String. */
            FileOutputStream fos = new FileOutputStream(file);
            fos.write(baf.toByteArray());
            fos.close();
            Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] download ready in"
                            + ((System.currentTimeMillis() - startTime) / 1000)
                            + " sec");
            Log.d(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] size is " + baf.length());

            ret = true;
        } 
        catch (Exception e) 
        {
        	Log.e(JNIConst.LOG_TAG, "[JNIHttpDownloader.DownloadFileFromUrl] " + e);
        	ret = false;
        }
        
        return ret;
	}
}
