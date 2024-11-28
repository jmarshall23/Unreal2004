/*=============================================================================
	D3DRenderInterface.h: Unreal Direct3D rendering interface definition.
	Copyright 2001 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Andrew Scheidecker
=============================================================================*/

#ifndef HEADER_D3DRENDERINTERFACE
#define HEADER_D3DRENDERINTERFACE

///
//	FD3DModifierInfo
//
struct FD3DModifierInfo
{
	UBOOL				ModifyTextureTransforms;
	UBOOL				ModifyFramebufferBlending;
	UBOOL				ModifyColor;
	UBOOL				ModifyOpacity;

	// texture modifier
	FMatrix				Matrix;
	BYTE				TexCoordSource;
	BYTE				TexCoordCount;
	UBOOL				TexCoordProjected;

	// framebuffer blend modifier
    BYTE				FrameBufferBlending;
    UBOOL				ZWrite;
    UBOOL				ZTest;
    UBOOL				AlphaTest;
    UBOOL				TwoSided;
    BYTE				AlphaRef;

	// color modifier
	FColor				TFactorColor;
	UBOOL				AlphaBlend;

	// fallback info
	UMaterial*			BestFallbackPoint;

	// opacity modifier
	UMaterial*			Opacity;
	UBOOL				OpacityOverrideTexModifier;

	// Constructor
	FD3DModifierInfo();

	// FD3DModifierInfo interface
	void SetDetailTextureScale( FLOAT Scale );
};

///
//	FD3DMaterialStateStage
//
struct FD3DMaterialStateStage
{
	FD3DTexture*		Texture;
	D3DTEXTUREADDRESS	TextureAddressU,
						TextureAddressV,
						TextureAddressW;
    FLOAT               TextureMipLODBias; // sjs
	D3DTEXTUREOP 		ColorOp,
						AlphaOp;
	DWORD				ColorArg0,
						ColorArg1,
						ColorArg2,
						AlphaArg0,
						AlphaArg1,
						AlphaArg2,
						ResultArg;

	DWORD				TexCoordIndex;
	DWORD				TexCoordCount;
	UBOOL				TextureTransformsEnabled;
	D3DMATRIX			TextureTransformMatrix;

	FD3DMaterialStateStage();
};

///
//	FD3DMaterialState
//
class FD3DMaterialState
{
public:
	EPixelShader		PixelShader;
	UBOOL				AlphaBlending;			// Alpha blending is enabled
	UBOOL				AlphaTest;				// Alpha test is enabled?
	BYTE				AlphaRef;				// If alpha testing, the value to compare against
	UBOOL				ZTest;					// Test zbuffer
	UBOOL				ZWrite;					// Write to zbuffer
	UBOOL				TwoSided;
	EFillMode			FillMode;				// Wireframe, flatshaded or solid
	D3DCOLOR			TFactorColor;
	D3DBLEND			SrcBlend,
						DestBlend;
	UBOOL				OverrideFogColor;
	FColor				OverriddenFogColor;
	UBOOL				PatchLighting;

	INT					StagesUsed;
	FD3DMaterialStateStage	Stages[8];

	INT	NumRefs;

	FD3DMaterialState();
};

//
//	FD3DMaterialStatePool
//

class FD3DMaterialStatePool
{
public:

	TArray<FD3DMaterialState*>	FreeStates;

	// Destructor.

	~FD3DMaterialStatePool()
	{
		for(INT StateIndex = 0;StateIndex < FreeStates.Num();StateIndex++)
			delete FreeStates(StateIndex);
	}

	// AllocateState

	FD3DMaterialState* AllocateState(FD3DMaterialState* DefaultState)
	{
		FD3DMaterialState*	Result;

		if(FreeStates.Num())
			Result = FreeStates.Pop();
		else
			Result = new(TEXT("D3DMaterialState")) FD3DMaterialState();

		if(DefaultState)
			appMemcpy(Result,DefaultState,sizeof(FD3DMaterialState));

		Result->NumRefs = 1;

		return Result;
	}

	// FreeState

	void FreeState(FD3DMaterialState* State)
	{
		for( INT i=0; i<FreeStates.Num(); i++ )
			check( FreeStates(i) != State );
		FreeStates.AddItem(State);
	}
};

//
//	FD3DRenderInterface
//
class FD3DRenderInterface : public FRenderInterface
{
public:

	///
	//	FD3DSavedState
	//
	class FD3DSavedState
	{
	public:

		IDirect3DSurface8*	RenderTargetSurface;
		IDirect3DSurface8*	DepthStencilSurface;

		INT					ViewportX,
							ViewportY,
							ViewportWidth,
							ViewportHeight;

		INT					ZBias;

		ECompareFunction	StencilTest;
		EStencilOp			StencilFailOp,
							StencilZFailOp,
							StencilPassOp;
		DWORD				StencilRef,
							StencilMask,
							StencilWriteMask;

