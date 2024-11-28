/*=======================================s======================================
	XBoxAudioVoice.cpp: Unreal XBox Voice Chat interface object.
	Copyright 1999-2001 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.

	Basically just the VoiceChat example code + bugfixes and minor tweaks
=============================================================================*/

/*------------------------------------------------------------------------------------
	Audio includes.
------------------------------------------------------------------------------------*/

#include "XboxAudioPrivate.h"

#if USE_VOICE_CHAT

// !!! sjs - these are no defined by libs, remove
// Predefined voice masks        fSpecEnergyWeight           fPitchScale                 fWhisperValue               fRoboticValue
#define _XVOICE_MASK_NONE         XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED
#define _XVOICE_MASK_CARTOON      0.10f,                      XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED
#define _XVOICE_MASK_BIGGUY       0.90f,                      0.05f,                      XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED
#define _XVOICE_MASK_CHILD        0.10f,                      0.70f,                      XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED
#define _XVOICE_MASK_ROBOT        0.50f,                      XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED, 0.05f
#define _XVOICE_MASK_DARKMASTER   1.00f,                      0.00f,                      XVOICE_MASK_PARAM_DISABLED, XVOICE_MASK_PARAM_DISABLED

static XVOICE_MASK VoiceMaskPresets[] =
{
    { _XVOICE_MASK_NONE			},
    { _XVOICE_MASK_CARTOON		},
    { _XVOICE_MASK_BIGGUY		},
    { _XVOICE_MASK_CHILD			},
    { _XVOICE_MASK_ROBOT			},
    { _XVOICE_MASK_DARKMASTER	},
};
static const DWORD NUM_VOICEMASKPRESETS = ARRAY_COUNT(VoiceMaskPresets);



//-----------------------------------------------------------------------------
// File: VoiceCommunicator.cpp
//
// Desc: Implementation of Voice Communicator support
//
// Hist: 1.17.02 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: CVoiceManager (ctor)
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CVoiceManager::CVoiceManager( UXBoxAudioSubsystem* InXBoxAudio)
{
	XBoxAudio					= InXBoxAudio;
	m_dwMicrophoneState         = 0;
    m_dwHeadphoneState          = 0;
    m_dwConnectedCommunicators  = 0;
    m_bIsInChatSession          = FALSE;

    ZeroMemory( m_aChatters, sizeof( REMOTE_CHATTER ) * MAX_CHATTERS ); 
}




//-----------------------------------------------------------------------------
// Name: ~CVoiceManager (dtor)
// Desc: Verifies that object was shut down properly
//-----------------------------------------------------------------------------
CVoiceManager::~CVoiceManager()
{
    check( m_dwConnectedCommunicators == 0 );
}



