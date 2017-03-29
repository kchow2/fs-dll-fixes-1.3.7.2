#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs05Mission : public SPMission 
{
public:
	fs05Mission(void)
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
	void CheckRecDeployed(void);
	void RemoveSnoop(void);
	
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
		CheckSnooper,
		RecyDeployed,
		RemoveSnooper,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		f_last;

	// handles
	Handle
		h_first,
		player,
		Drop1,
		Drop2,
		Drop3,
		Drop4,
		recycler,
		pool1,
		pool2,
		pool3,
		Tblue1,
		Tblue2,
		Tblue3,
		Tblue4,
		Attacker,
		nav_1,
		nav_2,
		nav_3,
		ruin_1,
		MechanaFact,
		coms,
		power,
		Snooper,
		SwarmFact,
		SwarmRec,
		swarm_nav,
		base_nav,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		i_last;

	char
		*Orders = "Blue Team will scout ahead for\nbiometal pools , we need you to\ndeploy the recycler and start\n setting up a base .",
		*Pool1 = "This is Blue Team we have a pool\n for you , dropping Nav Beacon.",
		*Pool2 = "We have another pool for you .",
		*Pool3 = "Here's another pool , send out\nthe scavs while we search for more.",
		*Ruins = "we're finding alien ruins, looks\n like they've been empty a long\ntime, we're finding remains of\nmilitary hardware must have been\na battle here.",
		*Attacked = "Commander some of these defences\nare still operational , we are\nunder attack.",
		*Attackers = "We have lost contact with team blue\nwe are detecting movement\njust north of their last known\nposition ,attempting to get\na visual of the area.",
		*Mechana = "We seem to have accidentally\ntriggered some kind of ancient\nplanetary defense system prepare\nfor incoming attackers.",
		*Snoop = "We need to shut these defence systems\ndown fast, we are picking up some\nstrange transmitions in\nthe area dispatching a snooper\nscout to investigate.",
		*Snoop2 = "Commander the snooper scout has arrived,\nit will track down and identify\nthe source of the transmitions\nkeep this unit safe at all costs.",
		*Coms = "Commander we've located the source of\nthe transmitions, it appears to be\nsome kind communications\nrelay, attempting to access the\nnetwork now.",
		*Hacked = "We've managed to temporarily disable\nthe alien defence systems you have\napprox 30 minutes to take \nout the swarm base before they come\nback online.",
		*WinText = "Well done commander, the Swarm\noutpost has been erradicated.",
		*death1 = "DEPLOY NEAR THE BEACON NOT HALF\nA MILE AWAY,MISSION FAILED";
};
DLLBase * BuildMission(void)
{
	return new fs05Mission();
}

void fs05Mission::Setup(void)
{
//  bools
	MissionFailed = false;
	CheckSnooper = false;
	RecyDeployed = false;
	RemoveSnooper = false;
  //  floats

//  handles

//  integers
	mission_state = 0;
}

void fs05Mission::AddObject(Handle h)
{

}

