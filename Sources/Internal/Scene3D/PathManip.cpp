/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#include "Scene3D/PathManip.h"

namespace DAVA
{

	PathManip::PathManip(const char * src)
	{
		splitToEntries(src);	
	}

	/*
	PathManip::PathManip(const PathManip& orig) {
	}

	PathManip::~PathManip() {
	}
	 */


	void PathManip::splitToEntries(const char * src)
	{
		pathEntries.clear();

		String s(src);
		size_t pos = 0;
		bool finish = false;

		while(finish == false)
		{
			size_t n = s.find("/", pos);

			if (n == String::npos)
			{
				finish = true;
				n = s.length();
			}

			size_t len = n-pos;

			if (len)
			{
				pathEntries.push_back(s.substr(pos, len));
			}
			else if (n == 0)
			{
				pathEntries.push_back("/");
			}

			pos = n+1;

			if (pos >= s.length())
				finish = true;
		}
	}

	String PathManip::getSuffix()
	{
		if (pathEntries.size() == 0)
			return "";

		String & s = pathEntries.back();
		size_t n = s.rfind('.');
		if (n == String::npos)
			return "";
		
		return s.substr(n, s.length()-n);
	}

	void PathManip::setSuffix(const String & suf)
	{
		if (pathEntries.size() == 0)
		{
			pathEntries.push_back(suf);
			return;
		}

		String & s = pathEntries.back();
		size_t n = s.rfind('.');
		if (n == String::npos)
		{
			s += suf;
			return;
		}

		String news = s.substr(0, n);
		news += suf;
		pathEntries.pop_back();
		pathEntries.push_back(news);
	}

	String PathManip::GetName()
	{
		if (pathEntries.size())
		{
			return pathEntries.back();
		}

		return "";
	}

	String PathManip::GetPath()
	{
		if (pathEntries.size() == 0)
		{
			return "";
		}

		String s;
		size_t n = 0;
		size_t last = pathEntries.size()-1;

		for(List<String>::iterator i = pathEntries.begin(); n < last; ++i, ++n)
		{
			if ((*i) == "/")
			{
				s += (*i);
			}
			else
			{
				s += (*i) + "/";
			}
		}

		return s;
	}

	String PathManip::GetString()
	{
		if (pathEntries.size() == 0)
		{
			return "";
		}

		String s;
		size_t n = 0;
		size_t last = pathEntries.size()-1;
		
		for(List<String>::iterator i = pathEntries.begin(); i != pathEntries.end(); ++i, ++n)
		{
			s += (*i);

			if ((n != last) && ((*i) != "/"))
			{
				s += "/";
			}
		}

		return s;
	}


} // ns