		FMatrix				LocalToWorld,
							WorldToCamera,
							CameraToScreen;

		FD3DVertexShader*	VertexShader;
		FD3DVertexStream*	Streams[16];
		INT					StreamStrides[16],
							NumStreams;

		FD3DIndexBuffer*	IndexBuffer;
		INT					IndexBufferBase;

		D3DCULL				CullMode;

		UBOOL				UseDetailTexturing;
		UBOOL				UseDynamicLighting;
		UBOOL				UseStaticLighting;
		UBOOL				LightingModulate2X;
		UBOOL				LightingOnly;
		FD3DTexture*		Lightmap;
		FSphere				LitSphere;
		FColor				AmbientLightColor;
		D3DLIGHT8			Lights[8];
		UBOOL				LightEnabled[8];

		UBOOL				DistanceFogEnabled;
		FLOAT				DistanceFogStart,
							DistanceFogEnd;
		FColor				DistanceFogColor;

		FLOAT				NPatchTesselation;

		FD3DMaterialState*	MaterialPasses[8];
		INT					NumMaterialPasses;
		FD3DMaterialState*	CurrentMaterialState;
	
		FD3DSavedState();
	};


	UD3DRenderDevice*		RenDev;
	UViewport*				Viewport;
#define MAX_STATESTACKDEPTH	128
	FD3DSavedState			SavedStates[MAX_STATESTACKDEPTH];
	FD3DSavedState*			CurrentState;
	FD3DMaterialState		DefaultPass;

	INT						SavedStateIndex;

	FD3DMaterialStatePool	MaterialStatePool;

	EPrecacheMode			PrecacheMode;

	// Hit stack.
	TArray<BYTE>			HitStack;
	BYTE*					HitData;
	INT*					HitSize;
	INT						HitCount;
	DWORD					HitPixels[HIT_SIZE][HIT_SIZE];

	// Constructor.
	FD3DRenderInterface(UD3DRenderDevice* InRenDev);

	// FRenderInterface interface.
	virtual void PushState();
	virtual void PopState();

	virtual UBOOL SetRenderTarget(FRenderTarget* RenderTarget);
	virtual void SetViewport(INT X,INT Y,INT Width,INT Height);
	virtual void Clear(UBOOL UseColor,FColor Color,UBOOL UseDepth,FLOAT Depth,UBOOL UseStencil,DWORD Stencil);

	virtual void PushHit(const BYTE* Data,INT Count);
	virtual void PopHit(INT Count,UBOOL Force);

	virtual void SetCullMode(ECullMode CullMode);

	virtual void SetAmbientLight(FColor Color);
	virtual void EnableLighting(UBOOL UseDynamic, UBOOL UseStatic, UBOOL Modulate2X, FBaseTexture* LightmapTexture, UBOOL LightingOnly, FSphere LitSphere );
	virtual void SetLight(INT LightIndex,FDynamicLight* Light,FLOAT Scale); // sjs
 
	virtual void SetNPatchTesselation( FLOAT Tesselation );
	virtual void SetDistanceFog(UBOOL Enable,FLOAT FogStart,FLOAT FogEnd,FColor Color);
	virtual void SetGlobalColor(FColor Color);
	
	virtual void SetTransform(ETransformType Type,const FMatrix& Matrix);

	virtual void SetMaterial(UMaterial* Material, FString* ErrorString, UMaterial** ErrorMaterial, INT* NumPasses);
	virtual void SetZBias(INT ZBias);
	virtual void SetStencilOp(ECompareFunction Test,DWORD Ref,DWORD Mask,EStencilOp FailOp,EStencilOp ZFailOp,EStencilOp PassOp,DWORD WriteMask);
	
	virtual void SetPrecacheMode( EPrecacheMode PrecacheMode );
	
	virtual INT  SetVertexStreams(EVertexShader Shader,FVertexStream** Streams,INT NumStreams);
	virtual INT  SetDynamicStream(EVertexShader Shader,FVertexStream* Stream);

	virtual INT  SetIndexBuffer(FIndexBuffer* IndexBuffer,INT BaseVertexIndex);
	virtual INT  SetDynamicIndexBuffer(FIndexBuffer* IndexBuffer,INT BaseVertexIndex);

	virtual void DrawPrimitive(EPrimitiveType PrimitiveType,INT FirstIndex,INT NumPrimitives,INT MinIndex,INT MaxIndex);

	// Helper function.
	virtual D3DSTENCILOP GetD3DStencilOp( EStencilOp StencilOp );
	virtual D3DCMPFUNC   GetD3DCompFunc( ECompareFunction CompFunc );

	// FD3DRenderInterface interface.
	void Locked( UViewport* InViewport, BYTE* InHitData,INT* InHitSize);
	void Unlocked();

