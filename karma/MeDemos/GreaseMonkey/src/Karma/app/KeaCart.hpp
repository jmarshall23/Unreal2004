/***********************************************************************************************
*
*	$Id: KeaCart.hpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*
************************************************************************************************/
// wild willy behavior

#ifndef KEACAR_H
#define KEACAR_H

//typedef MeReal MyMatrix[4][4];

typedef MeReal MeMatrix[16];

//static MeReal me_mtx[5][16];

#include "Mdt.h"

class KeaKart
{
  // dynamics stuff


//  keaBody *body[5];					// wheels (bl,br,fl,fr) and chassis
//keaHingeJoint *back_joint[2];		// joints to the 2 back wheels
//keaSteeringHingeJoint1 *front_joint[2]; // joints to the 2 front wheels
	MdtBody *body[5];					// wheels (bl,br,fl,fr) and chassis
	MdtHinge *back_joint[2];		// joints to the 2 back wheels
  //MdtBSJoint *back_joint[2];		// joints to the 2 back wheels

	MdtHinge *front_joint[2];		// joints to the 2 back wheels
  //MdtBSJoint *front_joint[2];
  //MdtSteeringHinge1 *front_joint[2]; // joints to the 2 front wheels

  //MeReal initial_pos [5][3];	// initial pos's relative to chassis

	MdtBody *steering_body[2]; //intermediate bodies
	MdtHinge *steering_joint[2];

	MeReal wheel_radius;
	MeReal max_str;
  // I/O stuff

	MeMatrix4 mcd_mtx_mem[5];
	MeReal steer, rthrust,lthrust;

	MeReal curr_steer;
	MeReal curr_theta[2];
	//void SetInitialPosition(void);
    void stop();
  void clear();
public:
  KeaKart();
  ~KeaKart();


  static int num_cars;
  int car_num;

  MeMatrix me_mtx[5];
  const MeReal *GetCmptMeTransformationMatrix(int i) {return me_mtx[i]; }

  void init();
  void start();

  void PreEvolve(MeReal);
  void PostEvolve();

  void SetPosition(MeReal x, MeReal y);
  void SetPosition(MeReal x, MeReal y, MeReal z);
	void UpdateMeShapeMatrices();
  MeReal GetBodyPosCmpt(int b, int i); // { return body[b]->pos[i]; }
  MeReal GetBodyVelCmpt(int b, int i); // { return body[b]->vel[i]; }
  void SetSteering(MeReal str) { steer = str>max_str?max_str:str<-max_str?-max_str:str; }

  void SetThrottle(MeReal thr) { rthrust = lthrust = thr>1.0f?1.0f:thr<-1.0f?-1.0f:thr; }
  void SetRThrottle(MeReal thr) { rthrust = thr>1.0f?1.0f:thr<-1.0f?-1.0f:thr; }
  void SetLThrottle(MeReal thr) { lthrust = thr>1.0f?1.0f:thr<-1.0f?-1.0f:thr; }
  void FlipCar();
  MeReal GetEngineRPM();
};

#endif //KEACAR_H
