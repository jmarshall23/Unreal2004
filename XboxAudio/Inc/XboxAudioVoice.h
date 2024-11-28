/*=============================================================================
	XBoxAudioVoice.h: Unreal XBox Voice Chat public header file.
	Copyright 1997-2001 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Daniel Vogel.

	Basically just the VoiceChat example code + bugfixes and minor tweaks
=============================================================================*/

#ifndef _INC_XBOXAUDIOVOICE
#define _INC_XBOXAUDIOVOICE

//-----------------------------------------------------------------------------
// File: VoiceCommunicator.h
//
// Desc: Class and strucuture definitions related to VoiceCommunicator support
//
// Hist: 1.17.02 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#pragma pack( push )
#pragma pack( 8 )

#include <xtl.h>
#include <xvoice.h>
#include <xonline.h>

#pragma pack( pop )

const DWORD COMMUNICATOR_COUNT = 4;
const LONG INACTIVE_SLOT = -1;
const DWORD NUM_PREBUFFER_PACKETS = 4;
const DWORD NUM_VOICE_PACKETS = 8;
const DWORD VOICE_SAMPLE_RATE = 8000;
const DWORD VOICE_PACKET_SIZE = 20 * 8000 * 2 / 1000;
const DWORD VOICE_BUFFER_SIZE = NUM_VOICE_PACKETS * VOICE_PACKET_SIZE;
const DWORD COMPRESSED_PACKET_SIZE = 10;
const DWORD MAX_CHATTERS = 16;  // 4 boxes * 4 controllers per box

enum ACTION
{
    NO_ACTION , 
    ADD_PORT , 
    DELETE_PORT , 
    ADD_AND_RESPOND
};


// Pack to minimize network traffic
#pragma pack( push )
#pragma pack( 1 )

struct MsgVoiceData
{
    WORD wControllerPort;
    BYTE byData[COMPRESSED_PACKET_SIZE];
};
#pragma pack( pop )


struct REMOTE_CHATTER
{
    DWORD				PlayerHandle;
    WORD                wControllerPort;
    XMediaObject*       pVoiceQueueXMO;
    LPXVOICEDECODER     pDecoderXMO;
    LPDIRECTSOUNDSTREAM pOutputStream;
    BOOL                bHasOutput;
    BOOL                bIsTalking;

    BYTE*               pbStreamBuffer;
    DWORD               adwStatus[NUM_VOICE_PACKETS];
    DWORD               dwCurrentPacket;
    BOOL                bMuted[XGetPortCount()]; // cache of who's listening
};

class CVoiceManager;

class CVoiceCommunicator
{
public:
    friend CVoiceManager;

    CVoiceCommunicator();
    ~CVoiceCommunicator();

    HRESULT OnInsertion( DWORD dwSlot );
    HRESULT OnRemoval();

    DWORD   MicrophonePacketStatus();
    HRESULT GetMicrophonePacket( XMEDIAPACKET* pPacket );
    HRESULT SubmitMicrophonePacket( XMEDIAPACKET* pPacket );

    DWORD   HeadphonePacketStatus();
    HRESULT GetHeadphonePacket( XMEDIAPACKET* pPacket );
    HRESULT SubmitHeadphonePacket( XMEDIAPACKET* pPacket );

	HRESULT SetVoiceMask( XVOICE_MASK );

private:
    LONG            m_lSlot;
    XMediaObject*   m_pMicrophoneXMO;
    XMediaObject*   m_pHeadphoneXMO;
	LPXVOICEENCODER m_pEncoderXMO;
    LPXVOICEDECODER m_pDecoderXMO;

    DWORD           m_dwEncodedPacketSize;
    BYTE*           m_pbTempEncodedPacket;

    DWORD           m_adwMicrophonePacketStatus[NUM_VOICE_PACKETS];
    DWORD           m_dwNextMicrophonePacket;
    BYTE*           m_pbMicrophoneBuffer;

    DWORD           m_adwHeadphonePacketStatus[NUM_VOICE_PACKETS];
    DWORD           m_dwNextHeadphonePacket;
    BYTE*           m_pbHeadphoneBuffer;
};

class CVoiceManager
{
public:
    CVoiceManager( class UXBoxAudioSubsystem* XBoxAudio );
    ~CVoiceManager();

    HRESULT ProcessVoice();
    HRESULT ReceivePacket( DWORD PlayerHandle, WORD wControllerPort, VOID* pvData, INT nSize );

    //////////////////////////////////////////////////////////////////////////
    // Functions for managing the chatter list
    //////////////////////////////////////////////////////////////////////////
    VOID    SetIsInChatSession( BOOL bIsInChatSession ) { m_bIsInChatSession = bIsInChatSession; }
    BOOL    IsInChatSession() { return m_bIsInChatSession; }
    HRESULT AddChatter( DWORD PlayerHandle, WORD wControllerPort );
    HRESULT RemoveChatter( DWORD PlayerHandle, WORD wControllerPort );
    HRESULT SendLocalChatters();

    VOID    EnterChatSession();
    VOID    LeaveChatSession();

	VOID	UpdateVoiceMasks();

