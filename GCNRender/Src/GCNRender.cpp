/*=============================================================================
	GCNRender.cpp: GCN render device implementation.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

// Playstation2 includes.
#ifdef __GCN__

#include <demo.h>
#include "Engine.h"


UBOOL HideTerrain=0;
UBOOL HideStaticMesh=0;
UBOOL HideMesh=0;
UBOOL HideSkeletalMesh=0;
UBOOL HideFans=0;
UBOOL HideTiles=0;
UBOOL HideLines=1;

INT NUMVERTS=0;

struct FTplHeader //64 Bytes
{
	DWORD Version;
	DWORD NumTextures;
	DWORD TextureOffset;
	DWORD ImageOffset;
	DWORD CLUTOffset;
	_WORD Height;
	_WORD Width;
	DWORD Format;
	DWORD DataOffset;
	DWORD WrapS;
	DWORD WrapT;
	DWORD MinFilter;
	DWORD MagFilter;
	DWORD LODBias;
	BYTE  LODEdge;
	BYTE  LODMin;
	BYTE  LODMax;
	BYTE  Pad;
	BYTE  Pad2[8];

#ifdef WIN32
	// Byte Swap on emulator only
	void EndianSwap32(void* buffer) {
		u32 output;
		((u8*)&output)[0] = ((u8*)buffer)[3];
		((u8*)&output)[1] = ((u8*)buffer)[2];
		((u8*)&output)[2] = ((u8*)buffer)[1];
		((u8*)&output)[3] = ((u8*)buffer)[0];
		*((u32*)buffer) = output;
	}
	void EndianSwap16(void* buffer) {
		u16 output;
		((u8*)&output)[0] = ((u8*)buffer)[1];
		((u8*)&output)[1] = ((u8*)buffer)[0];
		*((u16*)buffer) = output;
	}

	void ByteSwap(void)
	{
		EndianSwap32(&Version);
		EndianSwap32(&NumTextures);
		EndianSwap32(&TextureOffset);
		EndianSwap32(&ImageOffset);
		EndianSwap32(&CLUTOffset);
		EndianSwap16(&Width);
		EndianSwap16(&Height);
		EndianSwap32(&Format);
		EndianSwap32(&DataOffset);
		EndianSwap32(&WrapS);
		EndianSwap32(&WrapT);
		EndianSwap32(&MinFilter);
		EndianSwap32(&MagFilter);
		EndianSwap32(&LODBias);
	};
#endif
};


// Low-level Playstation2 renderer.
struct FGCNRenderInterface : public FRenderInterface
{
	// Variables.
	INT					ResX, ResY;
	INT					SurfTime, PolyTime, TileTime, ThrashTime, VU1Time, VU1Chunks;
	INT					MatrixTime, BrushTransformTime, BrushRenderTime, LandscapeTransformTime, LandscapeRenderTime, FlushDMATime;
	INT					SetupTime, RenderTime;
	DWORD				Surfs, Polys, Tiles, Thrashes;		

	FMatrix				SavedLocalToWorld;
	FLOAT				OldFOV;
	UBOOL				RemapRedModulatedToGreen;
	FLOAT				FogStart;
	FLOAT				FogEnd;
	FLOAT				FogScale;
	UBOOL				NewVertexBuffer;
	UViewport*			LockedViewport;
	FLOAT MinU;
	FLOAT MinV;
	INT					LightMask;
	FRenderLightInfo*	ActiveLights[8];
	GXLightObj			GCNLights[8];
	FColor				AmbientColor;
	UBOOL				HardwareLighting;
	UBOOL				FirstStaticMeshRenderOfFrame;
	UBOOL				FirstBSPRenderOfFrame;
	UBOOL				FirstTerrainRenderOfFrame;
	UBOOL				FirstEmitterRenderOfFrame;
	INT					WhichCode;
	INT					SavedPolyFlags;
	FMatrix				SavedTextureMatrix[4];
	ETexCoordSource		SavedTextureTransformType[4];
	ECullMode			SavedCullMode;
	FIndexBuffer*		SavedIndexBuffer;
	INT					SavedBaseIndex;
	FVertexStream*		SavedVertexStreams[16];
	FMatrix				SavedWorldToCamera;
	FMatrix				SavedCameraToScreen;

	TMap<QWORD,GXTexObj*> TexCache;

/*
	GS::CMemArea*		AreaFB1;
	GS::CMemArea*		AreaFB2;
	GS::CMemArea*		AreaZB;
*/

	UBOOL SetRes(UViewport* Viewport, INT NewX, INT NewY, UBOOL Fullscreen)
	{
		guard(UGCNRenderDevice::SetRes);
		// Hardcoded resolution.
		NewX            = ResX = 640;
		NewY            = ResY = 448;
		
		Viewport->SizeX = ResX;
		Viewport->SizeY = ResY;
//SLUW		Viewport->OffX  = 0;
//SLUW		Viewport->OffY  = 0;


		return 1;
		unguard;
	}

	// URenderDevice interface.
	UBOOL Init(void)
	{
		guard(UGCNRenderDevice::Init);
		debugf(TEXT("Initializing Gamecube rendering [DOUBLE SIZE %d, %d]"), sizeof(DOUBLE), sizeof(double));

		INT l;
		for (l=0; l<ARRAY_COUNT(ActiveLights); l++)
			ActiveLights[l] = NULL;

		LightMask = 0;

		return 1;
		unguard;
	}
	void Flush(UViewport* Viewport, UBOOL AllowPrecache)
	{
	}

	void Exit(UViewport* Viewport)
	{
		guard(UGCNRenderDevice::Exit);
		
		// SL TODO: Free all resources, incdluing GS:: resources
		unguard;
	}
	UBOOL Exec(const TCHAR* Cmd,FOutputDevice& Ar)
	{
		if( ParseCommand( &Cmd, TEXT("PS2Toggle") ) )
		{
			if (appStrstr(Cmd, TEXT("Terrain")))
				HideTerrain ^= 1;
			else if (appStrstr(Cmd, TEXT("StaticMesh")))
				HideStaticMesh ^= 1;
			else if (appStrstr(Cmd, TEXT("Skeletal")))
				HideSkeletalMesh ^= 1;
			else if (appStrstr(Cmd, TEXT("Fans")))
				HideFans ^= 1;
			else if (appStrstr(Cmd, TEXT("Tiles")))
				HideTiles ^= 1;
			return 1;
		}
		return 0;
	}
	virtual void Clear(UBOOL UseColor,FColor Color,UBOOL UseDepth,FLOAT Depth)
	{
	}
	void UpdateGamma( UViewport* Viewport )
	{
		guard(UD3DRenderDevice::UpdateGamma);
		FLOAT Gamma		 = Viewport->GetOuterUClient()->Gamma;
		FLOAT Brightness = Viewport->GetOuterUClient()->Brightness;
		FLOAT Contrast   = Viewport->GetOuterUClient()->Contrast;
		// SL TODO???
		unguard;
	}

	void RestoreGamma()
	{
		// SL TODO???
	}
	void Lock(UViewport* Viewport, BYTE* HitData, INT* HitSize)
	{
		guard(UGCNRenderDevice::Lock);

		if (LockedViewport == NULL)
			this->SetRes(Viewport, Viewport->SizeX, Viewport->SizeY, 1);
		LockedViewport = Viewport;

		SurfTime=PolyTime=TileTime=ThrashTime=0;VU1Time=0;VU1Chunks=0;
		Surfs=Polys=Tiles=Thrashes=0;
		MatrixTime=BrushTransformTime=BrushRenderTime=LandscapeTransformTime=LandscapeRenderTime=FlushDMATime=0;
		SetupTime=RenderTime=0;

		static int initted = 0;
		if (initted == 0)
		{
			initted = 1;

			// use Unreal style culling
			GXSetCullMode(GX_CULL_FRONT);

			// floating point
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
			// floating point (possible? might need to be fixed point
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
			// floating point
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
			// Color 0 has 4 components (r, g, b, a), each component is 8b.
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

			// Initialize lighting, texgen, and tev parameters
			GXSetNumChans(1); // default, color = vertex color

			GXColor black = {0, 0, 0, 0};
			GXSetCopyClear(black, 0x00FFFFFF);


//			GXSetChanCtrl(
//				GX_COLOR0,
//				HardwareLighting ? GX_ENABLE : GX_DISABLE,     // enable channel
//				GX_SRC_REG,    // amb source
//				GX_SRC_VTX,    // mat source
//				0,     // light mask
//				GX_DF_CLAMP,   // diffuse function
//				GX_AF_SPEC);

			// disable alpha lighting (???)
//			GXSetChanCtrl(
//				GX_ALPHA0,
//				GX_DISABLE,    // disable channel
//				GX_SRC_REG,    // amb source
//				GX_SRC_REG,    // mat source
//				GX_LIGHT_NULL, // light mask
//				GX_DF_NONE,    // diffuse function
//				GX_AF_NONE);

		}
		DEMOBeforeRender();

		// SL TODO: DO we need to clear the screen at all?
		Clear(1, FColor(0, 0, 0), 0, 0);

		// Calculate the once per frame things
		FirstStaticMeshRenderOfFrame = true;
		FirstBSPRenderOfFrame = true;
		FirstTerrainRenderOfFrame = true;
		FirstEmitterRenderOfFrame = true;
//debugf("--");
		unguard;
	}
	void Unlock(UBOOL Blit)
	{
		guard(UGCNRenderDevice::Unlock);
		if(Blit)
		{
//TCHAR buf[1024];
//this->GetStats(buf);
//debugf(buf);
	        DEMODoneRender();
//AnimTick(v);        // Update animation.
		}
		else
		{
		}
		unguard;
	}
	virtual void SetCullMode(ECullMode CullMode)
	{
//		if (CullMode == CM_CW)
//			GXSetCullMode(GX_CULL_FRONT);
//		else if (CullMode == CM_CCW)
//			GXSetCullMode(GX_CULL_BACK);
//		else
			GXSetCullMode(GX_CULL_NONE);
	}
	void UpdateChanCtrl(void)
	{
//		GXSetChanCtrl(
//			GX_COLOR0,
//			HardwareLighting ? GX_ENABLE : GX_DISABLE,     // enable channel
//			GX_SRC_REG,    // amb source
//			GX_SRC_REG,    // mat source
//			LightMask,     // light mask
//			GX_DF_CLAMP,   // diffuse function
//			GX_AF_SPEC);
	}
	virtual void SetAmbientLight(FColor Color)
	{
		GXSetChanAmbColor(GX_COLOR0A0, (GXColor&)Color);
	}
	virtual void EnableLighting(UBOOL Enable)
	{
		HardwareLighting = Enable;

		this->UpdateChanCtrl();
	}
	virtual void SetLight(INT LightIndex,FRenderLightInfo* LightInfo)
	{
		if (LightIndex > 7)
			return;

		if (LightInfo == NULL)
			LightMask &= ~(1 << LightIndex);
		else
		{
			LightMask |=  (1 << LightIndex);

			GXInitLightColor(&GCNLights[LightIndex], (GXColor&)LightInfo->Diffuse);
			GXInitLightAttnK(&GCNLights[LightIndex], LightInfo->QuadraticAttenuation, LightInfo->LinearAttenuation, LightInfo->ConstantAttenuation);
		}
		this->UpdateChanCtrl();

		ActiveLights[LightIndex] = LightInfo;
	}

	virtual void SetDistanceFog(UBOOL Enable,FLOAT InFogStart,FLOAT InFogEnd,FColor FogColor)
	{
		if (Enable)
		{
			FogStart = InFogStart;
			FogEnd = InFogEnd;
			FogScale = 255.0f / (FogEnd - FogStart);
			//SL TODO
		}
		else
		{
			FogScale = 0;
		}
	}

	virtual void SetTransform(ETransformType Type,FMatrix Matrix)
	{
		if (Type == TT_LocalToWorld)
		{
			SavedLocalToWorld = Matrix.Transpose();
		}
		if (Type == TT_WorldToCamera)
		{
			// flip the Z to negate it
			// SL TODO: Do the copy manually, transposing/negating it along the way
			Matrix.M[0][2] = -Matrix.M[0][2];
			Matrix.M[1][2] = -Matrix.M[1][2];
			Matrix.M[2][2] = -Matrix.M[2][2];
			Matrix.M[3][2] = -Matrix.M[3][2];
			SavedWorldToCamera = Matrix.Transpose();
		}
		if (Type == TT_CameraToScreen)
		{
			// [0][0] is the type, perspective of ortho
			Mtx44 m;
			if (Matrix.M[0][0] == 0)
			{
				MTXPerspective(m, Matrix.M[0][1] * 448.0f/640.0f, 640.0f/448.0f, Matrix.M[0][2], Matrix.M[0][3]);
				GXSetProjection(m, GX_PERSPECTIVE);
			}
			else
			{
				MTXOrtho(m, 0, 448, 0, 640, 1.0f, 65535.0f);
				GXSetProjection(m, GX_ORTHOGRAPHIC);
			}
		}
	}

	virtual void SetBlending(DWORD PolyFlags)
	{
return;
		if (PolyFlags & PF_Masked)
		{
			// Modulate
			DEMOSetTevOp(GX_TEVSTAGE0, GX_MODULATE);

			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);

			GXSetAlphaCompare(GX_GREATER, 128, GX_AOP_AND, GX_ALWAYS, 0);
		}
		else if (PolyFlags & PF_Translucent)
		{
			GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
//		    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_TEXC, GX_CC_TEXC, GX_CC_TEXA);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
//		    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_RASA, GX_CA_RASA, GX_CA_RASA, GX_CA_TEXA);
//			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, 0, GX_TEVPREV);
            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
			GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
		}
		else
		{
			// Pass texture color (no vertex lighting)
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC );
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE,	GX_TEVPREV);
			GXSetTevOp(GX_TEVSTAGE0, GX_DECAL);
