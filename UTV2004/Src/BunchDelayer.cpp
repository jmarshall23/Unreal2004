#include "BunchDelayer.h"
#include "ReplicatorEngine.h"
#include "UTVPackageMap.h"
#include "UtvVoiceChannel.h"

/////////////////////////////////

MoveDelayer mDelayer;

MoveDelayer::MoveDelayer(void)
{
}

void MoveDelayer::Tick(float DeltaSeconds)
{
	while(!WaitingMoves.Empty() && WaitingMoves.Front().OutTime< Delayer->CurrentTime){
		WaitingMove wb=WaitingMoves.Front();
		WaitingMoves.PopFront();

//		debugf (TEXT ("Sending out delayed move: %s"), wb.Move);

		for( INT i=0; i<UtvEngine->ListenDriver->ClientConnections.Num(); i++ ){
			UNetConnection* Connection=UtvEngine->ListenDriver->ClientConnections(i);
			if(UtvEngine->OpenConnections.Find(Connection)->isReady && Connection!=UtvEngine->PrimaryConnection){
				UtvEngine->SendMessageToClient (Connection, wb.Move);
			}
		}
	}

	while(!WaitingActors.Empty() && WaitingActors.Front().OutTime < Delayer->CurrentTime){
		WaitingActor wb=WaitingActors.Front();
		WaitingActors.PopFront();

		for( INT i=0; i<UtvEngine->ListenDriver->ClientConnections.Num(); i++ ){
			UNetConnection* Connection=UtvEngine->ListenDriver->ClientConnections(i);
			if(UtvEngine->OpenConnections.Find(Connection)->isReady && Connection!=UtvEngine->PrimaryConnection){
				UtvEngine->SendActorToClient (Connection, wb.Actor);
			}
		}
	}
}

void MoveDelayer::AddMove(FString Move)
{
	WaitingMove wb;
	wb.Move = Move;
	wb.OutTime=Delayer->CurrentTime+Delayer->DelayTime-5;
	WaitingMoves.PushBack(wb);
}

void MoveDelayer::AddActor (INT Actor)
{
	WaitingActor wb;
	wb.Actor = Actor;
	wb.OutTime=Delayer->CurrentTime+Delayer->DelayTime;
	WaitingActors.PushBack(wb);
}

void MoveDelayer::Restart()
{
	while(!WaitingMoves.Empty()){
		WaitingMoves.PopFront();
	}
	while (!WaitingActors.Empty ()) {
		WaitingActors.PopFront ();
	}
}

/////////////////////////////////

VoiceDelayer vDelayer;

VoiceDelayer::VoiceDelayer(void)
{
}

void VoiceDelayer::Tick(float DeltaSeconds)
{
	while(!WaitingVoices.Empty() && WaitingVoices.Front().OutTime < Delayer->CurrentTime){
		WaitingVoice wv=WaitingVoices.Front();
		WaitingVoices.PopFront();

		if(UtvEngine->PrimaryConnection && UtvEngine->PrimaryConnection->VoiceChannel)
			((UtvVoiceChannel*)UtvEngine->PrimaryConnection->VoiceChannel)->DistributeVoicePacket(&wv.VoiceInfo);

		delete[] wv.VoiceInfo.PacketData;
	}
}

void VoiceDelayer::AddVoice(FVoiceInfo* VoiceInfo)
{
	WaitingVoice wv;
	wv.VoiceInfo = *VoiceInfo;
	wv.VoiceInfo.PacketData=new BYTE[wv.VoiceInfo.PacketSize];
	memcpy(wv.VoiceInfo.PacketData,VoiceInfo->PacketData,wv.VoiceInfo.PacketSize);
	wv.OutTime=Delayer->CurrentTime+Delayer->DelayTime-0.5;
	WaitingVoices.PushBack(wv);
}

void VoiceDelayer::Restart()
{
	while(!WaitingVoices.Empty()){
		delete[] WaitingVoices.Front().VoiceInfo.PacketData;
		WaitingVoices.PopFront();
	}
}

/////////////////////////////////

