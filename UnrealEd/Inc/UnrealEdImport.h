/*=============================================================================
	UnrealEdImport.h:
	Interface to UnrealEdDLL.dll viewed from unEditor.mll	
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.	

	Revision history:
		* Created by Michael Arnold,  Secret Level.
=============================================================================*/

//
// Description: 
//
// 
// Functions in interface:
//
//	UNREALED_API INT	LaunchEditor( HINSTANCE hInInstance, char* InCmdLine, INT nCmdShow );
//		Used to bring up the editor from external caller application (maya)
//
//	UNREALED_API void	PollEditor(void);
//		Used in idle time of external caller application (maya) to allow UnrealEdDLL.dll to work its magic
//
//	UNREALED_API INT	ShutDownEditor(void);
//		Used to (not entirely successfully) shut down the editor.
//
//	UNREALED_API bool	IsLaunched(void);
//		Indicates that LaunchEditor has been called - currently remains true when shut down
//
//	UNREALED_API bool	IsShutdown(void);
//		Indicates that ShutDownEditor has been called 
//
//	
//	UNREALED_API void LoadSkin(const char * package, const char * name, struct VSkin * skin, double scale);
//		Loads a skeletal skin into appropriate package, generating said package if necessary
//
//	UNREALED_API void LoadAnimations(const char * package, const char * name, struct VSkin * skin, struct VAnimationList * anims);
//		Loads an animation (after corresponding skin is loaded)
//
//	UNREALED_API void LoadMesh(const char * package, const char * group, const char * name, const UnEditor::FMesh * mesh);
//		Loads a static mesh into appropriate package, generating said package if necessary
//
//	UNREALED_API void LoadMeshWithNormals(const char * package, const char * group, const char * name, const UnEditor::FMesh_1 * mesh);
//		Enhancement of LoadMesh() to load a static mesh that has normals specified.
//
//	UNREALED_API void FinishAnimImport(void);
//		Updates the animation viewer after LoadSkin/LoadAnimations combo
//
//	UNREALED_API void FinishMeshImport(void);
//		Updates static mesh viewer after LoadMesh has been called
//	

#ifndef UNREALED_API
#define UNREALED_API __declspec(dllimport)
#endif

#pragma once

namespace UnEditor {

	struct FMaterial
	{
		char		MaterialName[64]; // Straightforward ascii array, for binary input.
		int         TextureIndex;     // multi/sub texture index 
	};

	struct FUV
	{
		FLOAT	U;
		FLOAT	V;
	};

	struct FVertex
	{
		INT		pointIndex;
		INT		normalIndex;
		INT		uvIndex;
	};

	struct FTriangle
	{
		INT		points[3];
		INT		t_points[3];
		INT		MatIndex;
	};

	struct FTriangle_1	// Enhanced to include normals
	{
		FVertex	vertex[ 3 ];
		INT		materialIndex;
	};

	struct FCollisionTriangle
	{
		INT		points[3];
	};

	struct FMesh
	{
		TArray <FMaterial>			Materials;	// Materials

		TArray <FVector>			Points;		// 3D Points
		TArray <FUV>				TPoints;    // UV Points

		TArray <FTriangle>			Faces;      // Faces
	};

	struct FMesh_1	// Enhanced to include normals 
	{
		TArray <FMaterial>			materials;	// Materials

		TArray <FVector>			points;		// 3D Points
		TArray <FVector>			normals;	// Normals
		TArray <FUV>				uvs;		// UV Points

		TArray <FTriangle_1>		faces;      // Faces
		TArray <FCollisionTriangle>	collision;	// Collision geometry
	};

	// NOTE: This stuff is not used
	struct VBitmapOrigin
	{
		char RawBitmapName[MAX_PATH];
		char RawBitmapPath[MAX_PATH];
	};

	struct VSkin
	{
		TArray <VMaterial>			Materials; // Materials
		TArray <VPoint>				Points;    // 3D Points
		TArray <VVertex>			Wedges;  // Wedges
		TArray <VTriangle>			Faces;       // Faces
		TArray <VBone>				RefBones;   // reference skeleton
		TArray <void*>				RawMaterials; // alongside Materials - to access extra data //Mtl*
		TArray <VRawBoneInfluence>	RawWeights;  // Raw bone weights..
		TArray <VBitmapOrigin>      RawBMPaths;  // Full bitmap Paths for logging

		// Brushes: UV must embody the material size...
		TArray <INT>				MaterialUSize;
		TArray <INT>				MaterialVSize;
		
		int NumBones; // Explicit number of bones (?)
	};


	struct VAnimation
	{
		AnimInfoBinary        AnimInfo;   //  Generic animation info as saved for the engine
		TArray <VQuatAnimKey> KeyTrack;   //  Animation track - keys * bones
		VAnimation()
		{}
	};

	struct VAnimationList
	{
		explicit VAnimationList(TArray<VAnimation> * list = NULL)
			: AnimList(list)
		{}

		TArray<VAnimation> *	AnimList;
	};
};


UNREALED_API INT	LaunchEditor( HINSTANCE hInInstance, char* InCmdLine, INT nCmdShow );
UNREALED_API void	PollEditor(void);
UNREALED_API INT	ShutDownEditor(void);
UNREALED_API bool	IsLaunched(void);
UNREALED_API bool	IsShutdown(void);

UNREALED_API void LoadSkin(const char * package, const char * name, struct UnEditor::VSkin * skin, double scale);
UNREALED_API void LoadAnimations(const char * package, const char * name, struct UnEditor::VSkin * skin, struct UnEditor::VAnimationList * anims, bool merge = false);
UNREALED_API void LoadMesh(const char * package, const char * group, const char * name, const UnEditor::FMesh * mesh);
UNREALED_API void LoadMeshWithNormals(const char * package, const char * group, const char * name, const UnEditor::FMesh_1 * mesh);
UNREALED_API void FinishAnimImport(void);
UNREALED_API void FinishMeshImport(void);

