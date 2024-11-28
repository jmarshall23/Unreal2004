/***********************************************************************************************
*
*	$Id: driver.hpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*
************************************************************************************************/
#ifndef DRIVER_HPP
#define DRIVER_HPP

#define MAX_COURSES (10)
#define MAX_COURSE_POINTS (50)

typedef struct course_point
{
	RwInt32 id, next_id;
	RwV2d pos;			//location of control point
	RwReal hdg;			//required heading through point
	RwReal spd;			//required speed through point
	RwReal tol1;	//radius around point
	RwReal tol2;	//radius around point
} COURSE_POINT;

typedef struct course_s
{
	RwChar name[50];
	RwInt32 num_points, start_point;
	COURSE_POINT *course_points;
} COURSE;

typedef struct shape_cmpt
{
	const MeReal *me_mtx;
	RpClump *clump;
} SHAPE_CMPT;

typedef enum {
	COMP_CONTROL = 0,
	PLAYER_CONTROL,
	NO_CONTROL
} CTRL_METHOD;

class Driver
{
private:
	COURSE_POINT	*curr_point;
	//KeaKart			*car;
	Car				*car;
	CTRL_METHOD		ctrl_method;
	int				id;
	MeReal		trapped_timer; //timer used to determine if car is trapped and needs to be reset
	int			trapped, timer_state;
	MeReal		lap_timer, best_time;
	void SetCurrentPointOnCourse();
	void DriveToCurrentPoint();
public:
	MeReal		steering_p_gain, steering_tcp_gain;
	MeReal		steering_d_gain;
	MeReal		steering_slip_gain;
	MeReal		throttle_p_gain;
	SHAPE_CMPT		car_cmpts[NUM_CAR_CMPTS];

	RpClump *internal_shape;
	RwTexture *car_texture;
	char car_data_file[MAX_PATHLEN];
//	char cmpt_names[NUM_CAR_CMPTS][MAX_PATHLEN];
	char texture_path[MAX_PATHLEN];
	char shape_dir[MAX_PATHLEN];
	int	track;
protected:

public:
	Driver();
	~Driver();
	static int num_drivers;
	static MeReal race_best;

	CTRL_METHOD GetControlMethod() { return ctrl_method; }
	void SetControlMethod(CTRL_METHOD m);
	void DoControl();
	void DoSmoke();
	void DoLapTimes();
	void Get2DPosition(RwV2d *);
	void Get2DVelocity(RwV2d *);
//	void PreEvolve(MeReal t) { car->PreEvolve(t); }
//	void PostEvolve();
	//KeaKart *GetCar() { return car; }
	Car *GetCar() { return car; }
	void SetSteering(MeReal s) { car->SetSteeringAngle(s > 1.0f ? 1.0f : s < -1.0f ? -1.0f : s); }
	void SetThrottle(MeReal t) { car->SetDrive(t > 1.0f ? 1.0f : t < -1.0f ? -1.0f : t); }
	void SetTranny(int type) { car->SetTrannyType(type);}
	//void SetRThrottle(MeReal t) {}; //{ car->SetRThrottle(t); }
	//void SetLThrottle(MeReal t) {}; //{ car->SetLThrottle(t); }
	void SetPosition(MeReal x,MeReal y) {}; //{ car->SetPosition(x,y); }
	void SetPosition(MeReal x,MeReal y,MeReal z) {}; //{car->SetPosition(x,y,z); }
	RwReal GetMotorRPM();
	MeReal GetBestTime() { return best_time; }
	MeReal GetLapTime() { return lap_timer; }

	void   Upshift();
	void   Downshift();
	RwBool LoadAndInitShapes();
	void UpdateShapes();
	RpClump *GetChassisClump() { return car_cmpts[CHASSIS_CMPT].clump; }
	void SetSpeedo();
	int GetID() { return id; }
	int GetPointID() { return curr_point->id; }
	void TurnOffCarShape();
	void TurnOnCarShape();

	int OutsideWorld();
	void Update();
	void Reset();
	void ResetOnTrackIfOK();
	int CheckIfTrapped();



	int is_skidding;
#ifdef USE_SOUND
	// handles to sound channels
	int v8snd;
	int enginesnd;

	// handle to actual sound
	int skidsound;
	int channel1;
	int channel2;
	int channel3;

#endif // USE_SOUND

};

extern Driver **drivers;
//extern COURSE_POINT coursepoints[];
extern Driver *player;
extern COURSE tracks[MAX_COURSES];
extern int num_tracks;

#endif //DRIVER_HPP