    // sjs ---
	virtual int LockDynBuffer(BYTE** pOutBuffer, int numVerts, int stride, DWORD componentFlags);
	virtual int UnlockDynBuffer( void );
	virtual void DrawDynQuads(INT NumPrimitives);
	virtual void DrawQuads(INT FirstVertex, INT NumPrimitives);
    // --- sjs

private:
	FD3DTexture* CacheTexture(FBaseTexture* Texture);
	void SetMaterialBlending( FD3DMaterialState* NewMaterialState, D3DCULL CullMode );
	
	// Various material handlers
	UBOOL SetShaderMaterial( UShader* InShader, FD3DModifierInfo InModifierInfo, FString* ErrorString, UMaterial** ErrorMaterial );
	UBOOL SetSimpleMaterial( UMaterial* InMaterial, FD3DModifierInfo InModifierInfo, FString* ErrorString, UMaterial** ErrorMaterial );
	UBOOL SetParticleMaterial( UParticleMaterial* InParticleMaterial, FD3DModifierInfo InModifierInfo, FString* ErrorString, UMaterial** ErrorMaterial );
	UBOOL SetTerrainMaterial( UTerrainMaterial* InTerrainMaterial, FD3DModifierInfo InModifierInfo, FString* ErrorString, UMaterial** ErrorMaterial );
	UBOOL SetProjectorMaterial( UProjectorMaterial* ProjectorMaterial, FD3DModifierInfo InModifierInfo, FString* ErrorString, UMaterial** ErrorMaterial );
	void  SetLightingOnlyMaterial();

	// Helpers
	inline UBOOL HandleCombinedMaterial( UMaterial* InMaterial, INT& PassesUsed, INT& StagesUsed, INT& FreeStagesUsed, FD3DModifierInfo ModifierInfo, UBOOL InvertOutputAlpha=0, FString* ErrorString=NULL, UMaterial** ErrorMaterial=NULL );
	inline void  SetShaderBitmap( FD3DMaterialStateStage& Stage, UBitmapMaterial* BitmapMaterial );
	inline void  HandleOpacityBitmap( FD3DMaterialStateStage& Stage, UBitmapMaterial* Bitmap, UBOOL ModulateAlpha=0 );
	inline void  HandleVertexOpacity( FD3DMaterialStateStage& Stage, UVertexColor* VertexColor );
	inline void  HandleSpecular_SP( FD3DMaterialStateStage& Stage, UBitmapMaterial* Bitmap, UBOOL UseSpecularity, UBOOL UseConstantSpecularity, UBOOL ModulateSpecular2X ); // sjs
	inline void  HandleSelfIllumination_SP( FD3DMaterialStateStage& Stage, UBitmapMaterial* Bitmap );
	inline void  HandleLighting_MP( FD3DMaterialStateStage& Stage, FD3DTexture* Lightmap, UBOOL UseDiffuse );
	inline void  HandleLightmap_SP( FD3DMaterialStateStage& Stage, FD3DTexture* Lightmap );
	inline void  HandleDiffuse_Patch( FD3DMaterialStateStage& Stage, UBOOL Modulate2X = 0 );
	inline void  HandleDiffuse_SP( FD3DMaterialStateStage& Stage );
	inline void  HandleDiffuse_Stage( FD3DMaterialStateStage& Stage, UBOOL Modulate2X = 0 );
	inline void  HandleDetail( UBitmapMaterial* DetailBitmap, INT& PassesUsed, INT& StagesUsed, INT& FreeStagesUsed, FD3DModifierInfo InModifierInfo, UBOOL SinglePassOnly=0 );
	inline void	 HandleTFactor_SP( FD3DMaterialStateStage& Stage );
	inline void  ApplyTexModifier( FD3DMaterialStateStage& Stage, FD3DModifierInfo* ModifierInfo );
	inline void  ApplyFinalBlend( FD3DModifierInfo* InModifierInfo );

	FString DescribeStage(FD3DMaterialState* MaterialState,INT StageIndex,UBOOL Alpha);
	FString DescribeArg(FD3DMaterialState* MaterialState,INT StageIndex,DWORD Arg,UBOOL Alpha);
};


/*----------------------------------------------------------------------------
	CheckMaterial.
----------------------------------------------------------------------------*/

// Faster material type checking. It uses bit mask comparisons rather than 
// directly using Cast<>(). If the material does not have a valid render hint 
// set, it will use Cast<>() to resolve it. If you can guarantee that each 
// material type will have a bitmask set, you can remove the fallback for 
// improved performance.
template<class TYPE,DWORD MaterialType> FORCEINLINE TYPE* MaterialCast(UMaterial* Material)
{
	if (Material == NULL) return NULL;
	// First check the material type hint for a match. If that fails,
	// check to see if this is an unregistered type. If it is, use the
	// fallback Cast<>() method to determine type. If not just fail.
	return Material->MaterialType & MaterialType ? (TYPE*)Material :
	Material->MaterialType == 0 ? Cast<TYPE>(Material) : NULL;
}

