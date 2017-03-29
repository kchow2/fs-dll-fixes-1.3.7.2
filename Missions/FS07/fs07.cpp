#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs07Mission : public SPMission 
{
public:
	fs07Mission(void)
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
	void SpawnSwarmAttackWaves1(void);
	void SpawnSwarmAttackWaves2(void);
	void Replace(Handle &h, char* newOdf);

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
		CheckT3Rec,
		CheckBackup,
		BuiltPower,
		BuiltFac,
		BuiltSBay,
		BuiltArmo,
		MakeItHarder,
		SwarmAttackWaves1,
		SwarmAttackWaves2,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		SwarmAttackTimer1,
		SwarmAttackTimer2,
		f_last;

	// handles
	Handle
		h_first,
		Probe,
		Player,
		Recycler,
		Attacker,
		Attacker2,
		t3rec,
		t3Recycler,
		t3fact,
		t3power1,
		t3power2,
		t3power3,
		t3com1,
		t3com2,
		t3GT1,
		t3GT2,
		t3GT3,
		PRproc,
		PRfact,
		PRcyard,
		PRpow_1,
		PRpow_2,
		PRpow_3,
		PRpow_4,
		PRGT_1,
		PRGT_2,
		PRGT_3,
		PRGT_4,
		PRGT_5,
		PRmine_1,
		PRmine_2,
		PRmine_3,
		PRcons,
		base_nav,
		swarm_nav,
		Brec,
		PlayerFac,
		PlayerPow,
		PlayerBay,
		PlayerArmo,
		PRoverlord,
		Sbuild,
		Swalk1,
		Swalk2,
		Backup1,
		Backup,
		PRserv_1,
		PRserv_2,
		Mlead1,
		Mlead2,
		Mwing1,
		Mwing2,
		Mwing3,
		Mwing4,
		Mwing5,
		SwarmRec,
		newguy,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		SwarmAttackState1,
		SwarmAttackState2,
		i_last;

	char
		*Orders1 = "Commander setup base at the nav\nbeacon,prevent the swarm from\nleaving the infested zone.\nDo not enter this zone\nmechana defenses are still active.",
		*death1 = "NEAR THE BEACON NOT HALF\nA MILE AWAY,MISSION FAILED",
		*EDF ="Commander the EDF base to the\neast has been compromised,their\nRecycler is lost,take your\nforces and defend the area\nuntil a replacement arrives.",
		*NewOrders ="Well done the breach is sealed\nhowever a significant swarm force\nhas broken through find\nand eliminate them,they\nwere last spotted moving north.",
		*SwarmDead ="The Swarm force has been\ndestroyed.We are picking up\npowerful energy readings from\nthe ruins in this sector\ninvestigate and find the source.",
		*ProbeMsg ="Commander whatever the hell\nthat thing is, guard it\nwith your life,backup is on\nit's way.",
		*Orders2 = "We've dropped a nav at the\nlast known position of the\nswarm force,a powerful energy\nsource in the area is\nblocking our scans.";
};
DLLBase * BuildMission(void)
{
	return new fs07Mission();
}

void fs07Mission::Setup(void)
{
//  bools
	MissionFailed = false;
	CheckT3Rec = false;
	CheckBackup = false;
	BuiltPower = false;
	BuiltFac = false;
	BuiltSBay = false;
	BuiltArmo = false;
	MakeItHarder = false;
	SwarmAttackWaves1 = false;
	SwarmAttackWaves2 = false;
  //  floats
	SwarmAttackTimer1 = 0;
	SwarmAttackTimer2 = 0;

//  handles
	PlayerPow = NULL;
	PlayerFac = NULL;
	PlayerArmo = NULL;
	PlayerBay = NULL;

//  integers
	mission_state = 0;
	SwarmAttackState1 = 0;
	SwarmAttackState2 = 0;

	//Preloading helps to reduce lag when these things are spawned in for the first time
	PreloadODF("Sbrecy00");
	PreloadODF("Svcons");
	PreloadODF("ibrecy");
	PreloadODF("pbrecy");
	PreloadODF("pvcons");
}

