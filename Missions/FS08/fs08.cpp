#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs08Mission : public SPMission 
{
public:
	fs08Mission(void)
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
	void CheckStuffIsAlive(void);

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
		MissionFailed,
		CheckRecycler,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		f_last;

	// handles
	Handle
		h_first,
		S1recy,
		S2recy,
		launcher1,
		launcher2,
		launcher3,
		Player,
		Nav1,
		Nav2,
		Nav3,
		BaseNav,
		Recycler,
		PRover,
		PRproc,
		PRgt1,
		PRgt2,
		PRgt3,
		PRgt4,
		PRgt5,
		PRsbay,
		PRcom,
		PRserv1,
		PRserv2,
		PRserv3,
		Turr1,
		Turr2,
		Turr3,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		i_last;

	char
		*Orders = "The Swarm is attempting to escape.\nTake out those launchers.\nWe've dropped navs to help you.\nBe quick, time is short.",
		*Orders2 = "Well done Commander, now build a\nbase and destroy the\nremaining swarm forces. A\nrecycler is on it's way\nto you.";
};
DLLBase * BuildMission(void)
{
	return new fs08Mission();
}

void fs08Mission::Setup(void)
{
//  bools
	MissionFailed = false;
	CheckRecycler = false;
//  integers
	mission_state = 0;

	//This should fix the lag spike at the very start of the mission when stuff is spawned in.
	PreloadODF("sbrecy00");
	PreloadODF("Pvcons");
	PreloadODF("pbproc");
	PreloadODF("pbrecy");
	PreloadODF("ivrecyBD");
}

void fs08Mission::AddObject(Handle h)
{

}

void fs08Mission::Execute(void)
{
	Player = GetPlayerHandle();
	MISSION_START{
		Ally(5, 6);
		Ally(1, 2);
		S1recy = GetHandle("S1recy");
		launcher1 = GetHandle("launcher1");
		launcher2 = GetHandle("launcher2");
		launcher3 = GetHandle("launcher3");
		S2recy = BuildObject("sbrecy00", 5, "S2recy");
		PRproc = BuildObject("pbproc", 2, "PRproc");
		PRover = BuildObject("Pvcons", 2, "PRover");
		PRgt1 = BuildObject("Pbgtow", 2, "PRgt1");
		PRgt2 = BuildObject("Pbgtow", 2, "PRgt2");
		PRgt3 = BuildObject("Pbgtow", 2, "PRgt3");
		PRgt4 = BuildObject("Pbgtow", 2, "PRgt4");
		PRgt5 = BuildObject("Pbgtow", 2, "PRgt5");
		PRcom = BuildObject("Pbcoms", 2, "PRcom");
		PRsbay = BuildObject("Pbsbay", 2, "PRsbay");
		PRserv1 = BuildObject("Pvserv", 2, "PRover");
		PRserv1 = BuildObject("Pvserv", 2, "PRover");
		PRserv1 = BuildObject("Pvserv", 2, "PRover");
		SetScrap(2, 40);
		SetScrap(5, 40);
		SetScrap(6, 40);
		SetPlan("FS08_s1.aip", 6);
		SetPlan("FS08_s2.aip", 5);
		SetPlan("FS08_p1.aip", 2);
		Nav1 = BuildObject("ibnav", 1, "Nav1");
		SetObjectiveName(Nav1, "Launcher 1");
		SetObjectiveOn(Nav1);
		Nav2 = BuildObject("ibnav", 1, "Nav2");
		SetObjectiveName(Nav2, "Launcher 2");
		SetObjectiveOn(Nav2);
		Nav3 = BuildObject("ibnav", 1, "Nav3");
		SetObjectiveName(Nav3, "Launcher 3");
		SetObjectiveOn(Nav3);
		ClearObjectives();
		AddObjective(Orders, WHITE);
		StartCockpitTimer(600);
		Advance();
	}
	STATE(1){	//COUNT_DOWN
		if (!IsAround(launcher1) && 
			!IsAround(launcher2) && 
			!IsAround(launcher3)){
			Advance();
		}
		else if (GetCockpitTimer() <= 0){
			FailMission(GetTime() + 10, "FS08fail1.des");
			MissionFailed = true;
			SetState(99);
		}
	}
	STATE(2){	//NEXT
		StopCockpitTimer();
		HideCockpitTimer();	//fixed the mission timer not going away after part 1
		SetObjectiveOff(Nav1);
		SetObjectiveOff(Nav2);
		SetObjectiveOff(Nav3);
		BaseNav = BuildObject("ibnav", 1, "BaseNav");
		SetObjectiveName(BaseNav, "Deploy Recycler Here");
		SetObjectiveOn(BaseNav);
		ClearObjectives();
		AddObjective(Orders2, GREEN);
		AdvanceIn(10);
	}
	STATE(3){
		Recycler = BuildObject("ivrecyBD", 1, "Recycler");
		Turr1 = BuildObject("ivtur_BD", 1, "Recycler");
		Turr2 = BuildObject("ivtur_BD", 1, "Recycler");
		Turr3 = BuildObject("ivtur_BD", 1, "Recycler");
		SetPlan("FS08_s3.aip", 6);
		SetPlan("FS08_s4.aip", 5);
		SetPlan("FS08_p2.aip", 2);
		SetScrap(1, 40);
		Goto(Recycler, BaseNav, 0);
		Follow(Turr1, Recycler, 0);
		Follow(Turr2, Recycler, 0);
		Follow(Turr3, Recycler, 0);
		CheckRecycler = true;
		Advance();
	}
	STATE(4){	//CHECK_SWARM
		if (!IsAround(S1recy) && !IsAround(S2recy)){
			SucceedMission(GetTime() + 10, "FS08win.des");
			Advance();
		}
	}
	CheckStuffIsAlive();
}

void fs08Mission::CheckStuffIsAlive(void){
	if (CheckRecycler && !IsAround(Recycler) && !MissionFailed){
		FailMission(GetTime() + 10, "FS08fail2.des");
		MissionFailed = true;
	}
}