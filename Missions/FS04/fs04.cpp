#include "..\Shared\SPMission.h"

//Some useful defines for defining mission states.
#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs04Mission : public SPMission
{
public:
	fs04Mission(void)
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
	void CheckCollapse(void);
	void StartCollapse(Handle collapse);
	void SpawnCrab(Vector location);

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
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		CollapseTimer,
		f_last;

	// handles
	Handle
		h_first,
		dropship,
		constructor,
		collapsing_now,
		collapse_1,
		collapse_2,
		collapse_3,
		collapse_4,
		collapse_5,
		collapse_6,
		nav_1,
		nav_2,
		nav_3,
		nav_4,
		scav1,
		scav2,
		scav3,
		scav4,
		Attacker,
		rocks,
		radar,
		power,
		player,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		CollapseState,
		i_last;

	Vector
		Crabs1 = { 286, 19, -1641 },
		Crabs2 = { 1197, 53, -446 },
		Crabs3 = { -228, 24, -198 },
		Crabs4 = { -1614, 42, -212 },
		Crabs5 = { -683, 42, -584 },
		Crabs6 = { -362, 36, -925 },
		Crabs7 = { 548, 20, 555 };

	char
		*Orders = "Commander,escort the constructor\nwhile it builds a series of plasma\nbatteries,be careful of unstable\nterrain.",
		*Scavs = "It appears that the local wildlife\nhas taken a dislike to us,protect\nthose extractors at all costs ",
		*Const = "The terrain ahead is highly\nunstable guide the constructor\nthrough to nav beacon 3 .",
		*Const2 = "Well done , one more left to\ngo,guide the constructor to\nnav beacon 4,but be\nquick,the alien fleet is\nalmost upon us. ",
		*Fleet = "ATTENTION , we are getting reports\nthat the alien fleet is non-hostile\nrepeat,alien fleet is non-hostile,\nabort mission immediately .",
		*NewOrders = "We don't have much time,we need\nto shut down the plasma batteries,\n you can to do this from\nthe radar tracking station\nhighlighted on your HUD.",
		*Radar = "It's too late , we don't have time\n to shut the station down manually\nyou'll have to find another way\nto take it offline.",
		*Win = "Well done you have shutdown the\nradar tracking system in time .";
};
DLLBase * BuildMission(void)
{
	return new fs04Mission();
}

void fs04Mission::Setup(void)
{
	MissionFailed = false;
	mission_state = 0;
	CollapseState = 0;
}

void fs04Mission::AddObject(Handle h)
{

}