void fs05Mission::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START {
		Ally(1, 2);
		Ally(5, 6);
		Ally(5, 2);
		Ally(6, 2);
		Drop1 = GetHandle("Drop1");
		Drop2 = GetHandle("Drop2");
		Drop3 = GetHandle("Drop3");
		pool1 = GetHandle("pool1");
		pool2 = GetHandle("pool2");
		pool3 = GetHandle("pool3");
		ruin_1 = GetHandle("ruin_1");
		coms = GetHandle("coms");
		SwarmFact = GetHandle("SwarmFact");
		SwarmRec = GetHandle("SwarmRec");
		SetScrap(1, 40);
		SetTeamColor(2, 10, 10, 128);
		Tblue1 = BuildObject("ivtank", 2, "friend_1");
		SetObjectiveName(Tblue1, "Blue 1");
		Tblue2 = BuildObject("ivscout", 2, "friend_2");
		SetObjectiveName(Tblue2, "Blue 2");
		Tblue3 = BuildObject("ivscout", 2, "friend_3");
		SetObjectiveName(Tblue3, "Blue 3");
		Tblue4 = BuildObject("ivscout", 2, "friend_4");
		SetObjectiveName(Tblue4, "Blue 4");
		MechanaFact = BuildObject("obfact", 5, "FactEnemy");
		power = BuildObject("obpgen", 5, "pgen");
		SetAnimation(Drop1, "takeoff", 1);
		
		base_nav = BuildObject("ibnav", 1, "base");
		SetObjectiveName(base_nav, "Deploy Here");
		SetObjectiveOn(base_nav);
		recycler = GetHandle("recycler");
		AdvanceIn(3);
	}
	STATE(1){
		StartAnimation(Drop1);
		AdvanceIn(10);
	}
	STATE(2){
		RemoveObject(Drop1);
		AdvanceIn(3);
	}
	STATE(3){
		SetAnimation(Drop2, "takeoff", 1);
		AdvanceIn(3);
	}
	STATE(4){
		StartAnimation(Drop2);
		AdvanceIn(10);
	}
	STATE(5){
		RemoveObject(Drop2);
		AdvanceIn(3);
	}
	STATE(6){
		SetAnimation(Drop3, "takeoff", 1);
		AdvanceIn(3);
	}
	STATE(7){
		StartAnimation(Drop3);
		AdvanceIn(10);
	}
	STATE(8){
		RemoveObject(Drop3);
		AdvanceIn(3);
	}
	STATE(9){
		ClearObjectives();
		AddObjective(Orders, WHITE);
		Follow(Tblue2, Tblue1, 1);
		Follow(Tblue3, Tblue1, 1);
		Follow(Tblue4, Tblue1, 1);
		Goto(Tblue1, "patrol_1", 1);
		AdvanceIn(0);
	}
	STATE(10){	//POOL1
		Attacker = BuildObject("svscout_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(9);
	}
	STATE(11){
		Attacker = BuildObject("svscout_J", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(9);
	}
	STATE(12){
		Attacker = BuildObject("svscout_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(25);
	}
	STATE(13){
		//If you deploy a scav on the pool, it disappears, and the handle gets invalidated. This screws with GetDistance()!
		if (!IsAround(pool1) || GetDistance(Tblue1, pool1) < 80){
			AdvanceIn(5);
		}
		else{
			SetState(10);
		}
	}
	STATE(14){
		ClearObjectives();
		AddObjective(Pool1, WHITE);
		nav_1 = BuildObject("ibnav", 1, "nav1");
		SetObjectiveName(nav_1, "Pool 1");
		SetObjectiveOn(nav_1);
		AdvanceIn(7);
	}
	STATE(15){
		Goto(Tblue1, "patrol_2", 1);
		Advance();
	}
	STATE(16){	//POOL2
		Attacker = BuildObject("svscout_J", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(9);
	}
	STATE(17){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(9);
	}
	STATE(18){
		Attacker = BuildObject("svscout_A", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(32);
	}
	STATE(19){
		if (!IsAround(pool2) || GetDistance(Tblue1, pool2) < 80){
			AdvanceIn(5);
		}
		else{
			SetState(16);
		}
	}
	STATE(20){
		ClearObjectives();
		AddObjective(Pool2, WHITE);
		nav_2 = BuildObject("ibnav", 1, "nav2");
		SetObjectiveName(nav_2, "Pool 2");
		SetObjectiveOn(nav_2);
		AdvanceIn(7);
	}
	STATE(21){
		Goto(Tblue1, "patrol_3", 1);
		Advance();
	}
	STATE(22){	//POOL3
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(9);
	}
	STATE(23){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(9);
	}
	STATE(24){
		Attacker = BuildObject("svscout_A", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(22);
	}
	STATE(25){
		if (!IsAround(pool3) || GetDistance(Tblue1, pool3) < 80){
			AdvanceIn(5);
		}
		else{
			SetState(22);
		}
	}
	STATE(26){
		ClearObjectives();
		AddObjective(Pool3, WHITE);
		nav_3 = BuildObject("ibnav", 1, "nav3");
		SetObjectiveName(nav_3, "Pool 3");
		SetObjectiveOn(nav_3);
		AdvanceIn(7);
	}
	STATE(27){
		Goto(Tblue1, "patrol_4", 1);
		Advance();
	}
	STATE(28){	//RUIN_1
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(6);
	}
	STATE(29){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(6);
	}
	STATE(30){
		Attacker = BuildObject("svtank_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(18);
	}
	STATE(31){
		if (GetDistance(Tblue1, ruin_1) < 100){
			Ally(1, 6);
			CameraReady();
			Advance();
		}
		else{
			SetState(28);
		}
	}
	STATE(32){
		if (CameraPath("camera_1", 1500, 2500, ruin_1) || CameraCancelled()){
			CameraFinish();
			UnAlly(1, 6);
			AdvanceIn(5);
		}
	}
	STATE(33){
		ClearObjectives();
		AddObjective(Ruins, WHITE);
		AdvanceIn(7);
	}
	STATE(34){
		Goto(Tblue1, "patrol_5", 1);
		Goto(Tblue2, "patrol_5", 1);
		Goto(Tblue3, "patrol_5", 1);
		Goto(Tblue4, "patrol_5", 1);
		UnAlly(5, 2);
		UnAlly(6, 2);
		Advance();
	}
	STATE(35){	//CHECK_BLUE1
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(5);
	}
	STATE(36){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(5);
	}
	STATE(37){
		Attacker = BuildObject("svtank_L", 6, "swspawn_1");
		Goto(Attacker, recycler, 0);
		AdvanceIn(15);
	}
	STATE(38){
		if (!IsAround(Tblue1) || ((float)GetCurHealth(Tblue1) / (float)GetMaxHealth(Tblue1)) < 0.9f){
			Advance();
		}
		else{
			SetState(35);
		}
	}
	STATE(39){	//ATTACKED
		SetPlan("FS05.aip", 5);
		SetScrap(5, 50);
		ClearObjectives();
		AddObjective(Attacked, RED);
		AdvanceIn(25);
	}
	STATE(40){
		ClearObjectives();
		AddObjective(Attackers, WHITE);
		AdvanceIn(25);
	}
	STATE(41){
		Ally(1, 6);
		CameraReady();
		Advance();
	}
	STATE(42){
		if (CameraPath("camera_2", 3500, 5500, MechanaFact) || CameraCancelled()){
			CameraFinish();
			UnAlly(1, 6);
			AdvanceIn(15);
		}
	}
	STATE(43){
		ClearObjectives();
		AddObjective(Mechana, WHITE);
		AdvanceIn(250);
	}
	STATE(44){
		ClearObjectives();
		AddObjective(Snoop, WHITE);
		AdvanceIn(350);
	}
	STATE(45){
		ClearObjectives();
		AddObjective(Snoop2, WHITE);
		SetPlan("FS05b.aip", 6);
		SetScrap(6, 40);
		Snooper = BuildObject("ivsnoop_BD", 2, "LZ");
		SetObjectiveOn(Snooper);
		Ally(5, 2);
		AdvanceIn(20);
	}
	STATE(46){
		CheckSnooper = true;
		Goto(Snooper, "Snoop1", 1);
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(47){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(48){
		Attacker = BuildObject("svscout_A", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(190);
	}
	STATE(49){
		Goto(Snooper, "Snoop2", 1);
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(50){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(51){
		Attacker = BuildObject("svscout_A", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(160);
	}
	STATE(52){
		Goto(Snooper, "Snoop3", 1);
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(53){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(54){
		Attacker = BuildObject("svscout_A", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(160);
	}
	STATE(55){
		Goto(Snooper, coms, 1);
		Attacker = BuildObject("svartl_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(56){
		Attacker = BuildObject("svinst_L", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(9);
	}
	STATE(57){
		Attacker = BuildObject("svscout_A", 6, "swspawn_1");
		Goto(Attacker, Snooper, 0);
		AdvanceIn(10);
	}
	STATE(58){	//CHECK_SNOOP
		if (GetDistance(Snooper, coms) < 60){
			Advance();
		}
	}
	STATE(59){
		ClearObjectives();
		AddObjective(Coms, WHITE);
		AdvanceIn(100);
	}
	STATE(60){
		ClearObjectives();
		AddObjective(Hacked, WHITE);
		if (IsAround(power)){
			RemoveObject(power);
			power = BuildObject("obpgen2", 5, "pgen");
		}
		Goto(Snooper, recycler, 1);
		CheckSnooper = false;
		RemoveSnooper = true;
		AdvanceIn(5);
	}
	STATE(61){
		StartCockpitTimer(1800);
		swarm_nav = BuildObject("ibnav", 1, "swarm_nav");
		SetObjectiveName(swarm_nav, "Swarm Base");
		SetObjectiveOn(swarm_nav);
		Advance();
	}
	STATE(62){	//COUNT_DOWN
		if (!IsAround(SwarmRec) && !IsAround(SwarmFact)){
			ClearObjectives();
			AddObjective(WinText, GREEN);
			SucceedMission(GetTime() + 16, "FS05win.des");
			Advance();
		}
		else if(GetCockpitTimer() <= 0){
			FailMission(GetTime() + 10, "FS05fail1.des");
			MissionFailed = true;
			SetState(99);
		}
	}

	CheckStuffIsAlive();
	if (!RecyDeployed){
		CheckRecDeployed();
	}
	if (RemoveSnooper){
		RemoveSnoop();
	}
}

void fs05Mission::CheckStuffIsAlive(void){

	if (!IsAround(recycler) && !MissionFailed){
		FailMission(GetTime() + 10, "FS05fail2.des");
		MissionFailed = true;
	}
	if (CheckSnooper && !IsAround(Snooper) && !MissionFailed){
		FailMission(GetTime() + 10, "FS05fail3.des");
		MissionFailed = true;
	}
	if (!IsAround(coms) && !MissionFailed){
		FailMission(GetTime() + 10, "FS05fail5.des");
		MissionFailed = true;
	}
}

void fs05Mission::CheckRecDeployed(void){
	if(IsOdf(recycler, "ibrecy_BD5")){
		RecyDeployed = true;
		if (GetDistance(recycler, base_nav) > 250 && !MissionFailed){
			FailMission(GetTime() + 10, "FS05fail4.des");
			MissionFailed = true;
		}
	}
}

void fs05Mission::RemoveSnoop(void){
	if (GetDistance(Snooper, recycler) < 40){
		RemoveObject(Snooper);
		RemoveSnooper = false;
	}
}