static inline void SerializeCompVector( FArchive& Ar, FVector& V )
{
	INT X(appRound(V.X)), Y(appRound(V.Y)), Z(appRound(V.Z));
	DWORD Bits = Clamp<DWORD>( appCeilLogTwo(1+Max(Max(Abs(X),Abs(Y)),Abs(Z))), 1, 20 )-1;
	Ar.SerializeInt( Bits, 20 );
	INT   Bias = 1<<(Bits+1);
	DWORD Max  = 1<<(Bits+2);
	DWORD DX(X+Bias), DY(Y+Bias), DZ(Z+Bias);
	Ar.SerializeInt( DX, Max );
	Ar.SerializeInt( DY, Max );
	Ar.SerializeInt( DZ, Max );
	if( Ar.IsLoading() )
		V = FVector((INT)DX-Bias,(INT)DY-Bias,(INT)DZ-Bias);
}

static inline void SerializeCompRotator( FArchive& Ar, FRotator& R )
{
	BYTE Pitch(R.Pitch>>8), Yaw(R.Yaw>>8), Roll(R.Roll>>8), B;
	B = (Pitch!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
		Ar << Pitch;
	B = (Yaw!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
		Ar << Yaw;
	B = (Roll!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
		Ar << Roll;
	if( Ar.IsLoading() )
		R = FRotator(Pitch<<8,Yaw<<8,Roll<<8);
}

// passing NULL for UActorChannel will skip recent property update
static inline void SerializeCompressedInitial( FArchive& Bunch, FVector& Location, FRotator& Rotation, UBOOL bSerializeRotation, UActorChannel* Ch )
{
	guardSlow(SerializeCompressedInitial);
    // read/write compressed location
    SerializeCompVector( Bunch, Location );
    if( Ch && Ch->Recent.Num() )
    {
		//check(Cast<AActor>((UObject*)&Ch->Recent(0)));
        ((AActor*)&Ch->Recent(0))->Location = Location;
    }
    // optionally read/write compressed rotation
    if( bSerializeRotation )
    {
        SerializeCompRotator( Bunch, Rotation );
	    if( Ch && Ch->Recent.Num() )
        {
			//check(Cast<AActor>((UObject*)&Ch->Recent(0)));
            ((AActor*)&Ch->Recent(0))->Rotation = Rotation;
        }
    }
	unguardSlow;
}

/////////////////////////////////

BunchDelayer* Delayer;

BunchDelayer::BunchDelayer(void)
{
	CurrentTime=0;
	DelayTime=0;
	MaxChannelQueSize=5;
//	actors.SetSize (1024);
	ActorProps ap;
	ap.ActorClass=0;
	ap.worthTracking=false;
	for (INT i = 0; i <= UNetConnection::MAX_CHANNELS; ++i) {
		actors[i]=ap;
	}

	//Initialize the list of tracked actors intended for watchers (secondary clients)
	WatcherActor wa;
	wa.ActorClass = NULL;
	for (INT i = 0; i <= UNetConnection::MAX_CHANNELS; ++i) {
//		INT a=WActors.AddZeroed();
		WActors[i]=wa;
		OpenChannels[i]=0;
		DelayedOpenChannels[i]=0;
	}
	debugf (TEXT ("Initializing actor property list"));
}

void BunchDelayer::Destroy(void)
{
	guard(BunchDelayer::Destroy);
	Super::Destroy();
	while(!WaitingBunches.Empty()){
		delete WaitingBunches.Front().Bunch;
		WaitingBunches.PopFront();
	}
	for (INT i = 0; i <= UNetConnection::MAX_CHANNELS; ++i) {
		ClearActor (i);
		WActors[i].ActorClass = NULL;
	}
	unguard;
}

void BunchDelayer::Tick(float DeltaSeconds)
{
	guard(BunchDelayer::Tick);

	CurrentTime=appSeconds();
	while(!WaitingBunches.Empty() && WaitingBunches.Front().OutTime<CurrentTime){
		WaitingBunch wb=WaitingBunches.Front();
		WaitingBunches.PopFront();
		FInBunch* Bunch=wb.Bunch->bunch;

		if(Bunch->bOpen){
			check(DelayedOpenChannels[Bunch->ChIndex]==0);
			DelayedOpenChannels[Bunch->ChIndex]=new OpenPackets;
//			INT a=DelayedOpenChannels[Bunch->ChIndex]->AddZeroed();
			new (*DelayedOpenChannels[Bunch->ChIndex]) InternalBunch(*wb.Bunch);
			OpenChannelOnClients(Bunch->ChIndex);
		} else {
			check(DelayedOpenChannels[Bunch->ChIndex]!=0);
			if(DelayedOpenChannels[Bunch->ChIndex]->Num()<(INT)(MaxChannelQueSize* (wb.Important?8:1))){
//				INT a=DelayedOpenChannels[Bunch->ChIndex]->AddZeroed();
				new (*DelayedOpenChannels[Bunch->ChIndex]) InternalBunch(*wb.Bunch);
			}
		}
		//Parse some extra things
		ParseWatcherProperties (wb.Bunch);

		for( INT i=0; i<UtvEngine->ListenDriver->ClientConnections.Num(); i++ ){
			UNetConnection* Connection=UtvEngine->ListenDriver->ClientConnections(i);
			if(UtvEngine->OpenConnections.Find(Connection)->isReady && Connection!=UtvEngine->PrimaryConnection){
				UtvEngine->SendBunch(Connection,wb.Bunch);
			}
		}
		if(Bunch->bClose){
			check(DelayedOpenChannels[Bunch->ChIndex]!=0);
			delete DelayedOpenChannels[Bunch->ChIndex];
			DelayedOpenChannels[Bunch->ChIndex]=0;
		}

		delete wb.Bunch;
		UtvEngine->SentClientData=true;
	}
	unguard;
}

void BunchDelayer::ClearActor (DWORD index)
{
	guard (BunchDelayer::ClearActor);

	//Clear any old data since this one is new and fine
	for (INT i = 0; i < WActors[index].Data.Num(); ++i) {
		BYTE* d = WActors[index].Data(i);
		if (d && (d != (BYTE *)1))
			delete[] d;
	}
	WActors[index].Data.Empty ();

	unguard;
}

bool BunchDelayer::ParseProperties (InternalBunch* IntBunch)
{
	bool interesting = false;

	guard(BunchDelayer::ParseProperties);
	FInBunch* Bunch=IntBunch->bunch;

	FInBunch tmp(*Bunch);

	//New actor channel?
	if (tmp.bOpen) {
		//Mark it as broken/unused first in case we fail to interpret it
		actors[tmp.ChIndex].ActorClass = NULL;
		actors[tmp.ChIndex].worthTracking = false;

		BYTE B=0; 
		tmp.SerializeBits (&B, 1);
		if (!B) {
			DWORD Index=0;
			tmp.SerializeInt (Index, UtvEngine->ServerPackageMap->GetMaxObjectIndex ());
			UObject* Object;
			Object = UtvEngine->ServerPackageMap->IndexToObject( Index, 1 );
			AActor* InActor = Cast<AActor>( Object );			
			//Now we may have an actor if it was on the level
			if( InActor==NULL )
			{
				// Transient actor.
				UClass* ActorClass = Cast<UClass>( Object );			
				if (ActorClass != NULL) {
					actors[tmp.ChIndex].ActorClass = ActorClass;
				}
				else {
					debugf (TEXT("Null actor in parseproperties"));
					UtvEngine->ServerPackageMap->IndexToObjectName (Index);
					return false;
				}
			} 
			else {
				//A non-transient actor is never interesting
				return false;
			}
		}

		//Check if this actor is something we would like to track
		UClass* actor=actors[tmp.ChIndex].ActorClass;
		if(actor && (actor->IsChildOf(APawn::StaticClass()) || actor->IsChildOf(APlayerController::StaticClass()))){
			actors[tmp.ChIndex].worthTracking=true;
	//		debugf(TEXT("Channel %i is worth tracking"),tmp.ChIndex);
		}
		else {
			//return false;
		}
	}

	//Closing packet?
	if (tmp.bClose) {
		//memory will be cleared when reused.. should perhaps move here.
		actors[tmp.ChIndex].ActorClass = NULL;
		actors[tmp.ChIndex].worthTracking=false;
		actors[tmp.ChIndex].PropertiesUsed.Empty();
	
		//No need do do anything more with this bunch
		return false;
	}

	//can't stop here if we want to check actor dependency
	//if(!actors(tmp.ChIndex).worthTracking/* || !tmp.bReliable*/)
	//return false;

	//Now process properties if we have a valid actor
	if (actors[tmp.ChIndex].ActorClass) {
		FClassNetCache* ClassCache = UtvEngine->ServerPackageMap->GetClassNetCache(actors[tmp.ChIndex].ActorClass);
		INT             RepIndex   = tmp.ReadInt( ClassCache->GetMaxIndex() );
		FFieldNetCache* FieldCache = tmp.IsError() ? NULL : ClassCache->GetFromIndex( RepIndex );
		
		//debugf (TEXT ("Now parsing class %s"), (actors(tmp.ChIndex).ActorClass)->GetName ());
		//debugf (TEXT ("New bunch"));

		while( FieldCache ) {

			if(actors[tmp.ChIndex].PropertiesUsed.Find(RepIndex)==0){
				actors[tmp.ChIndex].PropertiesUsed.Set(RepIndex,1);	//the 1 doesnt matter
				//debugf(TEXT("New property %i on channel %i"),RepIndex,tmp.ChIndex);
				interesting = true;
				//return true;
			}
			
			//Parse the data so we can check for new properties
			UProperty* It;			

			if( FieldCache && (It=FlagCast<UProperty,CLASS_IsAUProperty>(FieldCache->Field))!=NULL )
			{		
				//debugf (TEXT ("Now checking field %s"), It->GetFullName());

				//Is it an array property? get index if so
				BYTE Element=0;
				if( It->ArrayDim != 1 ){
					tmp << Element;
					//debugf (TEXT ("Read array index %i"), Element);
				}

				// Receive property. Just throw away the value.. we only want the bits to get off the bunch
				guard(ReceiveProperty);

				BYTE Data[1024];
				appMemzero (Data, It->ElementSize);
				UtvEngine->ServerPackageMap->ClearDepend ();
				It->NetSerializeItem( tmp, UtvEngine->ServerPackageMap, Data);

				//check for dependency
				if (UtvEngine->ServerPackageMap->GetDepend () > 0) {
//					INT a=IntBunch->dependsOn.AddZeroed();
					new(IntBunch->dependsOn) INT(UtvEngine->ServerPackageMap->GetDepend ());
					//debugf (TEXT ("Now checking field %s"), It->GetFullName());
					//debugf (TEXT ("Depending on %i"), UtvEngine->ServerPackageMap->GetDepend ());
				}

				unguard;
			} 
			else {
				//Just stop now
				return interesting;				
			}

			// Get next
			RepIndex   = tmp.ReadInt( ClassCache->GetMaxIndex() );
			FieldCache = tmp.IsError() ? NULL : ClassCache->GetFromIndex( RepIndex );
		}
	}
	return interesting;
	
	unguard;
}

void BunchDelayer::ParseWatcherProperties (InternalBunch* IntBunch)
{
	guard(BunchDelayer::ParseWatcherProperties);
	FInBunch* Bunch=IntBunch->bunch;
	FInBunch tmp(*Bunch);

	//New actor channel?
	if (tmp.bOpen) {
		guard (OpenChannel);
		//Mark it as broken/unused first in case we fail to interpret it
		WActors[tmp.ChIndex].ActorClass = NULL;

		BYTE B=0; 
		tmp.SerializeBits (&B, 1);
		if (!B) {
			guard (ActorCheck);
			DWORD Index=0;
			tmp.SerializeInt (Index, UtvEngine->ServerPackageMap->GetMaxObjectIndex ());
			UObject* Object;
			Object = UtvEngine->ServerPackageMap->IndexToObject (Index, 1);
			AActor* InActor = Cast<AActor>( Object );			
			
			//Now we may have an actor if it was on the level
			if( InActor==NULL )
			{
				guard (Transient);
				// Transient actor.
				UClass* ActorClass = Cast<UClass>( Object );			
				if (ActorClass != NULL) {
					WActors[tmp.ChIndex].ActorClass = ActorClass;
		            
					FVector Location;
					FRotator Rotation(0,0,0);
					SerializeCompressedInitial( tmp, Location, Rotation, ActorClass->GetDefaultActor()->bNetInitialRotation, NULL );

				}
				else {
					debugf (TEXT ("Null actor in parse watcher properties"));
					return;
				}
				unguard;
			} 
			else {
				//A non-transient one, only interesting when tracking actors for watchers
				guard (NonTransient);
				WActors[tmp.ChIndex].ActorClass = InActor->GetClass();
				unguard;
				//return false;
			}
			unguard;
		}

		//Check if this is a class we want to do things with
		UClass* actor=WActors[tmp.ChIndex].ActorClass;
		if( ! (actor && (actor->IsChildOf(AReplicationInfo::StaticClass()) /*|| actor->IsChildOf(APlayerController::StaticClass()) */))){
			WActors[tmp.ChIndex].ActorClass = NULL;
			return;
		}

		//Initialize the properties array
		if (WActors[tmp.ChIndex].ActorClass != NULL) {
			
			guard (FullTrackInit);
			ClearActor (tmp.ChIndex);

			//And initalize a new list to null
			FClassNetCache* ClassCache = UtvEngine->ServerPackageMap->GetClassNetCache(WActors[tmp.ChIndex].ActorClass);
			for (int i = 0; i <= ClassCache->GetMaxIndex (); ++i) {
				WActors[tmp.ChIndex].Data.AddItem (NULL);
			}

			//debugf (TEXT ("Parse: started channel %i: %s"), tmp.ChIndex, WActors[tmp.ChIndex].ActorClass->GetName ());

			unguard;
		}
		unguard;
	}

	//Closing packet?
	if (tmp.bClose) {
		guard (CloseChannel);
		WActors[tmp.ChIndex].ActorClass = NULL;
		ClearActor (tmp.ChIndex);		

		//No need do do anything more with this bunch
		return;

		unguard;
	}

	//Now process properties if we have a valid actor
	if (WActors[tmp.ChIndex].ActorClass) {
		guard (BigParse);
		FClassNetCache* ClassCache = UtvEngine->ServerPackageMap->GetClassNetCache(WActors[tmp.ChIndex].ActorClass);
		INT             RepIndex   = tmp.ReadInt( ClassCache->GetMaxIndex() );
		FFieldNetCache* FieldCache = tmp.IsError() ? NULL : ClassCache->GetFromIndex( RepIndex );
		
		while( FieldCache ) {

			guard (FoundReplicationIndex);
			
			//Parse the data so we can check for new properties
			UProperty* It;			

			if( FieldCache && (It=FlagCast<UProperty,CLASS_IsAUProperty>(FieldCache->Field))!=NULL )
			{		
				guard (FoundProperty);
				//Is it an array property? get index if so
				BYTE Element=0;
				if( It->ArrayDim != 1 ){
					tmp << Element;
				}

				//Find offset and an area to put it
				INT Offset = Element * It->ElementSize;				
				BYTE* Data = WActors[tmp.ChIndex].Data(RepIndex);
				if (!Data) {
					Data = new BYTE[It->ElementSize * It->ArrayDim];
					appMemzero( Data, It->ElementSize * It->ArrayDim);		//suck, kraschar utan denna :)
					WActors[tmp.ChIndex].Data(RepIndex) = Data;
				}
				
				//Now save this value to our allocated place
				It->NetSerializeItem (tmp, UtvEngine->ServerPackageMap, Data + Offset);
				//debugf (TEXT ("Got property %s (%i)"), It->GetName (), It->ElementSize);

				unguard;
			} 
			else {
				//We only care about properties
				return;				
			}

			// Get next
			RepIndex   = tmp.ReadInt( ClassCache->GetMaxIndex() );
			FieldCache = tmp.IsError() ? NULL : ClassCache->GetFromIndex( RepIndex );

			unguard;
		}
		unguard;
	}
	
	unguard;
}

void BunchDelayer::AddBunch(FInBunch* Bunch)
{
	guard(BunchDelayer::AddBunch);

	WaitingBunch wb;
	InternalBunch* ib=new InternalBunch;
	ib->bunch=new FInBunch(*Bunch);
	wb.Bunch=ib;
	wb.OutTime=CurrentTime+DelayTime;
	wb.Important=ParseProperties (wb.Bunch);

	WaitingBunches.PushBack(wb);

	if(UtvEngine->PrimaryConnection){
		if(Bunch->bOpen){
			check(OpenChannels[Bunch->ChIndex]==0);
			OpenChannels[Bunch->ChIndex]=new OpenPackets;
//			INT a=OpenChannels[Bunch->ChIndex]->AddZeroed();
			new(*OpenChannels[Bunch->ChIndex]) InternalBunch(*ib);
			OpenChannel(UtvEngine->PrimaryConnection,Bunch->ChIndex);
		} else {
			check(OpenChannels[Bunch->ChIndex]!=0);
			if(OpenChannels[Bunch->ChIndex]->Num()<(INT)MaxChannelQueSize*(wb.Important?8:1) && Bunch->bReliable){
//				INT a=OpenChannels[Bunch->ChIndex]->AddZeroed();
				new(*OpenChannels[Bunch->ChIndex]) InternalBunch(*ib);
			}
		}
		if(Bunch->bClose){
			delete OpenChannels[Bunch->ChIndex];
			OpenChannels[Bunch->ChIndex]=0;
		}
		UtvEngine->SendBunch(UtvEngine->PrimaryConnection,wb.Bunch);
	}

	//mu
	//ParseProperties (wb.Bunch, false);
	unguard;
}

void BunchDelayer::NewConnection(UNetConnection* Connection)
{
	guard(BunchDelayer::NewConnection);

	guard (SJ-kod);
	//Create channels and send initialization packets
	for(int a=0;a<=UNetConnection::MAX_CHANNELS;++a){
		if(DelayedOpenChannels[a])
			OpenChannel(Connection,a);
	}
	for(int a=0;a<=UNetConnection::MAX_CHANNELS;++a){
		if(DelayedOpenChannels[a])
			UtvEngine->SendBunch(Connection,&((*DelayedOpenChannels[a])(0)));
	}
	for(int a=0;a<=UNetConnection::MAX_CHANNELS;++a){
		if(DelayedOpenChannels[a]){
		//	if (actors[di->first].worthTracking)
		//		debugf(TEXT("Channel %i has %i packets"),di->first,di->second.size());
			for(INT i=1;i<DelayedOpenChannels[a]->Num();++i)
				UtvEngine->SendBunch(Connection,&(*DelayedOpenChannels[a])(i));
		}
	} 
	unguard;

	guard (Fnord-kod);
	//Now send out all recorded properties that we are sitting on
	for (INT i = 0; i <= UNetConnection::MAX_CHANNELS; ++i) {

		if (WActors[i].ActorClass) {
			FClassNetCache* ClassCache = UtvEngine->ServerPackageMap->GetClassNetCache(WActors [i].ActorClass);

			//Create ourselves a bunch
			FOutBunch* out = new FOutBunch (Connection->Channels[i], false);
			out->bOpen = false;
			out->bClose = false;
			out->bReliable = true;
			BOOL written = false;

			//debugf (TEXT ("Sending replication for %i: %s"), i, WActors[i].ActorClass->GetName ());

			//Now go through all properties to see which have data to send
			for (INT j = 0; j < WActors[i].Data.Num (); ++j) {
			
				//Only send properties that have information
				BYTE* Data = WActors[i].Data(j);
				if (Data) {
					//Obtain the field cache
					FFieldNetCache* FieldCache = ClassCache->GetFromIndex (j);
					UProperty* It;

					//A regular property?
					if ((It=FlagCast<UProperty,CLASS_IsAUProperty>(FieldCache->Field))!=NULL) {
						//debugf (TEXT ("Property %i: %s"), j, It->GetName ());

						if (It->ArrayDim != 1) {
							DWORD Index = j;
							for (INT k = 0; k < It->ArrayDim; ++k) {
	
								//Only send array elements that are set to something
								if (!appMemIsZero (Data, It->ElementSize)) {
									BYTE el = k;
									out->SerializeInt (Index, ClassCache->GetMaxIndex ());
									*out << el;
									It->NetSerializeItem (*out, UtvEngine->ServerPackageMap, Data);								
									written = true;
								}
	
								Data += It->ElementSize; 
							} 
						}
						else {
							DWORD Index = j;
							out->SerializeInt (Index, ClassCache->GetMaxIndex ());
							It->NetSerializeItem (*out, UtvEngine->ServerPackageMap, Data);
							written = true;
						}

					}					
				}			

				if (out->GetNumBits() > 200) {
					UtvEngine->SendBunch (Connection, out);
					written = false;
					delete out;
					out = new FOutBunch (Connection->Channels[i], false);
					out->bOpen = false;
					out->bClose = false;
					out->bReliable = true;
				}
			}

			if (written) {
				UtvEngine->SendBunch (Connection, out);
			}
			delete out;
		}
	}
	unguard;

	//And create the fake actor
	UtvEngine->CreateFakeActor (Connection);

	unguard;
}

//Requires that the channel has been added to the correct openchannels map before this is called
void BunchDelayer::OpenChannel(UNetConnection* Connection,INT ChIndex,bool SendQuedBunches)
{
	OpenPackets* op;
	if(Connection==UtvEngine->PrimaryConnection)
		op=OpenChannels[ChIndex];
	else
		op=DelayedOpenChannels[ChIndex];

	if(!op){
//		if(Connection==UtvEngine->PrimaryConnection){
//			printf("Attempt to open channel without start packet %i\n",ChIndex);
//		}
//		new(UtvEngine->OpenConnections.Find(Connection)->ChannelHistory[ChIndex]) FString("Opening channel without start packet");
		return;
	}

	if(op){
		UtvEngine->OpenChannel(Connection,(*op)(0).bunch);
		if(SendQuedBunches){
			for(INT i=0;i<op->Num();++i)
				UtvEngine->SendBunch(Connection,&(*op)(i));
		}
	}
}

void BunchDelayer::OpenChannelOnClients(int ChIndex)
{
	for( INT i=0; i<UtvEngine->ListenDriver->ClientConnections.Num(); i++ ){
		UNetConnection* Connection=UtvEngine->ListenDriver->ClientConnections(i);
		if(UtvEngine->OpenConnections.Find(Connection)->isReady && Connection!=UtvEngine->PrimaryConnection){
			OpenChannel(Connection,ChIndex);
		}
	}
}

void BunchDelayer::Restart(void)
{
	while(!WaitingBunches.Empty()){
		delete WaitingBunches.Front().Bunch;
		WaitingBunches.PopFront();
	}
	
	for (INT i = 0; i <= UNetConnection::MAX_CHANNELS; ++i) {
		if(OpenChannels[i]!=0){
			delete OpenChannels[i];
			OpenChannels[i]=0;
		}
		if(DelayedOpenChannels[i]!=0){
			delete DelayedOpenChannels[i];
			DelayedOpenChannels[i]=0;
		}
	}

	ActorProps ap;
	ap.ActorClass=0;
	ap.worthTracking=false;
	//Reset all actor property data
	debugf (TEXT ("Resetting actor property list"));
	for (INT i = 0; i <= UNetConnection::MAX_CHANNELS; ++i) {
		ClearActor (i);
		WActors[i].ActorClass = NULL;
		actors[i]=ap;
	}
}

IMPLEMENT_CLASS(BunchDelayer);
