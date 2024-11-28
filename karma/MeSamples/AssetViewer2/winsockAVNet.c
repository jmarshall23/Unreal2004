/* -*- mode: C; -*- */

/*
Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $
  
    Date: $Date: 2002/04/04 15:29:08 $ - Revision: $Revision: 1.7.2.1 $
    
      This software and its accompanying manuals have been developed
      by Mathengine PLC ("MathEngine") and the copyright and all other
      intellectual property rights in them belong to MathEngine. All
      rights conferred by law (including rights under international
      copyright conventions) are reserved to MathEngine. This software
      may also incorporate information which is confidential to
      MathEngine.
      
        Save to the extent permitted by law, or as otherwise expressly
        permitted by MathEngine, this software and the manuals must not
        be copied (in whole or in part), re-arranged, altered or adapted
        in any way without the prior written consent of the Company. In
        addition, the information contained in the software may not be
        disseminated without the prior written consent of MathEngine.
        
*/
#ifdef WIN32

#include "AssetViewer2.h"

#include "instances.h"
#include "initialization.h"
#include "simulation.h"
#include "visualization.h"
#include "commandline.h"
#include "network.h"
#include "winsockAVNet.h"

#include <winsock2.h>

#define PORT 4000            
#define Max_Connections 1  

SOCKET ListeningSocket;

int MEAPI AVWinsockInit(void)
{
    WSADATA WSAData;
    TCHAR szError[100];                 

    /* Initialize Winsock */
    if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0) 
    {
        wsprintf (szError, TEXT("WSAStartup failed. Error: %d"), 
            WSAGetLastError ());
        MessageBox (NULL, szError, TEXT("Error"), MB_OK);
        return 1;
    }
    
    return CreateServerSocket();
}

int CreateServerSocket()
{
    TCHAR szError[100];                 
    unsigned long ulongval;
    struct sockaddr_in Server_Address;
    
    Server_Address.sin_family = AF_INET;
    Server_Address.sin_addr.s_addr = INADDR_ANY;
    Server_Address.sin_port = htons(PORT);
    
    /* Create a Socket */
    ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ListeningSocket == INVALID_SOCKET)
    {
        wsprintf (szError, TEXT("socket failed. Error: %d"), 
            WSAGetLastError ());
        MessageBox (NULL, szError, TEXT("Error"), MB_OK);
        return 1;
    }
    
    /* Make socket non-blocking */
    ulongval = 1;
    if( ioctlsocket(ListeningSocket, FIONBIO, &ulongval) )
    {
        wsprintf (szError, TEXT("ioctlsocket failed. Error: %d"), 
            WSAGetLastError ());
        MessageBox (NULL, szError, TEXT("Error"), MB_OK);
        return 1;
    }
    
    if( bind(ListeningSocket, (struct sockaddr *)&Server_Address, sizeof(Server_Address)) )
    {
        wsprintf (szError, TEXT("bind failed. Error: %d"), 
            WSAGetLastError ());
        MessageBox (NULL, szError, TEXT("Error"), MB_OK);
        return 1;
    }
    
    if( listen(ListeningSocket, SOMAXCONN) )
    {
        wsprintf (szError, TEXT("listen failed. Error: %d"), 
            WSAGetLastError ());
        MessageBox (NULL, szError, TEXT("Error"), MB_OK);
        return 1;
    }
    
    return 0; /* success */
}

void MEAPI AVWinsockCleanup(void)
{
    shutdown(ListeningSocket, SD_SEND);
    closesocket(ListeningSocket);
    WSACleanup();
}

void MEAPI AVWinsockPoll(AVCommandParseFunc parseFunc)
{
    SOCKET clientSocket;
    MeStream stream;
    
    clientSocket = accept(ListeningSocket, 0,0);
    if(clientSocket != INVALID_SOCKET)
    {
        /* got a connection */
        char TERM = (char)255;
        char buf[256];
        int charsRead = 0;
        MeBool bExitLoop = 0;

        shutdown(ListeningSocket,SD_SEND);
        closesocket(ListeningSocket);    
        
        stream = MeStreamOpenAsMemBuffer(256);
        
        while(!bExitLoop) 
        {
            charsRead = recv(clientSocket, buf, 256, 0);
            if( charsRead == SOCKET_ERROR )
            {
                if(WSAGetLastError() != WSAEWOULDBLOCK )
                    break;
            }
            else
            {
                if(charsRead == 0)
                    break;

                MeStreamWrite(buf, charsRead, 1, stream);
                
                if(stream->buffer[stream->bufLength-1] == TERM)
                {
                    stream->bufLength--;
                    /* process command */
                    if(stream->buffer[0] == 'q' && stream->bufLength == 1)
                    {
                        bExitLoop = 1;
                        while(charsRead = recv(clientSocket,buf,256,0))
                        {if(charsRead==SOCKET_ERROR)break;}
                        send(clientSocket, &TERM, 1, 0);                        
                    }
                    else
                    {
                        char* retString;
                        int   retStrLen;
                        
                        parseFunc(stream->buffer, stream->bufLength, &retString, &retStrLen);
                        
                        if(retStrLen != 0 && retString)
                        {
                            /* send return string */
                            send(clientSocket, retString, retStrLen, 0);                
                            MeMemoryAPI.destroy(retString);
                        }                        
                        send(clientSocket, &TERM, 1, 0);                        
                    }
                    
                    /* acknowledge command has been processed */
                    MeStreamClose(stream);
                    stream = MeStreamOpenAsMemBuffer(256);
                 }
            }
        }

        MeStreamClose(stream);        
        shutdown(clientSocket,SD_SEND);
        closesocket(clientSocket);

        CreateServerSocket();
    }
}



#endif