//            GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		}
	}

	virtual void SetZBias(INT ZBias)
	{
		// SL TODO: This function
	}

	virtual void SetTexture(INT TextureIndex,FBaseTexture* Texture)
	{
		if (Texture == NULL)
			return;
		FTexture* info = Texture->GetTextureInterface();
		if (info == NULL)
		{
			debugf(TEXT("Unknown texture type! - possibly a lightmap right now!"));
			return;
		}
if (info->GetUTexture() == NULL)
{
//debugf("Skipping Lightmap: %d", TextureIndex);
return;
}
		// we know the texture format is compressed, but we could check it in the tpl data
		// Mip[0] has the raw .tpl data

		GXTexObj* Tex = TexCache.FindRef(Texture->GetCacheId());
		if (Tex == NULL)
		{
			Tex = new GXTexObj;

			// Parse the .tpl data
			FTplHeader Hdr = *(FTplHeader*)info->GetRawTextureData(0);
#ifdef WIN32
			Hdr.ByteSwap();
#endif
			debugf(TEXT("Initializing texture: %d x %d x %d"), Hdr.Width, Hdr.Height, Hdr.LODMax);
			GXInitTexObj(Tex, ((BYTE*)(info->GetRawTextureData(0))) + Hdr.DataOffset, Hdr.Width, Hdr.Height, 
				(GXTexFmt)Hdr.Format, (GXTexWrapMode)Hdr.WrapS, (GXTexWrapMode)Hdr.WrapT, Hdr.LODMin != Hdr.LODMax);

			GXInitTexObjLOD(Tex, (GXTexFilter)Hdr.MinFilter, (GXTexFilter)Hdr.MagFilter, Hdr.LODMin, 
				Hdr.LODMax, Hdr.LODBias, GX_DISABLE, Hdr.LODEdge, GX_ANISO_1);

			TexCache.Set(Texture->GetCacheId(), Tex);
		}

		GXLoadTexObj(Tex, (GXTexMapID)(GX_TEXMAP0 + TextureIndex));
		return;
		//*GS_BGCOLOR=0;//black
	}

	virtual void SetTextureTransform(INT TextureIndex,FMatrix Transform,ETexCoordSource Source,UBOOL Projected)
	{
		// SL TODO: Use SourceUV
		//			Support TextureTransforms in all parts of DrawIndexedPrimitive

		SavedTextureTransformType[TextureIndex] = Source;
		SavedTextureMatrix[TextureIndex] = Transform;
	}

	virtual void SetVertexStreams(EVertexShader Shader,FVertexStream** Streams,INT NumStreams)
	{
//		debugf("Setting %d normal streams", NumStreams);
		// SL TODO: This function
		if (NumStreams > 16)
			appErrorf(TEXT("Too many vertex streams!"));
		for (int i=0; i<NumStreams; i++)
			SavedVertexStreams[i] = Streams[i];
	}

	virtual INT SetDynamicStream(EVertexShader Shader,FVertexStream* Stream)
	{
//		debugf("Setting a dynamic stream");
		// SL TODO: This function
		SavedVertexStreams[0] = Stream;
		return 0;
	}

	virtual void SetIndexBuffer(FIndexBuffer* IndexBuffer,INT BaseIndex)
	{
//		debugf("Setting an index buffer");
		SavedBaseIndex = BaseIndex;
		SavedIndexBuffer = IndexBuffer;
	}

	virtual INT SetDynamicIndexBuffer(FIndexBuffer* IndexBuffer,INT BaseIndex)
	{
//		debugf("Setting a dynamic index buffer");
		SavedBaseIndex = BaseIndex;
		SavedIndexBuffer = IndexBuffer;
		return 0;
	}

	virtual void DrawPrimitive(EPrimitiveType PrimitiveType,INT FirstIndex,INT NumPrimitives,INT MinIndex,INT MaxIndex)
	{
		INT VertexSize = SavedVertexStreams[0]->GetStride();
		INT PositionOffset=-1, ColorOffset=-1, UVOffset[4]={-1,-1,-1,-1}, NormalOffset=-1;
		INT CurrentOffset = 0;

		GXClearVtxDesc();

		TArray<FVertexComponent> Comps = SavedVertexStreams[0]->GetComponents();
		for (int c=0; c<Comps.Num(); c++)
		{
			FVertexComponent& Comp = Comps(c);
			if (Comp.Function == FVF_Position) 			{ PositionOffset = CurrentOffset;	GXSetVtxDesc(GX_VA_POS, GX_DIRECT); }
			else if (Comp.Function == FVF_Normal)		{ NormalOffset = CurrentOffset;		GXSetVtxDesc(GX_VA_NRM, GX_DIRECT); }
			else if (Comp.Function == FVF_Diffuse) 		{ ColorOffset = CurrentOffset;		GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT); }
			else if (Comp.Function == FVF_Specular)		{ ColorOffset = CurrentOffset;		GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT); }
			else if (Comp.Function == FVF_TexCoord0)	{ UVOffset[0] = CurrentOffset;		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT); }
