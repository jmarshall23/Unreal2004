#include "utvremotecontroll.h"

UTVRemoteControll::UTVRemoteControll(INT port)
{
	BindPort(port);
}

UTVRemoteControll::~UTVRemoteControll(void)
{
}

void UTVRemoteControll::Tick(void)
{
	Poll();
	while(WaitingClients.Size()>10)
		WaitingClients.PopFront();
}

void UTVRemoteControll::OnReceivedData( FIpAddr SrcAddr, BYTE* Data, INT Count )
{
	if(Data[0]==255 && Data[1]==255 && Data[2]==255 && Data[3]==255){
		debugf(TEXT("Got 0xff udp data"));
		if(Count>=14 && memcmp(&Data[4],"challenge rcon\n",14)==0){
			DWORD Challenge=rand();		//fix?

			WaitingClient wc;
			wc.address=SrcAddr;
			wc.ChallengeGiven=Challenge;
			WaitingClients.PushBack(wc);

			char Response[50];
			sprintf(Response,"xxxxchallenge rcon %u\n",Challenge);
			*(DWORD*)Response=0xffffffff;
			SendTo(SrcAddr,(BYTE*)Response,strlen(Response));

		} else if(Count>=9 && memcmp(&Data[4],"rcon ",5)==0){
			char* tempBuf=new char[Count];
			memcpy(tempBuf,&Data[9],Count-9);
			tempBuf[Count-9]=0;
			FString s(tempBuf);
			delete[] tempBuf;

			INT pos;
			FString Challenge;
			for(pos=0;pos<Count-9;++pos){
				if(s[pos]==TEXT(' '))
					break;
				Challenge+=s.Mid(pos,1);
			}
			DWORD remoteChallenge=appAtoi(*Challenge);

			bool challengeOk=false;
			for(UTVList<WaitingClient>::TIterator wci(WaitingClients);wci;++wci){
				if(wci->address==SrcAddr && wci->ChallengeGiven==remoteChallenge){
					challengeOk=true;
					break;
				}
			}
			if(!challengeOk)
				return;
			FString password;
			if(s[++pos]!='\"')
				return;
			for(++pos;pos<Count-9;++pos){
				if(s[pos]==TEXT('\"'))
					break;
				password+=s.Mid(pos,1);
			}
			if(password!=UtvEngine->PrimaryPassword)
				return;

			FString command=s.Mid(++pos);
			debugf(TEXT("Rcon request ok, Command %s"),command);
			UtvEngine->ParseCmdLine(*command);			
		}
	}
}