void fs04Mission::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START{
		dropship = GetHandle("dropship");
		nav_1 = GetHandle("nav_1");
		nav_2 = GetHandle("nav_2");
		nav_3 = GetHandle("nav_3");
		nav_4 = GetHandle("nav_4");
		collapse_1 = GetHandle("collapse_1");
		collapse_2 = GetHandle("collapse_2");
		collapse_3 = GetHandle("collapse_3");
		collapse_4 = GetHandle("collapse_4");
		collapse_5 = GetHandle("collapse_5");
		collapse_6 = GetHandle("collapse_6");
		rocks = GetHandle("rocks");
		radar = GetHandle("radar");
		power = GetHandle("power");
		scav1 = GetHandle("scav1");
		scav2 = GetHandle("scav2");
		scav3 = GetHandle("scav3");
		scav4 = GetHandle("scav4");
		constructor = GetHandle("const");
		SetAnimation(dropship, "takeoff", 1);
		AdvanceIn(3);
	}
	STATE(1){
		StartAnimation(dropship);
		AdvanceIn(10);
	}
	STATE(2){
		RemoveObject(dropship);
		ClearObjectives();
		AddObjective(Orders, WHITE);
		Goto(constructor, nav_1, 1);
		SetObjectiveOn(constructor);
		Advance();
	}
	STATE(3){	//GO_TO_NAV_1
		if (GetDistance(constructor, nav_1) < 180){
			Build(constructor, "IbplasBD1", 1);	//IMPORTANT: Build() and Deploy() can't be called in the same tick, otherwise the constructor won't build the thing
			Advance();
		}
	}
	STATE(4){
		Dropoff(constructor, "tower_1", 1);
		AdvanceIn(50);

	}
	STATE(5){
		Goto(constructor, nav_2, 1);
		Attacker = BuildObject("TvCrab", 6, Crabs1);
		Attack(Attacker, scav1, 1);
		AdvanceIn(2);
	}
	STATE(6){
		Goto(constructor, nav_2, 1);
		Attacker = BuildObject("TvCrab", 6, Crabs1);
		Attack(Attacker, scav1, 1);
		AdvanceIn(2);
	}
	STATE(7){
		Goto(constructor, nav_2, 1);
		Attacker = BuildObject("TvCrab", 6, Crabs1);
		Attack(Attacker, scav2, 1);
		AdvanceIn(2);
	}
	STATE(8){
		Goto(constructor, nav_2, 1);
		Attacker = BuildObject("TvCrab", 6, Crabs2);
		Attack(Attacker, scav3, 1);
		AdvanceIn(2);
	}
	STATE(9){
		Goto(constructor, nav_2, 1);
		Attacker = BuildObject("TvCrab", 6, Crabs2);
		Attack(Attacker, scav4, 1);
		AdvanceIn(2);
	}
	STATE(10){
		Goto(constructor, nav_2, 1);
		Attacker = BuildObject("TvCrab", 6, Crabs2);
		Attack(Attacker, scav4, 1);
		AdvanceIn(6);
	}
	STATE(11){
		CameraReady();
		Advance();
	}
	STATE(12){
		if (CameraPath("camera_1", 1500, 2500, scav2) || CameraCancelled())
			Advance();
	}
	STATE(13){
		CameraFinish();
		ClearObjectives();
		AddObjective(Scavs, WHITE);
		AdvanceIn(115);
	}
	STATE(14){
		SpawnCrab(Crabs3);
		AdvanceIn(4);
	}
	STATE(15){
		SpawnCrab(Crabs3);
		AdvanceIn(4);
	}
	STATE(16){
		SpawnCrab(Crabs3);
		AdvanceIn(4);
	}
	STATE(17){	//GO_TO_NAV_2
		AdvanceIn(20);
	}
	STATE(18){
		SpawnCrab(Crabs3);
		if (GetDistance(constructor, nav_2) < 180){
			SpawnCrab(Crabs6);
			AdvanceIn(5);
		}
		else{
			SetState(17);
		}
	}
	STATE(19){
		Build(constructor, "IbplasBD1", 1);
		Advance();
	}
	STATE(20){
		SpawnCrab(Crabs6);
		Dropoff(constructor, "tower_2", 1);
		SpawnCrab(Crabs6);
		AdvanceIn(5);
	}
	STATE(21){
		SpawnCrab(Crabs6);
		AdvanceIn(70);
	}
	STATE(22){
		Goto(constructor, "path_1", 1);
		Advance();
	}
	STATE(23){	//GO_TO_ROCKS
		SpawnCrab(Crabs6);
		AdvanceIn(20);
	}
	STATE(24){
		if (GetDistance(constructor, rocks) < 80){
			Goto(constructor, rocks, 0);
			ClearObjectives();
			AddObjective(Const, WHITE);
			Advance();
		}
		else{
			SetState(23);
		}
	}
	STATE(25){	//GO_TO_NAV3		
		SpawnCrab(Crabs4);
		AdvanceIn(15);
	}
	STATE(26){
		SpawnCrab(Crabs5);
		AdvanceIn(10);
	}
	STATE(27){
		if (GetDistance(constructor, nav_3) < 80){
			Build(constructor, "IbplasBD1", 1);
			Advance();
		}
		else{
			SetState(26);
		}
	}
	STATE(28){
		Dropoff(constructor, "tower_3", 1);
		AdvanceIn(50);
	}
	STATE(29){
		Goto(constructor, nav_3, 0);
		ClearObjectives();
		AddObjective(Const2, WHITE);
		Advance();
	}
	STATE(30){	//NAV_4
		SpawnCrab(Crabs7);
		AdvanceIn(20);
	}
	STATE(31){
		if (GetDistance(constructor, nav_4) < 80){
			ClearObjectives();
			AddObjective(Fleet, RED);
			AdvanceIn(15);
		}
		else{
			SetState(30);
		}
	}
	STATE(32){
		ClearObjectives();
		AddObjective(NewOrders, GREEN);
		AdvanceIn(6);
	}
	STATE(33){
		CameraReady();
		Advance();
	}
	STATE(34){
		if (CameraPath("camera_2", 1500, 3000, radar) || CameraCancelled()){
			CameraFinish();
			AdvanceIn(5);
		}
	}
	STATE(35){
		SetUserTarget(radar);
		StartCockpitTimer(205);
		Advance();
	}
	STATE(36){	//COUNTDOWN
		if (GetDistance(player, radar) < 80){
			Advance();
		}
		else if (GetCockpitTimer() <= 0){
			FailMission(GetTime() + 10, "FS04fail3.des");
			mission_state = 99;
			SetState(99);
		}
	}
	STATE(37){	//MESSAGE
		ClearObjectives();
		AddObjective(Radar, WHITE);
		Advance();
	}
	STATE(38){	//POWER	
		if (!IsAround(power)){
			Advance();
		}
		else if (GetCockpitTimer() <= 0){
			FailMission(GetTime() + 10, "FS04fail3.des");
			SetState(99);
		}
	}
	STATE(39){	//WIN
		StopCockpitTimer();
		ClearObjectives();
		AddObjective(Win, GREEN);
		SucceedMission(GetTime() + 16, "FS04win.des");
		Advance();
	}

	CheckStuffIsAlive();
	CheckCollapse();
}