//			else if (Comp.Function == FVF_TexCoord1)	{ UVOffset[1] = CurrentOffset;		GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT); }
			else if (Comp.Function == FVF_TexCoord2)	{ UVOffset[2] = CurrentOffset;		GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT); }
			else if (Comp.Function == FVF_TexCoord3)	{ UVOffset[3] = CurrentOffset;		GXSetVtxDesc(GX_VA_TEX3, GX_DIRECT); }

			if (Comp.Type == CT_Float4) CurrentOffset += 16;
			else if (Comp.Type == CT_Float3) CurrentOffset += 12;
			else if (Comp.Type == CT_Float2) CurrentOffset += 8;
			else if (Comp.Type == CT_Float1 || Comp.Type == CT_Color) CurrentOffset += 4;
		}

		INT NumVertices = NumPrimitives + 2; // Valid for fans and strips
		if (PrimitiveType == PT_TriangleList)
			NumVertices = NumPrimitives * 3;
		else if (PrimitiveType == PT_LineList)
			NumVertices = NumPrimitives * 2;

		char* VB = new char[SavedVertexStreams[0]->GetSize()];
		SavedVertexStreams[0]->GetStreamData(VB);

		_WORD* IB = NULL;
		if (SavedIndexBuffer)
		{
			IB = new _WORD[SavedIndexBuffer->GetSize()];
			SavedIndexBuffer->GetContents(IB);
		}

		// Setup up matrices
		MtxPtr local = (MtxPtr)&(SavedWorldToCamera.M[0][0]);
		MtxPtr world = (MtxPtr)&(SavedLocalToWorld.M[0][0]);
		Mtx out;
		MTXConcat(local, world, out);
		GXLoadPosMtxImm(out, GX_PNMTX0);

		// Setup the lights