//-----------------------------------------------------------------------------
// Name: AddChatter
// Desc: Adds a chatter to the list of remote chatters
// BUGBUG: Need to improve failure path
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::AddChatter( DWORD PlayerHandle, WORD wControllerPort )
{
    if( !IsInChatSession() )
        return S_FALSE;

    debugf(TEXT("Adding new chatter %d controller %d"), PlayerHandle, wControllerPort );

    if( ChatterIndexFromPlayerID( PlayerHandle, wControllerPort ) != MAX_CHATTERS ) // sjs - made this now fatal due to double-adds
    {
        debugf(TEXT("Chatter already in list, ignoring AddChatter for %d"), PlayerHandle);
        return S_OK;
    }

    check( ChatterIndexFromPlayerID( PlayerHandle, wControllerPort ) == MAX_CHATTERS );

    XVOICE_QUEUE_XMO_CONFIG VoiceQueueCfg = {0};
    VoiceQueueCfg.dwMsOfDataPerPacket     = 20;
    VoiceQueueCfg.dwCodecBufferSize       = COMPRESSED_PACKET_SIZE;

    DWORD WTF;
    REMOTE_CHATTER* pNewChatter = NULL;

    // Find a free slot
    for( WTF = 0; WTF < MAX_CHATTERS; WTF++ )
    {
        if( !m_aChatters[WTF].pVoiceQueueXMO )
        {
            pNewChatter = &m_aChatters[WTF];
            break;
        }
    }
    check( pNewChatter );

    WAVEFORMATEX wfx;
    wfx.cbSize = 0;
    wfx.nSamplesPerSec = VOICE_SAMPLE_RATE;
    wfx.nChannels = 1;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.wFormatTag = WAVE_FORMAT_PCM;

    // Send each voice to all 4 voice mixbins
    DSMIXBINVOLUMEPAIR dsmbvp[] = {
        { DSMIXBIN_FXSEND_0, 0 },
        { DSMIXBIN_FXSEND_1, 0 },
        { DSMIXBIN_FXSEND_2, 0 },
        { DSMIXBIN_FXSEND_3, 0 } };

    DWORD dwNumBins = 4;
    DSMIXBINS dsmb = { dwNumBins, dsmbvp };

    DSSTREAMDESC dssd = {0};
	appMemzero(&dssd,sizeof(DSSTREAMDESC));
    dssd.dwMaxAttachedPackets = NUM_VOICE_PACKETS;
    dssd.lpwfxFormat = &wfx;
    dssd.lpMixBins = &dsmb;
    
    if( FAILED( XVoiceQueueCreateMediaObject( &VoiceQueueCfg, &pNewChatter->pVoiceQueueXMO ) ) )
        return E_OUTOFMEMORY;
    
    if( FAILED( XVoiceCreateOneToOneDecoder( &pNewChatter->pDecoderXMO ) ) )
        return E_OUTOFMEMORY;

    if( FAILED( DirectSoundCreateStream( &dssd, &pNewChatter->pOutputStream ) ) )
        return E_OUTOFMEMORY;

    // Pause the stream until the assoicated queue has produced output.
    pNewChatter->pOutputStream->Pause( DSSTREAMPAUSE_PAUSE );
    pNewChatter->pOutputStream->SetHeadroom( 0 );

    pNewChatter->pbStreamBuffer = new BYTE[ VOICE_BUFFER_SIZE ];
    if( !pNewChatter->pbStreamBuffer )
        return E_OUTOFMEMORY;

    // Pre-buffer the stream with silence
    ZeroMemory( pNewChatter->adwStatus, NUM_VOICE_PACKETS * sizeof( DWORD ) );
    pNewChatter->dwCurrentPacket = 0;
    ZeroMemory( pNewChatter->pbStreamBuffer, VOICE_BUFFER_SIZE );
    for( DWORD i = 0; i < NUM_PREBUFFER_PACKETS; i++ )
    {
        XMEDIAPACKET xmp;
        GetStreamPacket( &xmp, WTF );
        SubmitStreamPacket( &xmp, WTF );
    }

    pNewChatter->PlayerHandle = PlayerHandle;
    pNewChatter->wControllerPort = wControllerPort;

    check( ChatterIndexFromPlayerID( PlayerHandle, wControllerPort ) < MAX_CHATTERS );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RemoveChatter
// Desc: Removes a chatter from the list of remote chatters
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::RemoveChatter( DWORD PlayerHandle, WORD wControllerPort )
{
    if( !IsInChatSession() )
        return S_FALSE;

    debugf(TEXT("Removing chatter %d controller %d"), PlayerHandle, wControllerPort );

    DWORD j;
    REMOTE_CHATTER* pChatterRemove = NULL;

    // Find the specified chatter
    for( j = 0; j < MAX_CHATTERS; j++ )
    {
        if( m_aChatters[j].PlayerHandle == PlayerHandle &&
            m_aChatters[j].wControllerPort == wControllerPort )
        {
            pChatterRemove = &m_aChatters[j];
            break;
        }
    }

	if( !pChatterRemove )
    {
        debugf(TEXT("Chatter not in list, ignoring RemoveChatter for %d"), PlayerHandle);
        return S_OK;
    }
	
    check( pChatterRemove );

    if( pChatterRemove->pVoiceQueueXMO )
        pChatterRemove->pVoiceQueueXMO->Release();
    if( pChatterRemove->pDecoderXMO )
        pChatterRemove->pDecoderXMO->Release();
    if( pChatterRemove->pOutputStream )
        pChatterRemove->pOutputStream->Release();
    if( pChatterRemove->pbStreamBuffer )
        delete[] pChatterRemove->pbStreamBuffer;

    ZeroMemory( pChatterRemove, sizeof( REMOTE_CHATTER ) );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SendLocalChatters
// Desc: Handles sending out notifications for our local chatters when we
//          join a session
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::SendLocalChatters()
{
    // We enable voice here, because this means
    // Server: We've created the session and are adding our chatters
    // Client: We're received a chatter list message and are about
    //          to process the list
    // Must do this BEFORE adding any chatters, because otherwise
    // AddChatters will bail.
    check( !IsInChatSession() );
    SetIsInChatSession( TRUE );

    return S_OK;
}


VOID CVoiceManager::UpdateVoiceMasks()
{
    for( WORD i = 0; i < XGetPortCount(); i++ )
    {
		if( IsCommunicatorInserted(i) )
		{
			//!!vogel: Set voice mask.
			DWORD VoiceMask = 0;
	
			UGameEngine* GameEngine = CastChecked<UGameEngine>( XBoxAudio->Viewport->Actor->GetLevel()->Engine );
			VoiceMask = appAtoi(GameEngine->LastURL.GetOption( TEXT("VoiceMask="), TEXT("2") ));
            VoiceMask = Clamp<DWORD>(VoiceMask, 0, 5 ); // sjs
			debugf(TEXT("VoiceMask set to: %d"), VoiceMask);

			m_aVoiceCommunicators[i].SetVoiceMask( VoiceMaskPresets[VoiceMask] );
		}
	}
}


//-----------------------------------------------------------------------------
// Name: CheckDeviceChanges
// Desc: Processes device changes to look for insertions and removals of
//          voice communicators.
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::CheckDeviceChanges()
{
    DWORD dwMicrophoneInsertions;
    DWORD dwMicrophoneRemovals;
    DWORD dwHeadphoneInsertions;
    DWORD dwHeadphoneRemovals;

    // Must call XGetDevice changes to track possible removal and insertion
    // in one frame
    XGetDeviceChanges( XDEVICE_TYPE_VOICE_MICROPHONE,
                       &dwMicrophoneInsertions,
                       &dwMicrophoneRemovals );
    XGetDeviceChanges( XDEVICE_TYPE_VOICE_HEADPHONE,
                       &dwHeadphoneInsertions,
                       &dwHeadphoneRemovals );

    // Update state for removals
    m_dwMicrophoneState &= ~( dwMicrophoneRemovals );
    m_dwHeadphoneState  &= ~( dwHeadphoneRemovals );

    // Then update state for new insertions
    m_dwMicrophoneState |= ( dwMicrophoneInsertions );
    m_dwHeadphoneState  |= ( dwHeadphoneInsertions );

    for( WORD i = 0; i < XGetPortCount(); i++ )
    {
        // If either the microphone or the headphone was
        // removed since last call, remove the communicator
        if( m_dwConnectedCommunicators & ( 1 << i ) &&
            ( ( dwMicrophoneRemovals   & ( 1 << i ) ) ||
              ( dwHeadphoneRemovals    & ( 1 << i ) ) ) )
        {
            m_dwConnectedCommunicators &= ~( 1 << i );
            m_aVoiceCommunicators[i].OnRemoval();
			check(XBoxAudio);
			check(XBoxAudio->Viewport);
			check(XBoxAudio->Viewport->Actor);
			check(XBoxAudio->LocalIpAddress);
			XBoxAudio->Viewport->Actor->eventServerChangeVoiceChatter( XBoxAudio->Viewport->Actor, XBoxAudio->LocalIpAddress, i, 0 );
			debugf(TEXT("DDD %s"), XBoxAudio->Viewport->Actor->GetPathName());
        }

        // If both microphone and headphone are present, and
        // we didn't have a communicator here last frame,
        // register the insertion
        if( ( m_dwMicrophoneState         & ( 1 << i ) ) &&
            ( m_dwHeadphoneState          & ( 1 << i ) ) &&
            !( m_dwConnectedCommunicators & ( 1 << i ) ) )
        {
            HRESULT hr = m_aVoiceCommunicators[i].OnInsertion( i );
            if( SUCCEEDED( hr ) )
            {
				//!!vogel: Set voice mask.
				DWORD VoiceMask = 0;
	
				UGameEngine* GameEngine = CastChecked<UGameEngine>( XBoxAudio->Viewport->Actor->GetLevel()->Engine );
				VoiceMask = appAtoi(GameEngine->LastURL.GetOption( TEXT("VoiceMask="), TEXT("2") ));

                VoiceMask = Clamp<DWORD>(VoiceMask, 0, 5 ); // sjs

                debugf(TEXT("VoiceMask set to: %d"), VoiceMask);

				m_aVoiceCommunicators[i].SetVoiceMask( VoiceMaskPresets[VoiceMask] );
                
				m_dwConnectedCommunicators |= ( 1 << i );
				check(XBoxAudio);
				check(XBoxAudio->Viewport);
				check(XBoxAudio->Viewport->Actor);
				check(XBoxAudio->LocalIpAddress);
				XBoxAudio->Viewport->Actor->eventServerChangeVoiceChatter( XBoxAudio->Viewport->Actor, XBoxAudio->LocalIpAddress, i, 1 );
            }
            else
            {
                debugf(TEXT("Insertion on port %d failed"), i );
                m_aVoiceCommunicators[i].OnRemoval();
            }
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: EnterChatSession
// Desc: Brings the box into the chat session
//-----------------------------------------------------------------------------
VOID CVoiceManager::EnterChatSession()
{
    if( IsInChatSession() )
        return;

    SendLocalChatters();
}



//-----------------------------------------------------------------------------
// Name: LeaveChatSession
// Desc: Leaves the chat session
//-----------------------------------------------------------------------------
VOID CVoiceManager::LeaveChatSession()
{
    // Remove all our remote chatters
    for( DWORD i = 0; i < MAX_CHATTERS; i++ )
    {
        if( m_aChatters[i].pVoiceQueueXMO )
            RemoveChatter( m_aChatters[i].PlayerHandle, m_aChatters[i].wControllerPort );
    }

    m_bIsInChatSession = FALSE;
}



//-----------------------------------------------------------------------------
// Name: ChatterIndexFromPlayerID
// Desc: Finds the index into m_aChatters for the given player ID.
//          Returns MAX_CHATTERS if not found
//-----------------------------------------------------------------------------
DWORD CVoiceManager::ChatterIndexFromPlayerID( DWORD PlayerHandle, WORD wControllerPort )
{
    for( DWORD i = 0; i < MAX_CHATTERS; i++ )
    {
        if( m_aChatters[i].PlayerHandle == PlayerHandle &&
            m_aChatters[i].wControllerPort == wControllerPort )
            return i;
    }

    return MAX_CHATTERS;
}



//-----------------------------------------------------------------------------
// Name: ToggleListenToChatter
// Desc: Modifies the chatter's stream's output mixbins so that the player 
//          on the specified port will or will not hear them
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ToggleListenToChatter( DWORD PlayerHandle, WORD wControllerPort, BOOL bMute, DWORD dwPort )
{
    check( IsInChatSession() );

    // Find the REMOTE_CHATTER struct
    DWORD chatterIndex = ChatterIndexFromPlayerID( PlayerHandle, wControllerPort );
    check( chatterIndex < MAX_CHATTERS );

    // See if we're already OK (ie, already remote-muted, etc.)
    if( m_aChatters[ chatterIndex ].bMuted[ dwPort ] == bMute )
        return S_OK;

    // Update flag saying if this player is listening or not
    m_aChatters[ chatterIndex ].bMuted[ dwPort ] = bMute;

    // Update mix bin outputs
    DSMIXBINVOLUMEPAIR dsmbvp[XGetPortCount()];
    for( DWORD i = 0; i < XGetPortCount(); i++ )
    {
        dsmbvp[i].dwMixBin = DSMIXBIN_FXSEND_0 + i;
        dsmbvp[i].lVolume  = m_aChatters[ chatterIndex ].bMuted[ i ] ? DSBVOLUME_MIN : DSBVOLUME_MAX;
    }

    DSMIXBINS dsmb = { XGetPortCount(), dsmbvp };
    m_aChatters[ chatterIndex ].pOutputStream->SetMixBins( &dsmb );

    return S_OK;
}






//-----------------------------------------------------------------------------
// Name: DoesPlayerHaveVoice
// Desc: Returns TRUE if the player has a voice communicator connected
//-----------------------------------------------------------------------------
BOOL CVoiceManager::DoesPlayerHaveVoice( DWORD PlayerHandle, WORD wControllerPort )
{
    DWORD chatterIndex = ChatterIndexFromPlayerID( PlayerHandle, wControllerPort );

    return( chatterIndex < MAX_CHATTERS );
}



//-----------------------------------------------------------------------------
// Name: IsPlayerTalking
// Desc: Returns TRUE if the player is currently talking
//-----------------------------------------------------------------------------
BOOL CVoiceManager::IsPlayerTalking( DWORD PlayerHandle, WORD wControllerPort )
{
    DWORD chatterIndex = ChatterIndexFromPlayerID( PlayerHandle, wControllerPort );
    check( chatterIndex < MAX_CHATTERS );

    return( m_aChatters[ chatterIndex ].bIsTalking );
}

//-----------------------------------------------------------------------------
// Name: BroadcastPacket
// Desc: Broadcasts a voice packet to everyone over the network
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::BroadcastPacket( VOID* pvData, INT nSize, WORD wControllerPort )
{
    if( !IsInChatSession() )
        return S_OK;

    check( nSize == COMPRESSED_PACKET_SIZE );
	//VOGEL:TODO
	XBoxAudio->SendVoiceData( (BYTE*)pvData, wControllerPort );
	//ReceivePacket( 1977, 0, pvData, nSize );
	//ReceivePacket( 1977, 1, pvData, nSize );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReceivePacket
// Desc: Handles receipt of a voice packet from the network
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ReceivePacket( DWORD PlayerHandle, WORD wControllerPort, VOID* pvData, INT nSize )
{
    if( !IsInChatSession() )
        return S_FALSE;

    if( nSize == 0 )
        return S_OK;

    for( DWORD j = 0; j < MAX_CHATTERS; j++ )
    {
        if( m_aChatters[j].PlayerHandle == PlayerHandle &&
            m_aChatters[j].wControllerPort == wControllerPort )
        {
            if( m_aChatters[j].pVoiceQueueXMO )
            {
                XMEDIAPACKET xmp = {0};

                // Send the packet to the queue
                xmp.pvBuffer          = pvData;
                xmp.dwMaxSize         = nSize;

                m_aChatters[j].pVoiceQueueXMO->Process( &xmp, NULL );
            }
            else
            {
                // This is probably ok as a user is getting added to a 
                // session, after they know about machines, but before they
                // know about players.
                // DumpMessage( "Voice", "Got packet from player, but no queue set up for them" );
            }
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTemporaryPacket
// Desc: Fills out an XMEDIAPACKET pointing to a temporary (compressed) buffer
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::GetTemporaryPacket( XMEDIAPACKET* pPacket, BOOL bCompressed )
{
    ZeroMemory( pPacket, sizeof( XMEDIAPACKET ) );

    if( bCompressed )
    {
        pPacket->pvBuffer   = m_pbTempEncodedPacket;
        pPacket->dwMaxSize  = COMPRESSED_PACKET_SIZE;
    }
    else
    {
        pPacket->pvBuffer   = m_pbTempDecodedPacket;
        pPacket->dwMaxSize  = VOICE_PACKET_SIZE;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: GetStreamPacket
// Desc: Fills out an XMEDIAPACKET pointing to the next stream packet for
//          the specified chatter
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::GetStreamPacket( XMEDIAPACKET* pPacket, DWORD dwChatterIndex )
{
    REMOTE_CHATTER* pChatter = &m_aChatters[ dwChatterIndex ];

    ZeroMemory( pPacket, sizeof( XMEDIAPACKET ) );

    pPacket->pvBuffer   = pChatter->pbStreamBuffer + pChatter->dwCurrentPacket * VOICE_PACKET_SIZE;
    pPacket->pdwStatus  = &pChatter->adwStatus[ pChatter->dwCurrentPacket ];
    pPacket->dwMaxSize  = VOICE_PACKET_SIZE;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SubmitStreamPacket
// Desc: Submits the XMEDIAPACKET for the specified chatter
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::SubmitStreamPacket( XMEDIAPACKET* pPacket, DWORD dwChatterIndex )
{
    REMOTE_CHATTER* pChatter = &m_aChatters[ dwChatterIndex ];
    HRESULT hr;

    hr = pChatter->pOutputStream->Process( pPacket, NULL );
    pChatter->dwCurrentPacket = ( pChatter->dwCurrentPacket + 1 ) % NUM_VOICE_PACKETS;

    return hr;
}



//-----------------------------------------------------------------------------
// Name: ProcessMicrophones
// Desc: Processes input from the microphones
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ProcessMicrophones()
{
#ifdef MONITOR_QUEUES
    static DWORD dwCycle = 15;
    dwCycle = ( dwCycle + 1 ) % 60;
#endif // MONITOR_QUEUES

    for( WORD i = 0; i < XGetPortCount(); i++ )
    {
        // If we've got an active communicator, process the mic
        if( m_dwConnectedCommunicators & ( 1 << i ) )
        {
#ifdef MONITOR_QUEUES
            DWORD dwNumPendingPackets = 0;
            for( DWORD j = 0; j < NUM_VOICE_PACKETS; j++ )
            {
                if( m_aVoiceCommunicators[i].m_adwMicrophonePacketStatus[j] == XMEDIAPACKET_STATUS_PENDING )
                    ++dwNumPendingPackets;
            }

            if( !dwNumPendingPackets )
                debugf(TEXT("Voice microphone is starving"));

            if( !dwCycle )
            {
                CHAR str[100];
                sprintf( str, "%d packets pending in microphone (port %d)", dwNumPendingPackets, i );
                debugf(TEXT("%s"), ANSI_TO_TCHAR(str) );
            }
#endif // MONITOR_QUEUES

            while( m_aVoiceCommunicators[i].MicrophonePacketStatus() != XMEDIAPACKET_STATUS_PENDING )
            {
                // Packet is done
                HRESULT hr;
                DWORD dwCompressedSize;
                XMEDIAPACKET xmpMicrophone;
                XMEDIAPACKET xmpCompressed;

                // Compress from microphone to temporary
                m_aVoiceCommunicators[i].GetMicrophonePacket( &xmpMicrophone );
                if( m_aVoiceCommunicators[i].MicrophonePacketStatus() == XMEDIAPACKET_STATUS_SUCCESS )
                {
                    GetTemporaryPacket( &xmpCompressed, TRUE );
                    xmpCompressed.pdwCompletedSize    = &dwCompressedSize;

                    hr = m_aVoiceCommunicators[i].m_pEncoderXMO->ProcessMultiple( 1, &xmpMicrophone, 1, &xmpCompressed );
                    check( SUCCEEDED( hr ) && 
                            ( dwCompressedSize == m_aVoiceCommunicators[i].m_dwEncodedPacketSize ||
                              dwCompressedSize == 0 ) );

                    // Nothing compressed, so nothing to decompress/playback
                    if( dwCompressedSize > 0 )
                    {
                        // sjs - what??? check( *(BYTE *)xmpCompressed.pvBuffer == 1 );
                        hr = BroadcastPacket( xmpCompressed.pvBuffer, dwCompressedSize, i );
                        check( SUCCEEDED( hr ) );
                    }
                }

                // Re-submit microphone packet to microphone XMO
                if( FAILED( m_aVoiceCommunicators[i].SubmitMicrophonePacket( &xmpMicrophone ) ) )
                    break;
            }
        }
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: ProcessQueues
// Desc: Processes output from the network queues
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ProcessQueues()
{
#ifdef MONITOR_QUEUES
    static DWORD dwCycle = 25;
    dwCycle = ( dwCycle + 1 ) % 60;
#endif // MONITOR_QUEUES

    BOOL bSomeoneHadOutput;
    BOOL bOkayToProcess;

    // Loop over all remote chatters we have, and see if everyone
    // who has provided output is ready to output
    do
    {
        bSomeoneHadOutput = FALSE;
        bOkayToProcess = TRUE;

        for( DWORD j = 0; j < MAX_CHATTERS; j++ )
        {
            if( m_aChatters[j].pVoiceQueueXMO && m_aChatters[j].bHasOutput )
            {
                DWORD dwQueueStatus;
                DWORD dwStreamStatus;
                m_aChatters[j].pVoiceQueueXMO->GetStatus( &dwQueueStatus );
                m_aChatters[j].pOutputStream->GetStatus( &dwStreamStatus );

                if( !( dwQueueStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA ) ||
                    !( dwStreamStatus & XMO_STATUSF_ACCEPT_INPUT_DATA ) )
                {
                    bOkayToProcess = FALSE;
                    break;
                }
            }
        }

        // Ok, it's time to get output
        if( bOkayToProcess )
        {
            for( DWORD j = 0; j < MAX_CHATTERS; j++ )
            {
                if( m_aChatters[j].pVoiceQueueXMO )
                {
#ifdef MONITOR_QUEUES
                    DWORD dwNumPendingPackets = 0;
                    for( DWORD i = 0; i < NUM_VOICE_PACKETS; i++ )
                    {
                        if( m_aChatters[j].adwStatus[i] == XMEDIAPACKET_STATUS_PENDING )
                            ++dwNumPendingPackets;
                    }

                    if( !dwCycle && m_aChatters[j].bHasOutput )
                    {
                        CHAR str[100];
                        sprintf( str, "%d packets pending in Chatter %d's stream", dwNumPendingPackets, j );
                        debugf(TEXT("%s"), ANSI_TO_TCHAR(str) );
                    }
#endif // MONITOR_QUEUES
                    // Check the queue for data...
                    HRESULT      hr;
                    XMEDIAPACKET xmpCompressed;
                    XMEDIAPACKET xmpStream = {0};
                    DWORD        dwFromQueue;

                    // Get a packet for the stream
                    GetStreamPacket( &xmpStream, j );
                    check( *xmpStream.pdwStatus != XMEDIAPACKET_STATUS_PENDING );

                    // Get a temporary compressed packet
                    GetTemporaryPacket( &xmpCompressed, TRUE );
                    xmpCompressed.pdwCompletedSize = &dwFromQueue;

                    // Get the compressed data from the queue
                    hr = m_aChatters[j].pVoiceQueueXMO->Process( NULL, &xmpCompressed );
                    if( SUCCEEDED( hr ) )
                    {
                        bSomeoneHadOutput = TRUE;
                        if( dwFromQueue > 0 )
                        {
                            // We actually got data
                            check( dwFromQueue == COMPRESSED_PACKET_SIZE );
                            m_aChatters[j].bHasOutput = TRUE;
                            m_aChatters[j].bIsTalking = TRUE;
                        
                            // Decode it into our per-chatter stream buffer
                            hr = m_aChatters[j].pDecoderXMO->ProcessMultiple( 1, &xmpCompressed, 1, &xmpStream );
                            check( SUCCEEDED( hr ) );
                        }
                        else
                        {
                            // No data, so create silence
                            m_aChatters[j].bIsTalking = FALSE;
                            ZeroMemory( xmpStream.pvBuffer, VOICE_PACKET_SIZE );
                        }

                        // Tell the stream to process
                        if( m_aChatters[j].bHasOutput )
                        {
                            m_aChatters[j].pOutputStream->Pause( DSSTREAMPAUSE_RESUME );
                            hr = SubmitStreamPacket( &xmpStream, j );
                            check( SUCCEEDED( hr ) );
                        }
                    }
                    else
                    {
                        // Only chatters who have never provided output
                        // should be able to fail.  In that case,
                        // bIsTalking should always already be FALSE
                        check( !m_aChatters[j].bHasOutput );
                        check( !m_aChatters[j].bIsTalking );
                    }
                }
            }
        }
    } while( bOkayToProcess && bSomeoneHadOutput );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: ProcessHeadphones
// Desc: Processes output to headphones
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ProcessHeadphones()
{
#ifdef MONITOR_QUEUES
    static DWORD dwCycle = 5;
    dwCycle = ( dwCycle + 1 ) % 60;
#endif // MONITOR_QUEUES

    // Ok, now try to feed headphones based off output from the SRC
    for( DWORD i = 0; i < XGetPortCount(); i++ )
    {
        // If we've got an active communicator, process the mic
        if( m_dwConnectedCommunicators & ( 1 << i ) )
        {
#ifdef MONITOR_QUEUES
            DWORD dwNumPendingPackets = 0;
            for( DWORD j = 0; j < NUM_VOICE_PACKETS; j++ )
            {
                if( m_aVoiceCommunicators[i].m_adwHeadphonePacketStatus[j] == XMEDIAPACKET_STATUS_PENDING )
                    ++dwNumPendingPackets;
            }

            if( !dwNumPendingPackets )
                debugf(TEXT("Voice headphone is starving"));

            if( !dwCycle )
            {
                CHAR str[100];
                sprintf( str, "%d packets pending in headphone (port %d)", dwNumPendingPackets, i );
                debugf(TEXT("%s"), ANSI_TO_TCHAR(str) );
            }
#endif // MONITOR_QUEUES

            static DWORD s_dwOldPosition[4];

            // Grab packets of audio data from the output of the Sample 
            // Rate Converter DSP effect.  Right now, the effect is outputting
            // 32 bit samples instead of 16, so we have to downconvert them.
            // In the future, the SRC effect will be able to output 16
            // bit samples, and this will no longer be necessary
            LPCDSFX_SAMPLE_RATE_CONVERTER_PARAMS pSrcParams = (LPCDSFX_SAMPLE_RATE_CONVERTER_PARAMS)XBoxAudio->DSPImage->aEffectMaps[Graph1_SRCforheadphone1+i].lpvStateSegment; 
            DWORD dwCircularBufferSize = XBoxAudio->DSPImage->aEffectMaps[Graph1_SRCforheadphone1+i].dwScratchSize;
            DWORD dwSamplesToStream = VOICE_PACKET_SIZE / sizeof( WORD );
            DWORD dwSrcPacketSize = VOICE_PACKET_SIZE * ( sizeof( DWORD ) / sizeof( WORD ) );
            DWORD dwCurrentPosition = pSrcParams->dwScratchSampleOffset;

#ifdef MONITOR_QUEUES
            DWORD dwPendingDataSize = ( dwCurrentPosition + dwCircularBufferSize - s_dwOldPosition[i] ) % dwCircularBufferSize;
            DWORD dwNumSRCPackets = dwPendingDataSize / dwSrcPacketSize;

            if( !dwCycle )
            {
                CHAR str[100];
                sprintf( str, "%d packets in SRC output (port %d)", dwNumSRCPackets, i );
                debugf(TEXT("%s"), ANSI_TO_TCHAR(str) );
            }
#endif // MONITOR_QUEUES
//            BOOL bReSync = FALSE;

            // If the headphone has space and there's a whole packet ready
            // (Note that these 2 should be in sync)
            while( m_aVoiceCommunicators[i].HeadphonePacketStatus() != XMEDIAPACKET_STATUS_PENDING &&
                   ( dwCurrentPosition + dwCircularBufferSize - s_dwOldPosition[i] ) % dwCircularBufferSize > dwSrcPacketSize )
            {
                // Create a silent buffer to mix into
                XMEDIAPACKET xmpHeadphone;
                m_aVoiceCommunicators[i].GetHeadphonePacket( &xmpHeadphone );

                // Convert from 32-bit to 16-bit samples as we copy into the headphone buffer
                PDWORD pSrcData = (DWORD *)( (BYTE *)(XBoxAudio->DSPImage->aEffectMaps[Graph1_SRCforheadphone1+i].lpvScratchSegment) + s_dwOldPosition[i] );
                PWORD  pDestData = (PWORD)xmpHeadphone.pvBuffer;
                for( DWORD dwSample = 0; dwSample < dwSamplesToStream; dwSample++ )
                    *pDestData++ = WORD( *pSrcData++ >> 16 );

                m_aVoiceCommunicators[i].SubmitHeadphonePacket( &xmpHeadphone );
                s_dwOldPosition[i] = ( s_dwOldPosition[i] + dwSrcPacketSize ) % dwCircularBufferSize;
            }
        }
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: ProcessVoice
// Desc: Processes input from the input controller devices.
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ProcessVoice()
{
    CheckDeviceChanges();

    if( m_bFlushQueuesOnNextProcess )
    {
        m_bFlushQueuesOnNextProcess = FALSE;
        FlushQueuesInternal();
    }

    ProcessMicrophones();

    ProcessQueues();

    ProcessHeadphones();

    DirectSoundDoWork();

#if _DEBUG
    ValidateStateDbg();
#endif // _DEBUG
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FlushQueuesInternal
// Desc: Iterates over each chatter, flushing their queue
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::FlushQueuesInternal()
{
    debugf(TEXT("Flushing all voice queues."));
    for( DWORD j = 0; j < MAX_CHATTERS; j++ )
    {
        if( m_aChatters[j].pVoiceQueueXMO )
        {
            m_aChatters[j].bHasOutput = FALSE;
            m_aChatters[j].bIsTalking = FALSE;
            m_aChatters[j].pVoiceQueueXMO->Flush();

            // Flush the stream and pre-buffer with silence
            m_aChatters[j].pOutputStream->Flush();
            m_aChatters[j].pOutputStream->Pause( DSSTREAMPAUSE_PAUSE );
            ZeroMemory( m_aChatters[j].pbStreamBuffer, VOICE_BUFFER_SIZE );
            ZeroMemory( m_aChatters[j].adwStatus, NUM_VOICE_PACKETS * sizeof( DWORD ) );
            m_aChatters[j].dwCurrentPacket = 0;
            for( DWORD i = 0; i < NUM_PREBUFFER_PACKETS; i++ )
            {
                XMEDIAPACKET xmp;
                GetStreamPacket( &xmp, j );
                SubmitStreamPacket( &xmp, j );
            }
        }
    }

    // Flush and pre-buffer each communicator's headphone and mic
    for( DWORD i = 0; i < XGetPortCount(); i++ )
    {
        if( m_dwConnectedCommunicators & ( 1 << i ) )
        {
            m_aVoiceCommunicators[i].m_pMicrophoneXMO->Flush();
            m_aVoiceCommunicators[i].m_dwNextMicrophonePacket = 0;
            for( DWORD j = 0; j < NUM_PREBUFFER_PACKETS; j++ )
            {
                XMEDIAPACKET xmp;
                m_aVoiceCommunicators[i].GetMicrophonePacket( &xmp );
                m_aVoiceCommunicators[i].SubmitMicrophonePacket( &xmp );
            }

            m_aVoiceCommunicators[i].m_pHeadphoneXMO->Flush();
            ZeroMemory( m_aVoiceCommunicators[i].m_pbHeadphoneBuffer, VOICE_BUFFER_SIZE );
            m_aVoiceCommunicators[i].m_dwNextHeadphonePacket = 0;

            for( DWORD j = 0; j < NUM_PREBUFFER_PACKETS; j++ )
            {
                XMEDIAPACKET xmp;
                m_aVoiceCommunicators[i].GetHeadphonePacket( &xmp );
                m_aVoiceCommunicators[i].SubmitHeadphonePacket( &xmp );
            }
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SendVoiceData
// Desc: Sends out a packet of voice data to all other players
//-----------------------------------------------------------------------------
#if 0
VOID CVoiceManager::SendVoiceData( BYTE* pbData, WORD wControllerPort )
{
    Message msgVoiceData( MSG_VOICEDATA );
    MsgVoiceData& msg = msgVoiceData.GetMsgVoiceData();

    msg.wControllerPort = wControllerPort;
    memcpy( msg.byData, pbData, COMPRESSED_PACKET_SIZE );
    for( PlayerList::iterator i = m_Players.begin(); i != m_Players.end(); ++i )
    {
        INT nBytes = m_DirectSock.SendTo( &msgVoiceData, msgVoiceData.GetSize(), CXBSockAddr( i->inAddr, DIRECT_PORT ).GetPtr() );
        assert( nBytes == msgVoiceData.GetSize() );
        (VOID)nBytes;
    }
}
#endif



#if _DEBUG
//-----------------------------------------------------------------------------
// Name: ValidateStateDbg
// Desc: Validates that the VoiceManager is in a consistent state
//-----------------------------------------------------------------------------
HRESULT CVoiceManager::ValidateStateDbg()
{
    // Only the bottom 4 bits should be used
    check( !(m_dwConnectedCommunicators & 0xFFFFFFF0 ) );
    check( !(m_dwMicrophoneState & 0xFFFFFFF0 ) );
    check( !(m_dwHeadphoneState & 0xFFFFFFF0 ) );

    // Check the state of the chatters list
    DWORD dwNumChatters = 0;
    for( DWORD i = 0; i < MAX_CHATTERS; i++ )
    {
        if( m_aChatters[i].pVoiceQueueXMO ) 
        {
            ++dwNumChatters;

            // Make sure they're not duplicated in the chatters list
            for( DWORD j = i + 1; j < MAX_CHATTERS; j++ )
            {
                if( m_aChatters[j].PlayerHandle == m_aChatters[i].PlayerHandle &&
                    m_aChatters[i].wControllerPort == m_aChatters[j].wControllerPort &&
                    m_aChatters[j].pVoiceQueueXMO )
                {
                    check( FALSE && "Duplicate player in chatters list!" );
                }
            }

            if( !m_aChatters[i].bHasOutput )
            {
                // If they haven't spoken, they can't be talking
                check( !m_aChatters[i].bIsTalking );

                DWORD dwStreamStatus;
                m_aChatters[i].pOutputStream->GetStatus( &dwStreamStatus );
                check( dwStreamStatus == ( DSSTREAMSTATUS_PAUSED | XMO_STATUSF_ACCEPT_INPUT_DATA ) );
            }
            else
            {

                DWORD dwStreamStatus;
                m_aChatters[i].pOutputStream->GetStatus( &dwStreamStatus );
                check( dwStreamStatus & ( DSSTREAMSTATUS_PLAYING | DSSTREAMSTATUS_STARVED ) );
            }
        }
    }

    check( IsInChatSession() || dwNumChatters == 0 );

    return S_OK;
}
#endif // _DEBUG



//-----------------------------------------------------------------------------
// Name: CVoiceCommunicator (ctor)
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CVoiceCommunicator::CVoiceCommunicator()
{
    m_pMicrophoneXMO    = NULL;
    m_pHeadphoneXMO     = NULL;
    m_pEncoderXMO       = NULL;

    m_lSlot             = INACTIVE_SLOT;
}




//-----------------------------------------------------------------------------
// Name: ~CVoiceCommunicator (dtor)
// Desc: Verifies that object was shut down properly
//-----------------------------------------------------------------------------
CVoiceCommunicator::~CVoiceCommunicator()
{
    check( m_pMicrophoneXMO == NULL &&
            m_pHeadphoneXMO  == NULL &&
            m_pEncoderXMO    == NULL );

    check( m_pbMicrophoneBuffer  == NULL &&
            m_pbHeadphoneBuffer   == NULL &&
            m_pbTempEncodedPacket == NULL );
}



HRESULT CVoiceCommunicator::SetVoiceMask( XVOICE_MASK VoiceMask )
{
	m_pEncoderXMO->SetVoiceMask( 0, &VoiceMask );
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: OnInsertion
// Desc: Handles insertion of a voice communicator:
//       1) Create the microphone and headphone devices
//       2) Create XMEDIAPACKET queues for both the microphone and headphone
//       3) Create voice encoders and decoders
//       4) Handle updating voice web
//-----------------------------------------------------------------------------
HRESULT CVoiceCommunicator::OnInsertion( DWORD dwSlot )
{
    debugf(TEXT("Communicator inserted into slot %d"), dwSlot );

    m_lSlot = LONG( dwSlot );

    // 1) Create the microphone and headphone devices
    WAVEFORMATEX wfx;
    wfx.cbSize = 0;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 8000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.wFormatTag = WAVE_FORMAT_PCM;

    HRESULT hr;
    hr = XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_MICROPHONE, 
                                  m_lSlot,
                                  NUM_VOICE_PACKETS,
                                  &wfx,
                                  &m_pMicrophoneXMO );
    if( FAILED( hr ) )
        return E_FAIL;

    hr = XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_HEADPHONE, 
                                  m_lSlot,
                                  NUM_VOICE_PACKETS,
                                  &wfx,
                                  &m_pHeadphoneXMO );
    if( FAILED( hr ) )
        return E_FAIL;


    // 2) Set up the XMEDIAPACKET queues.  Note, I'm assuming that
    // the XMOs must complete packets in order, otherwise I'll
    // need to keep track of order of submission myself.
    m_pbMicrophoneBuffer = new BYTE[ VOICE_BUFFER_SIZE ];
    m_dwNextMicrophonePacket = 0;
    for( DWORD i = 0; i < NUM_VOICE_PACKETS; i++ )
    {
        XMEDIAPACKET xmp;
        GetMicrophonePacket( &xmp );
        SubmitMicrophonePacket( &xmp );
    }

    // Now headphone
    m_pbHeadphoneBuffer = new BYTE[ VOICE_BUFFER_SIZE ];
    m_dwNextHeadphonePacket = 0;
    for( DWORD i = 0; i < NUM_VOICE_PACKETS; i++ )
        m_adwHeadphonePacketStatus[ i ] = XMEDIAPACKET_STATUS_SUCCESS;

    // Pre-buffer the headphone
    ZeroMemory( m_pbHeadphoneBuffer, VOICE_BUFFER_SIZE );
    for( DWORD i = 0; i < NUM_PREBUFFER_PACKETS; i++ )
    {
        XMEDIAPACKET xmp;
        GetHeadphonePacket( &xmp );
        SubmitHeadphonePacket( &xmp );
    }

    // 3) Create the encoder
    XVoiceCreateOneToOneEncoder( &m_pEncoderXMO );

    // Get the encoded packet size
    m_pEncoderXMO->GetCodecBufferSize( VOICE_PACKET_SIZE, &m_dwEncodedPacketSize );
    check( m_dwEncodedPacketSize == COMPRESSED_PACKET_SIZE );
    m_pbTempEncodedPacket = new BYTE[ m_dwEncodedPacketSize ];
    check( m_pbTempEncodedPacket );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: OnRemoval
// Desc: Handles removal of a voice communicator:
//       1) Notifies remote chatters that we're not here anymore
//       2) Frees all XMOs and buffers associated with the communicator
//-----------------------------------------------------------------------------
HRESULT CVoiceCommunicator::OnRemoval()
{
    debugf(TEXT("Communicator removed from slot %d"), m_lSlot );

    m_lSlot = INACTIVE_SLOT;

    if( m_pMicrophoneXMO )
    {
        m_pMicrophoneXMO->Release();
        m_pMicrophoneXMO = NULL;
    }
    if( m_pHeadphoneXMO )
    {
        m_pHeadphoneXMO->Release();
        m_pHeadphoneXMO = NULL;
    }
    if( m_pEncoderXMO )
    {
        m_pEncoderXMO->Release();
        m_pEncoderXMO = NULL;
    }

    delete[] m_pbMicrophoneBuffer;
    m_pbMicrophoneBuffer = NULL;
    delete[] m_pbHeadphoneBuffer;
    m_pbHeadphoneBuffer = NULL;
    delete[] m_pbTempEncodedPacket;
    m_pbTempEncodedPacket = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MicrophonePacketStatus
// Desc: Returns the status of the current microphone packet
//-----------------------------------------------------------------------------
DWORD CVoiceCommunicator::MicrophonePacketStatus()
{
    return m_adwMicrophonePacketStatus[ m_dwNextMicrophonePacket ];
}



//-----------------------------------------------------------------------------
// Name: GetMicrophonePacket
// Desc: Fills out an XMEDIAPACKET structure for the current microphone packet
//-----------------------------------------------------------------------------
HRESULT CVoiceCommunicator::GetMicrophonePacket( XMEDIAPACKET* pPacket )
{
    ZeroMemory( pPacket, sizeof( XMEDIAPACKET ) );
    pPacket->pvBuffer   = m_pbMicrophoneBuffer + m_dwNextMicrophonePacket * VOICE_PACKET_SIZE;
    pPacket->dwMaxSize  = VOICE_PACKET_SIZE;
    pPacket->pdwStatus  = &m_adwMicrophonePacketStatus[ m_dwNextMicrophonePacket ];

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SubmitMicrophonePacket
// Desc: Submits the XMEDIAPACKET to the microphone XMO
//-----------------------------------------------------------------------------
HRESULT CVoiceCommunicator::SubmitMicrophonePacket( XMEDIAPACKET* pPacket )
{
    HRESULT hr;

    hr = m_pMicrophoneXMO->Process( NULL, pPacket );

    m_dwNextMicrophonePacket = ( m_dwNextMicrophonePacket + 1 ) % NUM_VOICE_PACKETS;

    return hr;
}



//-----------------------------------------------------------------------------
// Name: HeadphonePacketStatus
// Desc: Returns the status of the current headphone packet
//-----------------------------------------------------------------------------
DWORD CVoiceCommunicator::HeadphonePacketStatus()
{
    return m_adwHeadphonePacketStatus[ m_dwNextHeadphonePacket ];
}



//-----------------------------------------------------------------------------
// Name: GetHeadphonePacket
// Desc: Fills out an XMEDIAPACKET structure for the current headphone packet
//-----------------------------------------------------------------------------
HRESULT CVoiceCommunicator::GetHeadphonePacket( XMEDIAPACKET* pPacket )
{
    ZeroMemory( pPacket, sizeof( XMEDIAPACKET ) );
    pPacket->pvBuffer   = m_pbHeadphoneBuffer + m_dwNextHeadphonePacket * VOICE_PACKET_SIZE;
    pPacket->dwMaxSize  = VOICE_PACKET_SIZE;
    pPacket->pdwStatus  = &m_adwHeadphonePacketStatus[ m_dwNextHeadphonePacket ];

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SubmitHeadphonePacket
// Desc: Submits the XMEDIAPACKET to the headphone XMO
//-----------------------------------------------------------------------------
HRESULT CVoiceCommunicator::SubmitHeadphonePacket( XMEDIAPACKET* pPacket )
{
    HRESULT hr;

    hr = m_pHeadphoneXMO->Process( pPacket, NULL );

    m_dwNextHeadphonePacket = ( m_dwNextHeadphonePacket + 1 ) % NUM_VOICE_PACKETS;

    return hr;
}


#endif