void fs07Mission::AddObject(Handle h){
	char ODFName[64];
	int team = GetTeamNum(h);
	GetObjInfo(h, Get_CFG, ODFName);

	if (!IsAround(PlayerPow) && IsOdf2(h, "ibpgenBD") && team == 1){
		PlayerPow = h;
		BuiltPower = true;
	}
	else if (!IsAround(PlayerFac) && IsOdf2(h, "ibfactBD") && team == 1){
		PlayerFac = h;
		BuiltFac = true;
	}
	else if (!IsAround(PlayerArmo) && IsOdf2(h, "ibarmoBD") && team == 1){
		PlayerArmo = h;
		BuiltArmo = true;
	}
	else if (!IsAround(PlayerBay) && IsOdf2(h, "ibsbayBD") && team == 1){
		PlayerBay = h;
		BuiltSBay = true;
	}

	if (team != 3 && team != 6){
		int skill = MakeItHarder ? 3 : 2;
		SetSkill(h, skill);
	}
}

void fs07Mission::Execute(void)
{
	Player = GetPlayerHandle();
	MISSION_START{
		Ally(1, 2);
		Ally(1, 3);
		Ally(2, 3);
		Ally(5, 6);
		Ally(5, 2);
		SetScrap(1, 40);
		SetScrap(2, 40);
		SetScrap(3, 40);
		SetScrap(5, 40);
		Probe = GetHandle("Probe");
		Recycler = GetHandle("Recycler");
		t3fact = BuildObject("ibfact", 3, "t3fact");
		t3rec = BuildObject("ibrecy", 3, "t3rec");
		t3power1 = BuildObject("ibpgen", 3, "t3power1");
		t3power2 = BuildObject("ibpgen", 3, "t3power2");
		t3power3 = BuildObject("ibpgen", 3, "t3power3");
		t3com1 = BuildObject("ibcbun", 3, "t3com1");
		t3com2 = BuildObject("ibcbun", 3, "t3com2");
		t3GT1 = BuildObject("ibgtow", 3, "t3GT1");
		t3GT2 = BuildObject("ibgtow", 3, "t3GT2");
		t3GT3 = BuildObject("ibgtow", 3, "t3GT3");

		PRproc = BuildObject("pbproc", 2, "PRproc");
		PRfact = BuildObject("pbfact", 2, "PRfact");
		PRcyard = BuildObject("pbrecy", 2, "PRcyard");
		PRpow_1 = BuildObject("pbpgen", 2, "PRpow_1");
		PRpow_2 = BuildObject("pbpgen", 2, "PRpow_2");
		PRpow_3 = BuildObject("pbpgen", 2, "PRpow_3");
		PRpow_4 = BuildObject("pbpgen", 2, "PRpow_4");
		PRGT_1 = BuildObject("pbgtow", 2, "PRGT_1");
		PRGT_2 = BuildObject("pbgtow", 2, "PRGT_2");
		PRGT_3 = BuildObject("pbgtow", 2, "PRGT_3");
		PRGT_4 = BuildObject("pbgtow", 2, "PRGT_4");
		PRGT_5 = BuildObject("pbgtow", 2, "PRGT_5");
		PRmine_1 = BuildObject("pbsturr", 2, "PRmine_1");
		PRmine_2 = BuildObject("pbmturr", 2, "PRmine_2");
		PRmine_3 = BuildObject("pbsturr", 2, "PRmine_3");
		PRoverlord = BuildObject("pvcons", 2, "PRove");
		PRserv_1 = BuildObject("pvserv", 2, "PRserv_1");
		PRserv_2 = BuildObject("pvserv", 2, "PRserv_2");

		Sbuild = BuildObject("Svcons", 5, "Sbuild");
		SetPlan("FS07_s1.aip", 5);

		base_nav = BuildObject("ibnav", 1, "base");
		SetObjectiveName(base_nav, "Base Area");
		SetObjectiveOn(base_nav);
		ClearObjectives();
		AddObjective(Orders1, WHITE);
		SetPlan("FS07_e1.aip", 3);
		SetPlan("FS07_p1.aip", 2);
		Advance();
	}
	STATE(1){	//REC_DEPLOYED
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_2");
		Attack(Attacker, PRcyard, 1);
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_3");
		Attack(Attacker, t3rec, 1);
		AdvanceIn(10);
	}
	STATE(2){	
		if (IsOdf(Recycler, "ibrecy_BD")){
			if (GetDistance(Recycler, base_nav) > 250){
				ClearObjectives();
				AddObjective(death1, RED);
				FailMission(GetTime() + 10, "FS07fail1.des");
				MissionFailed = true;
				SetState(99);
			}
			else{
				Advance();
			}
		}
		else{
			SetState(1, 10);
		}
	}
	STATE(3){	//CONTINUE_1
		AdvanceIn(8);
	}
	STATE(4){	
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		AdvanceIn(8);
	}
	STATE(5){
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_3");
		Attack(Attacker, t3rec, 1);

		if (BuiltPower){
			SwarmAttackWaves1 = true;
			Advance();
		}
		else{
			SetState(3);
		}
	}
	STATE(6){	//CONTINUE_2
		AdvanceIn(9);
	}
	STATE(7){
		Attacker = BuildObject("svtank_J", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		AdvanceIn(9);
	}
	STATE(8){
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		Attacker = BuildObject("svtank_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3fact, 1);
		if (BuiltFac){
			SwarmAttackWaves2 = true;
			Advance();
		}
		else{
			SetState(6);
		}
	}
	STATE(9){	//CONTINUE_3
		if (BuiltSBay && BuiltArmo){	//if the player builds an armory before the service bay, increase the skill of spawned enemies for some reason
			MakeItHarder = true;
			Advance();
		}
		else if (BuiltSBay){
			Advance();
		}
	}
	STATE(10){	//PART_2
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_1");
		Attack(Attacker, t3rec, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_2");
		Attack(Attacker, t3rec, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3GT1, 1);
		AdvanceIn(15);
	}
	STATE(11){
		Attacker = BuildObject("svwalk_J", 5, "swarmSpawn_2");
		Attack(Attacker, t3GT3, 1);
		Attacker = BuildObject("svwalk_L", 5, "swarmSpawn_3");
		Attack(Attacker, t3fact, 1);
		if (IsAround(t3rec)){
			SetState(10, 15);
		}
		else{
			UnAlly(5, 2);
			ClearObjectives();
			AddObjective(EDF, RED);
			SwarmAttackWaves1 = false;
			SwarmAttackWaves2 = false;
			t3Recycler = BuildObject("ivrecy", 3, "t3Recycler");
			Goto(t3Recycler, "t3rec_path1", 1);
			SetObjectiveOn(t3Recycler);
			CheckT3Rec = true;
			Advance();
		}
	}
	STATE(12){	//T3REC_AT_PRBASE
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_1");
		Attack(Attacker, t3Recycler, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_2");
		Attack(Attacker, t3Recycler, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3Recycler, 1);
		Attacker = BuildObject("svtank_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3Recycler, 1);
		AdvanceIn(55);
	}
	STATE(13){
		if (GetCurrentCommand(t3Recycler) == CMD_NONE){
			AdvanceIn(45);
		}
		else{
			SetState(12);
		}
	}
	STATE(14){
		Goto(t3Recycler, "t3rec_path2", 1);
		Advance();
	}
	STATE(15){	//T3REC_AT_BASE
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_1");
		Attack(Attacker, t3Recycler, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_2");
		Attack(Attacker, t3Recycler, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3Recycler, 1);
		Attacker = BuildObject("svtank_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3Recycler, 1);
		AdvanceIn(55);
	}
	STATE(16){
		if (GetDistance(t3Recycler, "t3rec") < 30){
			Advance();
		}
		else{
			SetState(15);
		}
	}
	STATE(17){
		SwarmRec = BuildObject("Sbrecy00", 5, "SwarmRec");
		SetPlan("FS07_s2.aip", 5);
		Ally(2, 5);
		Ally(3, 5);
		//patrol_1
		for (int i = 0; i < 4; i++){
			Attacker = BuildObject("svtank_J2", 5, "Sforce_1");
			Patrol(Attacker, "patrol_1", 1);
		}
		for (int i = 0; i < 2; i++){
			Attacker = BuildObject("svinst_J2", 5, "Sforce_1");
			Patrol(Attacker, "patrol_1", 1);
		}
		for (int i = 0; i < 2; i++){
			Attacker = BuildObject("svwalk_J2", 5, "Sforce_1");
			Patrol(Attacker, "patrol_1", 1);
		}
		//patrol_2
		for (int i = 0; i < 3; i++){
			Attacker = BuildObject("svtank_J2", 5, "Sforce_2");
			Patrol(Attacker, "patrol_2", 1);
		}
		for (int i = 0; i < 3; i++){
			Attacker = BuildObject("svinst_J2", 5, "Sforce_2");
			Patrol(Attacker, "patrol_2", 1);
		}
		Swalk1 = BuildObject("svwalk_J2", 5, "Sforce_2");
		Patrol(Swalk1, "patrol_2", 1);
		Swalk2 = BuildObject("svwalk_J2", 5, "Sforce_2");
		Patrol(Swalk2, "patrol_2", 1);
		//patrol_3
		for (int i = 0; i < 3; i++){
			Attacker = BuildObject("svtank_J2", 5, "Sforce_1");
			Patrol(Attacker, "patrol_3", 1);
		}
		for (int i = 0; i < 3; i++){
			Attacker = BuildObject("svinst_J2", 5, "Sforce_2");
			Patrol(Attacker, "patrol_3", 1);
		}
		for (int i = 0; i < 2; i++){
			Attacker = BuildObject("svwalk_J2", 5, "Sforce_1");
			Patrol(Attacker, "patrol_3", 1);
		}

		SetAnimation(t3Recycler, "deploy", 1);
		AdvanceIn(2);
	}
	STATE(18){
		StartAnimation(t3Recycler);
		AdvanceIn(10);
	}
	STATE(19){
		Replace(t3Recycler, "ibrecy");
		SetPlan("FS07_e1.aip", 3);
		ClearObjectives();
		AddObjective(NewOrders, GREEN);
		AdvanceIn(60);
	}
	STATE(20){
		swarm_nav = BuildObject("ibnav", 1, "swarm_pos");
		SetObjectiveName(swarm_nav, "Swarm Force");
		SetObjectiveOn(swarm_nav);
		ClearObjectives();
		AddObjective(Orders2, WHITE);
		Advance();
	}
	STATE(21){	//CHECK_SWARM_FORCE
		if (!IsAround(Sbuild) && 
			!IsAround(Swalk1) && 
			!IsAround(Swalk2) && 
			!IsAround(SwarmRec)){
			ClearObjectives();
			AddObjective(SwarmDead, GREEN);
			Advance();
		}
	}
	STATE(22){	//SEARCH_RUINS
		if (GetDistance(Player, Probe) < 350){
			CameraReady();
			Advance();
		}
		else{
			SetState(22, 10);
		}
	}
	STATE(23){
		if (CameraPath("camera_1", 5800, 6000, Probe) || CameraCancelled()){
			CameraFinish();
			AdvanceIn(10);
		}
	}
	STATE(24){
		ClearObjectives();
		AddObjective(ProbeMsg, GREEN);
		SetTeamNum(Probe, 1);
		Mlead1 = BuildObject("OvCruise", 6, "m_sp1");
		Goto(Mlead1, Probe, 1);
		Mwing1 = BuildObject("OvBeam", 6, "m_sp1");
		Follow(Mwing1, Mlead1, 1);
		Mwing2 = BuildObject("OvBeam", 6, "m_sp1");
		Follow(Mwing2, Mlead1, 1);

		Mlead2 = BuildObject("OvCruise", 6, "m_sp2");
		Goto(Mlead1, Probe, 1);
		Mwing3 = BuildObject("OvBeam", 6, "m_sp2");
		Follow(Mwing3, Mlead2, 1);
		Mwing4 = BuildObject("OvBeam", 6, "m_sp2");
		Follow(Mwing4, Mlead2, 1);
		AdvanceIn(20);
	}
	STATE(25){
		Ally(5, 2);
		Ally(6, 2);
		Backup1 = BuildObject("pvrckt2", 2, "b_sp");
		Goto(Backup1, "probe_path", 1);
		Backup = BuildObject("pvtank", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvtank", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvtank", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvtank", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvtank", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvtank", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvscout", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvscout", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvscout", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvrckt", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvrckt", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvmisl", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvmisl", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvmisl", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		Backup = BuildObject("pvmisl", 2, "b_sp");
		Follow(Backup, Backup1, 1);
		CheckBackup = true;
		SetObjectiveOn(Backup1);
		SetObjectiveName(Backup1, "Reinforcements");
		Advance();
	}
	STATE(26){	//BACKUP
		Mlead1 = BuildObject("OvCruise", 6, "m_sp1");
		Goto(Mlead1, Probe, 1);
		Mwing1 = BuildObject("OvBeam", 6, "m_sp1");
		Follow(Mwing1, Mlead1, 1);
		Mwing2 = BuildObject("OvMisl", 6, "m_sp1");
		Follow(Mwing2, Mlead1, 1);
		AdvanceIn(45);
	}
	STATE(27){
		Mlead2 = BuildObject("OvCruise", 6, "m_sp1");
		Goto(Mlead2, Probe, 1);
		Mwing3 = BuildObject("OvBeam", 6, "m_sp1");
		Follow(Mwing3, Mlead2, 1);
		Mwing4 = BuildObject("Ovwalk", 6, "m_sp1");
		Attack(Mwing4, Probe, 1);
		AdvanceIn(45);
	}
	STATE(28){
		if (GetDistance(Backup1, Probe) < 950){
			if (GetDistance(Backup1, Probe) < 350){
				SucceedMission(GetTime() + 10, "FS07win.des");
				Advance();
			}
		}
		else{
			SetState(26);
		}
	}
	
	CheckStuffIsAlive();

	if (SwarmAttackWaves1){
		SpawnSwarmAttackWaves1();
	}
	if (SwarmAttackWaves2){
		SpawnSwarmAttackWaves2();
	}	
}

void fs07Mission::SpawnSwarmAttackWaves1(void){
	if (GetTime() < SwarmAttackTimer1)
		return;

	switch (SwarmAttackState1){
	case 0:
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		SwarmAttackTimer1 = GetTime() + 15;
		SwarmAttackState1++;
		break;
	case 1:
		Attacker = BuildObject("svscL_D", 5, "swarmSpawn_3");
		Attack(Attacker, t3GT1, 1);
		SwarmAttackTimer1 = GetTime() + 15;
		SwarmAttackState1++;
		break;
	case 2:
		Attacker = BuildObject("svartl_J", 5, "swarmSpawn_1");
		Attack(Attacker, PlayerPow, 1);
		SwarmAttackTimer1 = GetTime() + 15;
		SwarmAttackState1 = 0;
		break;
	}
}

void fs07Mission::SpawnSwarmAttackWaves2(void){
	if (GetTime() < SwarmAttackTimer2)
		return;

	switch (SwarmAttackState2){
	case 0:
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_1");
		Attack(Attacker, Recycler, 1);
		Attacker = BuildObject("svinst_J", 5, "swarmSpawn_3");
		Attack(Attacker, t3fact, 1);
		SwarmAttackTimer2 = GetTime() + 15;
		SwarmAttackState2++;
		break;
	case 1:
		Attacker = BuildObject("svwalk_J", 5, "swarmSpawn_1");
		Attack(Attacker, PlayerFac, 1);
		SwarmAttackTimer2 = GetTime() + 15;
		SwarmAttackState2 = 0;
		break;
	}
}

void fs07Mission::CheckStuffIsAlive(void){
	if (!IsAround(PRoverlord) && !MissionFailed){
		FailMission(GetTime() + 10, "FS07fail2.des");
		MissionFailed = true;
	}
	if (!IsAround(Recycler) && !MissionFailed){
		FailMission(GetTime() + 10, "FS07fail3.des");
		MissionFailed = true;
	}
	if (CheckT3Rec && !IsAround(t3Recycler) && !MissionFailed){
		FailMission(GetTime() + 10, "FS07fail4.des");
		MissionFailed = true;
	}
	if (!IsAround(Probe) && !MissionFailed){
		FailMission(GetTime() + 10, "FS07fail5.des");
		MissionFailed = true;
	}
}

//to replace the edf recycler unit with the building version
void fs07Mission::Replace(Handle &h, char* odf){
	if (IsAround(h)){
		Vector oldPos = GetPosition(h);
		oldPos.y = TerrainFindFloor(oldPos.x, oldPos.z);	//otherwise the buildings spawn in the air
		int teamNum = GetTeamNum(h);
		RemoveObject(h);
		h = BuildObject(odf, teamNum, oldPos);
	}
}