//
// CHECKFALLBACK - return the Fallback material if it's decided we should use it.
//
#define CHECKFALLBACK( InMaterial )		\
			(UseFallbacks && InMaterial ? InMaterial->CheckFallback() : InMaterial)

//
// Attempt to cast a material/modifier chain to the appropriate class.
// If the chain ends with a material of the specfied class, remember any 
// modifier info along the way.
//
template<class C,DWORD MaterialType> C* CheckMaterial(FD3DRenderInterface* RI, UMaterial* InMaterial, FD3DModifierInfo* ModifierInfo=NULL, UBOOL UseFallbacks=0)
{
	C* Material = MaterialCast<C,MaterialType>(CHECKFALLBACK(InMaterial));
	if( Material )
		return Material;

	// See if we have a chain of Modifiers eventually pointing to material of class C
	UModifier* Modifier = MaterialCast<UModifier,MT_Modifier>(CHECKFALLBACK(InMaterial));
	while( Modifier )
	{
		Material = MaterialCast<C,MaterialType>(CHECKFALLBACK(Modifier->Material));
		Modifier = MaterialCast<UModifier,MT_Modifier>(CHECKFALLBACK(Modifier->Material));
	}

	// If we have a C, go through the Modifier list and combine the matrices for any TexModifiers.
	if( Material && ModifierInfo )
	{
		UBOOL NeedSource = 1;
		Modifier = MaterialCast<UModifier,MT_Modifier>(CHECKFALLBACK(InMaterial));
		while( Modifier )
		{
			// Remember the most specific fallback we see.
			if( Modifier->HasFallback() && UseFallbacks )
				ModifierInfo->BestFallbackPoint = Modifier;

			// Check for TexModifier.
			UTexModifier* TexModifier = MaterialCast<UTexModifier,MT_TexModifier>(Modifier);
			if( TexModifier )
			{
				FMatrix* TexMatrix = TexModifier->GetMatrix(RI->Viewport->Actor->Level->TimeSeconds);
				if( TexMatrix )
				{
					ModifierInfo->ModifyTextureTransforms = 1;
					ModifierInfo->Matrix *= *TexMatrix;
				}
				// Locate the first non-passthrough texture coordinate source.
				if( NeedSource && TexModifier->TexCoordSource!=TCS_NoChange )
				{
					ModifierInfo->ModifyTextureTransforms = 1;
					ModifierInfo->TexCoordSource	= TexModifier->TexCoordSource;
					ModifierInfo->TexCoordCount		= TexModifier->TexCoordCount;
					ModifierInfo->TexCoordProjected	= TexModifier->TexCoordProjected;
					NeedSource = 0;
				}
			}

			// Check for FinalBlend modifier.
			UFinalBlend* FinalBlend = MaterialCast<UFinalBlend,MT_FinalBlend>(Modifier);
			if( FinalBlend ) 	
			{
				ModifierInfo->ModifyFramebufferBlending = 1;

				ModifierInfo->FrameBufferBlending	= FinalBlend->FrameBufferBlending;
				ModifierInfo->ZWrite				= FinalBlend->ZWrite;
				ModifierInfo->ZTest					= FinalBlend->ZTest;
				ModifierInfo->AlphaTest				= FinalBlend->AlphaTest;
				ModifierInfo->TwoSided				= FinalBlend->TwoSided;
				ModifierInfo->AlphaRef				= FinalBlend->AlphaRef;
			}

			// Check for ColorModifier modifier.
			UColorModifier* ColorModifier = MaterialCast<UColorModifier,MT_ColorModifier>(Modifier);
			if( ColorModifier )
			{
				ModifierInfo->ModifyColor	= 1;
				
				ModifierInfo->AlphaBlend	|= ColorModifier->AlphaBlend;
				ModifierInfo->TwoSided		|= ColorModifier->RenderTwoSided;
				ModifierInfo->TFactorColor	= ColorModifier->Color;
			}

			// Check for an OpacityModifier modifier.
			UOpacityModifier* OpacityModifier = MaterialCast<UOpacityModifier,MT_OpacityModifier>(Modifier);
			if( OpacityModifier )
			{
				ModifierInfo->ModifyOpacity = 1;
				ModifierInfo->Opacity = OpacityModifier->Opacity;
				ModifierInfo->OpacityOverrideTexModifier = OpacityModifier->bOverrideTexModifier;			
			}

			// Move to the next modifier in the chain
			Modifier = MaterialCast<UModifier,MT_Modifier>(CHECKFALLBACK(Modifier->Material));
		}
	}

	return Material;
}


#endif
