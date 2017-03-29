#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs02Mission : public SPMission 
{
public:
	fs02Mission(void)
	{
		AllowRandomTracks(true);
		b_count = &b_last - &b_first - 1;
		b_array = &b_first + 1;

		f_count = &f_last - &f_first - 1;
		f_array = &f_first + 1;

		h_count = &h_last - &h_first - 1;
		h_array = &h_first + 1;

		i_count = &i_last - &i_first - 1;
		i_array = &i_first + 1;
	}

	void Setup(void);
	void AddObject(Handle h);
	void Execute(void);

	//advances the mission_state by one and waits for the specified time
	void AdvanceIn(float delay){
		next_event_time = GetTime() + delay;
		mission_state++;
	}
	//advances the mission state by one
	void Advance(void){
		AdvanceIn(0);
	}
	//Go to the specified state
	void SetState(int state, float delay = 0){
		next_event_time = GetTime() + delay;
		mission_state = state;
	}

	// bools
	bool
		b_first,

		b_last;

	// floats
	float
		f_first,
		next_event_time,
		f_last;

	// handles
	Handle
		h_first,
		Player,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		i_last;
};
DLLBase * BuildMission(void)
{
	return new fs02Mission();
}

void fs02Mission::Setup(void)
{
	//SetAutoGroupUnits(false);
/*
	Here's where you set the values at the start.  
*/

//  bools

  //  floats

//  handles

//  integers
	mission_state = 0;
}

void fs02Mission::AddObject(Handle h)
{

}



void fs02Mission::Execute(void)
{
	Player = GetPlayerHandle();
	MISSION_START{
		SucceedMission(GetTime() + 5, "Winner.des");
		AdvanceIn(5);
	}
	STATE(1){

	}
	
}