    //////////////////////////////////////////////////////////////////////////
    // Utility functions for non-voice code
    //////////////////////////////////////////////////////////////////////////
    BOOL    IsCommunicatorInserted( WORD wControllerPort ) { return m_dwConnectedCommunicators & ( 1 << wControllerPort ); }
    BOOL    DoesPlayerHaveVoice( DWORD PlayerHandle, WORD wControllerPort );
    BOOL    IsPlayerTalking( DWORD PlayerHandle, WORD wControllerPort );
    HRESULT FlushQueues() { m_bFlushQueuesOnNextProcess = TRUE; return S_OK; }

private:
    HRESULT CheckDeviceChanges();
    HRESULT ProcessMicrophones();
    HRESULT ProcessQueues();
    HRESULT ProcessHeadphones();
    HRESULT BroadcastPacket( VOID* pvData, INT nSize, WORD wControllerPort );

    HRESULT FlushQueuesInternal();
    HRESULT GetTemporaryPacket( XMEDIAPACKET* pPacket, BOOL bCompressed );
    HRESULT GetStreamPacket( XMEDIAPACKET* pPacket, DWORD dwChatterIndex );
    HRESULT SubmitStreamPacket( XMEDIAPACKET* pPacket, DWORD dwChatterIndex );
    

    DWORD   ChatterIndexFromPlayerID( DWORD PlayerHandle, WORD wControllerPort );
    HRESULT ToggleListenToChatter( DWORD PlayerHandle, WORD wControllerPort, BOOL bMute, DWORD dwPort );

    // TODO - write accessor functions for the data we need out of these guys
    REMOTE_CHATTER m_aChatters[MAX_CHATTERS];
    BOOL    m_bIsInChatSession;

    DWORD   m_dwConnectedCommunicators;
    DWORD   m_dwMicrophoneState;
    DWORD   m_dwHeadphoneState;
    CVoiceCommunicator  m_aVoiceCommunicators[XGetPortCount()];

    BOOL    m_bFlushQueuesOnNextProcess;

    BYTE    m_pbTempEncodedPacket[COMPRESSED_PACKET_SIZE];
    BYTE    m_pbTempDecodedPacket[VOICE_PACKET_SIZE];

	UXBoxAudioSubsystem* XBoxAudio;

#if _DEBUG
    HRESULT ValidateStateDbg();
#endif // _DEBUG
};



typedef enum _DSP_IMAGE_DSPImage_FX_INDICES {
    Graph0_ZeroTempMixbins0 = 0,
    Graph1_SRCforheadphone1 = 1,
    Graph2_SRCforheadphone2 = 2,
    Graph3_SRCforheadphone3 = 3,
    Graph4_SRCforheadphone4 = 4,
    Graph5_XTalk = 5,
    Graph5_XTalk2LFE_A = 6,
    Graph5_XTalk2LFE_B = 7
} DSP_IMAGE_DSPImage_FX_INDICES;

typedef struct _Graph0_FX0_ZeroTempMixbins0_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[8];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph0_FX0_ZeroTempMixbins0_STATE, *LPGraph0_FX0_ZeroTempMixbins0_STATE;

typedef const Graph0_FX0_ZeroTempMixbins0_STATE *LPCGraph0_FX0_ZeroTempMixbins0_STATE;

typedef struct _Graph1_FX0_SRCforheadphone1_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph1_FX0_SRCforheadphone1_STATE, *LPGraph1_FX0_SRCforheadphone1_STATE;

typedef const Graph1_FX0_SRCforheadphone1_STATE *LPCGraph1_FX0_SRCforheadphone1_STATE;

typedef struct _Graph2_FX0_SRCforheadphone2_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph2_FX0_SRCforheadphone2_STATE, *LPGraph2_FX0_SRCforheadphone2_STATE;

typedef const Graph2_FX0_SRCforheadphone2_STATE *LPCGraph2_FX0_SRCforheadphone2_STATE;

typedef struct _Graph3_FX0_SRCforheadphone3_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph3_FX0_SRCforheadphone3_STATE, *LPGraph3_FX0_SRCforheadphone3_STATE;

typedef const Graph3_FX0_SRCforheadphone3_STATE *LPCGraph3_FX0_SRCforheadphone3_STATE;

typedef struct _Graph4_FX0_SRCforheadphone4_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph4_FX0_SRCforheadphone4_STATE, *LPGraph4_FX0_SRCforheadphone4_STATE;

typedef const Graph4_FX0_SRCforheadphone4_STATE *LPCGraph4_FX0_SRCforheadphone4_STATE;

typedef struct _Graph5_FX0_XTalk_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[4];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[4];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph5_FX0_XTalk_STATE, *LPGraph5_FX0_XTalk_STATE;

typedef const Graph5_FX0_XTalk_STATE *LPCGraph5_FX0_XTalk_STATE;

typedef struct _Graph5_FX1_XTalk2LFE_A_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph5_FX1_XTalk2LFE_A_STATE, *LPGraph5_FX1_XTalk2LFE_A_STATE;

typedef const Graph5_FX1_XTalk2LFE_A_STATE *LPCGraph5_FX1_XTalk2LFE_A_STATE;

typedef struct _Graph5_FX2_XTalk2LFE_B_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Graph5_FX2_XTalk2LFE_B_STATE, *LPGraph5_FX2_XTalk2LFE_B_STATE;

typedef const Graph5_FX2_XTalk2LFE_B_STATE *LPCGraph5_FX2_XTalk2LFE_B_STATE;


#endif