//		for (INT i=0; i<8; i++)
//		{
//			if (LightMask & (1<<i))
//			{
//				Vec LightPos;
//				MTXMultVec(local, (VecPtr)&ActiveLights[i]->Origin, &LightPos);
//				GXInitLightPos(&GCNLights[i], LightPos.x, LightPos.y, LightPos.z);
//		        GXLoadLightObjImm(&GCNLights[i], GXLightID(1<<i));
//			}
//		}

		// Render!
		INT I;
		if (PrimitiveType == PT_TriangleFan)
		{
if (HideFans) goto out;
			GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, NumVertices);
		}
		else if (PrimitiveType == PT_TriangleList)
		{
if (HideTiles) goto out;
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, NumVertices);
		}
		else if (PrimitiveType == PT_TriangleStrip)
		{
if (HideStaticMesh) goto out;
			GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, NumVertices);
		}
		else if (PrimitiveType == PT_LineList)
		{
if (HideLines) goto out;
			GXBegin(GX_LINES, GX_VTXFMT0, NumVertices);
		}
		else 
		{
			goto out;
		}
		for (I=0; I<NumVertices; I++)
		{
			INT Index = FirstIndex + I;
			if (IB) Index = IB[Index];

			FVector* VertPos = ((FVector*)(&VB[VertexSize * Index + PositionOffset]));
			GXPosition3f32(VertPos->X, VertPos->Y, VertPos->Z);

			if (NormalOffset >= 0)
			{
				FVector* Normal = ((FVector*)(&VB[VertexSize * Index + NormalOffset]));
				GXNormal3f32(Normal->X, Normal->Y, Normal->Z);
			}

			if (ColorOffset >= 0)
			{
				FColor* Color = ((FColor*)(&VB[VertexSize * Index + ColorOffset]));
				GXColor4u8(Color->R, Color->G, Color->B, Color->A);
			}

			if (UVOffset[0] >= 0)
			{
				FLOAT* UV0 = ((FLOAT*)(&VB[VertexSize * Index + UVOffset[0]]));
				GXTexCoord2f32(UV0[0], UV0[1]);
			}

			if (UVOffset[1] >= 0)
			{
				FLOAT* UV1 = ((FLOAT*)(&VB[VertexSize * Index + UVOffset[1]]));
				GXTexCoord2f32(UV1[0], UV1[1]);
			}
		}
		GXEnd();