void fs04Mission::CheckStuffIsAlive(void){
	if (!IsAround(constructor) && !MissionFailed){
		FailMission(GetTime() + 10, "FS04fail1.des");
	}
	bool ScavsAlive = IsAround(scav1) && IsAround(scav2) && IsAround(scav3) && IsAround(scav4);
	if (!ScavsAlive && !MissionFailed){
		FailMission(GetTime() + 10, "FS04fail2.des");
	}
}

void fs04Mission::CheckCollapse(void){
	if (CollapseState == 0){
		if (IsAround(collapse_1) && GetDistance(constructor, collapse_1) < 40){
			StartCollapse(collapse_1);
		}
		else if (IsAround(collapse_2) && GetDistance(constructor, collapse_2) < 40){
			StartCollapse(collapse_2);
		}
		else if (IsAround(collapse_3) && GetDistance(constructor, collapse_3) < 40){
			StartCollapse(collapse_3);
		}
		else if (IsAround(collapse_4) && GetDistance(constructor, collapse_4) < 40){
			StartCollapse(collapse_4);
		}
		else if (IsAround(collapse_5) && GetDistance(constructor, collapse_5) < 40){
			StartCollapse(collapse_5);
		}
		else if (IsAround(collapse_6) && GetDistance(constructor, collapse_6) < 40){
			StartCollapse(collapse_6);
		}
	}
	else if (CollapseState == 1 && GetTime() > CollapseTimer){
		StartAnimation(collapsing_now);
		CollapseTimer = GetTime() + 4;
		CollapseState++;
	}
	else if (CollapseState == 2 && GetTime() > CollapseTimer){
		RemoveObject(collapsing_now);
		CollapseState = 0;
	}
}

void fs04Mission::StartCollapse(Handle collapse){
	collapsing_now = collapse;
	SetAnimation(collapsing_now, "break", 1);
	CollapseState = 1;
	CollapseTimer = GetTime() + 2;
}

void fs04Mission::SpawnCrab(Vector position){
	Attacker = BuildObject("TvCrab", 6, position);
	Attack(Attacker, constructor, 1);
}