/***********************************************************************************************
*
*	$Id: carAPI.hpp,v 1.1.2.2 2002/03/03 17:11:45 richardm Exp $
*
************************************************************************************************/

#ifndef _CARAPI_H
#define _CARAPI_H

class Car : public MdtCar {

	MeReal GraphicsTMatrix[5][16];
	void UpdateShapeMatrices();

public:
	Car();
	~Car();
    const MeReal *GetCmptMeTransformationMatrix(int i) {return &GraphicsTMatrix[i][0]; }

    void CreatePhysics();
	void Init(char *);
	void Update();
	void ReLoaddataFile();
	void SetPosition(MeReal x, MeReal y);
	void SetPosition(MeReal x, MeReal y, MeReal z);

};


extern void InitialiseCarSim(RpWorld *);    // create mathengine framework and world
extern void TerminateCarSim();  // delete mathengine framework and world
extern void SysEvolve(MeReal delta_t);

#endif //_CARAPI_H
