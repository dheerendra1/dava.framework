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
        * Created by Alexey 'Hottych' Prosin
=====================================================================================*/

#ifndef __DAVAENGINE_NETWORK_CONNECTION_H__
#define __DAVAENGINE_NETWORK_CONNECTION_H__

#include "Base/BaseObject.h"


namespace DAVA
{

	class NetworkPacket;
	class NetworkDelegate;
	
	class NetworkConnection : public BaseObject
	{
	public:
		enum eNetworkConnectionType
		{
			NETWORK_CONNECTION_WIFI = 0,
			NETWORK_CONNECTION_BLUETOOTH
		};
		
	protected:
		virtual ~NetworkConnection();
		NetworkConnection(eNetworkConnectionType connectionType);
		
		int type;
		
		NetworkDelegate *delegate;
		
	public:
		
		static NetworkConnection* Create(eNetworkConnectionType connectionType);
	
	
		int GetConnectionType()
		{
			return type;
		}
		
		virtual void StartServerWithName(WideString &name) = 0;
		virtual void StartClient() = 0;
		virtual void Stop() = 0;
		virtual const Vector<WideString> &ListServers() = 0;
		virtual void ConnectToServer(int index) = 0;
		virtual void SendPacket(NetworkPacket* packet) = 0;

		virtual void SetDelegate(NetworkDelegate *newDelegate) = 0;

	};

};










#endif