out:
		delete VB;
		delete IB;
	}


	void ClearZ(FSceneNode* Frame)
	{
		guard(UGCNRenderDevice::ClearZ);
		Clear(0, FColor(0,0,0), 1, 0);
		unguard;
	}
	void ReadPixels(UViewport* Viewport, FColor* Pixels)
	{
		for( INT y=0;y<ResY;y++ )
		{
			for( INT x=0;x<ResX;x++ )
			{
//				_WORD d=dest(x+y*ResX);
//				Pixels[x+y*ResX] = FColor(((d>>10)&0x1f)<<3, ((d>>5)&0x1f)<<3, (d&0x1f)<<3);
			}
		}
	}
	void GetStats( TCHAR* Result )
	{
		guard(GCN:GetStats);
//		appSprintf
//		(
//			Result,
//			TEXT("S=%03i %04.1f P=%03i %04.1f T=%03i %04.1f X=%03i %04.1f, VU1=%04.1f CKS=%02d"),
//			Surfs,
//			GSecondsPerCycle * 1000.f * SurfTime,
//			Polys,
//			GSecondsPerCycle * 1000.f * PolyTime,
//			Tiles,
//			GSecondsPerCycle * 1000.f * TileTime,
//			Thrashes,
//			GSecondsPerCycle * 1000.f * ThrashTime,
//			GSecondsPerCycle * 1000.f * VU1Time,
//			VU1Chunks
//		);		
//		appSprintf
//		(
//			Result,
//			TEXT("M=%04.1f BT=%04.1f BR=%04.1f LT=%04.1f LR=%04.1f FD=%04.1f"),
//			GSecondsPerCycle * 1000.f * MatrixTime,
//			GSecondsPerCycle * 1000.f * BrushTransformTime,
//			GSecondsPerCycle * 1000.f * BrushRenderTime,
//			GSecondsPerCycle * 1000.f * LandscapeTransformTime,
//			GSecondsPerCycle * 1000.f * LandscapeRenderTime,
//			GSecondsPerCycle * 1000.f * FlushDMATime
//		);
/*		appSprintf
		(
			Result,
			TEXT("Tex=%.2f S1=%.2f S2=%.2f S3=%.2f S4=%.2f Code=%.2f Ren=%.2f Syn=%.2f Exit=%.2f\n")
			TEXT("Get=%.2f Up=%.2f Pal=%.2f Set=%.2f Tgs=%.2f Lmp=%.2f D1=%.2f D2=%.2f D3=%.2f"),
			GSecondsPerCycle * 1000.f * RSM_Texture,
			GSecondsPerCycle * 1000.f * RSM_Setup1,
			GSecondsPerCycle * 1000.f * RSM_Setup2,
			GSecondsPerCycle * 1000.f * RSM_Setup3,
			GSecondsPerCycle * 1000.f * RSM_Setup4,
			GSecondsPerCycle * 1000.f * RSM_Code,
			GSecondsPerCycle * 1000.f * RSM_Render,
			GSecondsPerCycle * 1000.f * RSM_Sync,
			GSecondsPerCycle * 1000.f * RSM_Exit,
			GSecondsPerCycle * 1000.f * ST_GetTexture,
			GSecondsPerCycle * 1000.f * ST_Upload,
			GSecondsPerCycle * 1000.f * ST_GetPalette,
			GSecondsPerCycle * 1000.f * ST_SetPalette,
			GSecondsPerCycle * 1000.f * ST_Tags,
			GSecondsPerCycle * 1000.f * ST_Lightmap,
			GSecondsPerCycle * 1000.f * ST_DMA1,
			GSecondsPerCycle * 1000.f * ST_DMA2,
			GSecondsPerCycle * 1000.f * ST_DMA3
		);
*/
//debugf(Result);
		unguard;
	}

	// Unimplemented functions.
	FMatrix PushedMatrix1, PushedMatrix2, PushedMatrix3;
	virtual void PushState()
	{
		PushedMatrix1 = SavedLocalToWorld;
		PushedMatrix2 = SavedWorldToCamera;
		PushedMatrix3 = SavedCameraToScreen;
	}
	virtual void PopState()
	{
		SavedLocalToWorld = PushedMatrix1;
		SavedWorldToCamera = PushedMatrix2;
		SavedCameraToScreen = PushedMatrix3;
	}

	virtual void PushHit(const BYTE* Data,INT Count) {}
	virtual void PopHit(INT Count,UBOOL Force) {}

	void Draw3DLine(FSceneNode* Frame,FPlane Color,DWORD LineFlags,FVector OrigP,FVector OrigQ) {}
	void Draw2DClippedLine(FSceneNode* Frame,FPlane Color,DWORD LineFlags,FVector P1,FVector P2) {}
	void Draw2DLine(FSceneNode* Frame,FPlane Color,DWORD LineFlags,FVector P1,FVector P2) {}
	void Draw2DPoint(FSceneNode* Frame,FPlane Color,DWORD LineFlags,FLOAT X1,FLOAT Y1,FLOAT X2,FLOAT Y2,FLOAT Z) {}
};
FGCNRenderInterface* GCNRenderInterface;

