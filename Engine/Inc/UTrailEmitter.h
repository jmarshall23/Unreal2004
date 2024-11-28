/*=============================================================================
	UTrailEmitter.h.
	Copyright 2001 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	virtual void Initialize( INT InMaxParticles );
	virtual void CleanUp();
	virtual void PostEditChange();

	virtual inline FColor SetInitialPointColor(FColor ParticleColor);

	virtual void UpdateParticle( FLOAT DeltaTime, INT Index );
	virtual INT  UpdateParticles( FLOAT DeltaTime );
	virtual void SpawnParticle( INT Index, FLOAT SpawnTime, INT Flags = 0, INT SpawnFlags = 0, const FVector& LocalLocationOffset = FVector(0,0,0) );
	virtual INT  RenderParticles( FDynamicActor* DynActor, class FLevelSceneNode* SceneNode, TList<FDynamicLight*>* Lights, class FRenderInterface* RI );

	void		 ResetTrail();

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