class UGCNRenderDevice : public URenderDevice
{
	DECLARE_CLASS(UGCNRenderDevice,URenderDevice,CLASS_Config,GCNDrv)

	// URenderDevice interface.
	virtual UBOOL Init()
	{
		HardwareSkinning = 1;

		GCNRenderInterface = new FGCNRenderInterface;
		return GCNRenderInterface->Init();
	}

	virtual void Exit(UViewport* Viewport)
	{
		GCNRenderInterface->Exit(Viewport);
		delete GCNRenderInterface;
	}

	virtual UBOOL SetRes(UViewport* Viewport,INT NewX,INT NewY,UBOOL Fullscreen)
	{
		return GCNRenderInterface->SetRes(Viewport, NewX, NewY, Fullscreen);
	}

	virtual void Flush(UViewport* Viewport)
	{
	}
	virtual void UpdateGamma(UViewport* Viewport)
	{
	}
	virtual void RestoreGamma()
	{
	}

	virtual UBOOL Exec(const TCHAR* Cmd,FOutputDevice& Ar)
	{
		return GCNRenderInterface->Exec(Cmd, Ar);
	}

	virtual FRenderInterface* Lock(UViewport* Viewport,BYTE* HitData,INT* HitSize)
	{
		GCNRenderInterface->Lock(Viewport, HitData, HitSize);
		return GCNRenderInterface;
	}
	virtual void Unlock(FRenderInterface* RI,UBOOL Blit)
	{
		GCNRenderInterface->Unlock(Blit);
	}

	virtual void ReadPixels(UViewport* Viewport,FColor* Pixels)
	{
		GCNRenderInterface->ReadPixels(Viewport, Pixels);
	}

	virtual FString GetStats()
	{
		TCHAR Results[1024];
		GCNRenderInterface->GetStats(Results);
		return FString(Results);
	}
};

IMPLEMENT_CLASS(UGCNRenderDevice);


void InitGCNRenderPackage() {
	UGCNRenderDevice::StaticClass();
}

#endif

