#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs06Mission : public SPMission 
{
public:
	fs06Mission(void)
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
	void Replace(Handle &h, char* odf);
	void DoSpawnReinforcements();

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
		MissionSucceeded,
		CheckOverlord,
		CheckProc,
		CheckSwarm,
		PhaerRhanAI,
		PrecBuilt,
		PfactBuilt,
		ParmoBuilt,
		SpawnReinforcements,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		PhaerRhanTimer,
		ReinforcementTimer,
		f_last;

	// handles
	Handle
		h_first,
		SwarmGT1,
		SwarmGT2,
		SwarmGT3,
		SwarmGT4,
		SwarmGT5,
		PRDrop1,
		PRDrop2,
		PRDrop3,
		PRDrop4,
		player,
		LZ_nav,
		SwarmAA_1,
		SwarmAA_2,
		SwarmAA_3,
		SwarmAA_4,
		SwarmAA_5,
		SwarmAA_6,
		Crash_nav,
		PRConst,
		PRProc,
		PRescort1,
		PRescort2,
		PRescort3,
		PRGtow1,
		PRPgen1,
		Attacker,
		Prec,
		Pfact,
		SwarmRec,
		Parmo,
		Friend,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		PhaerRhanAIState,
		ReinforcementCount,
		i_last;

	char
		*Orders = "Commander take your team to the\nlanding zone and clear out the\nswarm infestation, secure the\narea before the Phaer-Rhan\ndropships arrive.",
		*Dropships = "Well done commander the LZ is\nsecure, the Phaer-Rhan will\narrive shortly.",
		*Canyon = "Dammit, the swarm have gotten\nAA units along the canyon, they\nare tearing the dropships to\npieces, take out those swarm units\nimmediately.",
		*Crashsite = "well done commander, we are\nrecieving a signal from a crashed\ndropship, go to the crashsite\nand escort the survivors to\nthe landing zone.",
		*Crashsite2 = "Escort that Phaer-Rhan Overlord\nto the landing zone ,this\nunit is vital , another essential\nunit , the processor has also\njust arrived at the LZ.",
		*LandingZone = "Well done , now help defend\nthe area while the Phaer_Rhan\nsetup base , make sure that\nprocessor is well protected .",
		*BaseUp = "Excellent work, the Phaer Rhan\nnow have a factory built\nassist them in their attack on the\nSwarm base to the north.";
};
DLLBase * BuildMission(void)
{
	return new fs06Mission();
}

void fs06Mission::Setup(void)
{
//  bools
	MissionFailed = false;
	MissionSucceeded = false;
	CheckOverlord = false;
	CheckProc = false;
	CheckSwarm = false;
	PhaerRhanAI = false;
	PrecBuilt = false;
	PfactBuilt = false;
	ParmoBuilt = false;
	SpawnReinforcements = false;

	//floats
	ReinforcementTimer = 0;

//  integers
	mission_state = 0;
	PhaerRhanAIState = 0;
	ReinforcementCount = 0;

	//Preloading helps to reduce the lag spike when these things are spawned in for the first time
	PreloadODF("pvcons");	
}

void fs06Mission::AddObject(Handle h)
{
	char ODFName[64];
	GetObjInfo(h, Get_CFG, ODFName);
	
	if (!PrecBuilt && IsOdf2(h, "pbrecy")){
		Prec = h;
		PrecBuilt = true;
	}
	else if (!PfactBuilt && IsOdf2(h, "pbfact")){
		Pfact = h;
		PfactBuilt = true;
	}
	else if (ParmoBuilt && IsOdf2(h, "pbarmo")){
		Parmo = h;
		ParmoBuilt = true;
	}
}

void fs06Mission::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START{
		Ally(1, 2);
		SwarmGT1 = GetHandle("SwarmGT1");
		SwarmGT2 = GetHandle("SwarmGT2");
		SwarmGT3 = GetHandle("SwarmGT3");
		SwarmGT4 = GetHandle("SwarmGT4");
		SwarmGT5 = GetHandle("SwarmGT5");
		SwarmAA_1 = GetHandle("SwarmAA_1");
		SwarmAA_2 = GetHandle("SwarmAA_2");
		SwarmAA_3 = GetHandle("SwarmAA_3");
		SwarmAA_4 = GetHandle("SwarmAA_4");
		SwarmAA_5 = GetHandle("SwarmAA_5");
		SwarmAA_6 = GetHandle("SwarmAA_6");
		SwarmRec = GetHandle("SwarmRec");
		LZ_nav = BuildObject("ibnav", 1, "LZ");
		SetObjectiveName(LZ_nav, "Landing Zone");
		SetObjectiveOn(LZ_nav);
		ClearObjectives();
		AddObjective(Orders, WHITE);
		SetPlan("FS06_s1.aip", 6);
		Advance();
	}
	STATE(1){
		Attacker = BuildObject("svscA_D", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(2){
		Attacker = BuildObject("svscA_D", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(3){
		Attacker = BuildObject("svwalk_j", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(4){
		Attacker = BuildObject("svwalk_j", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(5){
		Attacker = BuildObject("svinst_J", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(6){
		Attacker = BuildObject("svinst_J", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(7){
		Attacker = BuildObject("svscA_D", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(8){
		Attacker = BuildObject("svscA_D", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(9){
		Attacker = BuildObject("svtank_J", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(10){
		Attacker = BuildObject("svtank_J", 6, "LZ");
		Goto(Attacker, "attack_path", 1);
		AdvanceIn(2);
	}
	STATE(11){	//GT_ALIVE
		if (!IsAround(SwarmGT1) &&
			!IsAround(SwarmGT2) &&
			!IsAround(SwarmGT3) &&
			!IsAround(SwarmGT4) &&
			!IsAround(SwarmGT5)){
			AdvanceIn(10);
		}
	}
	STATE(12){
		ClearObjectives();
		AddObjective(Dropships, GREEN);
		PRDrop1 = BuildObject("PvDrop_01", 2, "drop_spawn");
		Goto(PRDrop1, "drop_path", 1);
		AdvanceIn(25);
	}
	STATE(13){
		Ally(1, 6);
		CameraReady();
		Advance();
	}
	STATE(14){
		if (CameraPath("camera_1", 6000, 2500, PRDrop1) || CameraCancelled()){
			CameraFinish();
			UnAlly(1, 6);
			PRDrop2 = BuildObject("PvDrop_01b", 2, "drop_spawn");
			Goto(PRDrop2, "drop_path", 1);
			AdvanceIn(5);
		}
	}
	STATE(15){
		PRDrop3 = BuildObject("PvDrop_01b", 2, "drop_spawn");
		Goto(PRDrop3, "drop_path", 1);
		AdvanceIn(45);
	}
	STATE(16){	//AA1_ALIVE
		ClearObjectives();
		AddObjective(Canyon, RED);
		Advance();
	}
	STATE(17){
		if (!IsAround(SwarmAA_1) &&
			!IsAround(SwarmAA_2) &&
			!IsAround(SwarmAA_3) &&
			!IsAround(SwarmAA_4) &&
			!IsAround(SwarmAA_5) &&
			!IsAround(SwarmAA_6)){
			AdvanceIn(10);
		}
	}
	STATE(18){
		Crash_nav = BuildObject("ibnav", 1, "crashsite");
		SetObjectiveName(Crash_nav, "Crashsite");
		SetObjectiveOn(Crash_nav);
		ClearObjectives();
		AddObjective(Crashsite, WHITE);
		AdvanceIn(5);
	}
	STATE(19){
		PRConst = BuildObject("pvcons", 2, "crashsite");
		AdvanceIn(5);
	}
	STATE(20){
		PRescort1 = BuildObject("pvscout", 2, "crashsite");
		AdvanceIn(5);
	}
	STATE(21){
		PRescort2 = BuildObject("pvscout", 2, "crashsite");
		AdvanceIn(5);
	}
	STATE(22){
		PRescort3 = BuildObject("pvscout", 2, "crashsite");
		SetObjectiveOn(PRConst);
		SetPlan("FS06_s2.aip", 6);
		SetScrap(6, 40);
		CheckOverlord = true;
		Advance();
	}
	STATE(23){
		if (GetDistance(player, PRConst) < 80){
			ClearObjectives();
			AddObjective(Crashsite2, WHITE);
			AdvanceIn(5);
		}
	}
	STATE(24){
		Goto(PRConst, "LZ", 1);
		AdvanceIn(3);
	}
	STATE(25){
		Follow(PRescort1, PRConst, 1);
		AdvanceIn(3);
	}
	STATE(26){
		Follow(PRescort2, PRConst, 1);
		AdvanceIn(3);
	}
	STATE(27){
		Follow(PRescort3, PRConst, 1);
		AdvanceIn(3);
	}
	STATE(28){
		PRDrop4 = BuildObject("PvDrop_03", 2, "drop");
		AdvanceIn(5);
	}
	STATE(29){
		PRProc = BuildObject("PvProc", 2, "proc");
		CheckProc = true;
		SetObjectiveOn(PRProc);
		AdvanceIn(5);
	}
	STATE(30){
		PRPgen1 = BuildObject("PvPgen", 2, "dropoff1");
		Goto(PRPgen1, "Pow1", 1);
		AdvanceIn(5);
	}
	STATE(31){
		PRGtow1 = BuildObject("PvGtow", 2, "dropoff2");
		AdvanceIn(5);
	}
	STATE(32){	//DEPLOY_POWER
		if (GetDistance(PRPgen1, "Pow1") < 5){
			Advance();
		}
	}
	STATE(33){
		SetAnimation(PRPgen1, "deploy", 1);
		AdvanceIn(2);
	}
	STATE(34){
		StartAnimation(PRPgen1);
		AdvanceIn(10);
	}
	STATE(35){
		Replace(PRPgen1, "pbpgen");
		Goto(PRGtow1, "GT1", 1);
		AdvanceIn(5);
	}
	STATE(36){	//DEPLOY_GT
		if (GetDistance(PRGtow1, "GT1") < 5){
			SetAnimation(PRGtow1, "deploy", 1);
			AdvanceIn(2);
		}
	}
	STATE(37){
		StartAnimation(PRGtow1);
		AdvanceIn(10);
	}
	STATE(38){
		Replace(PRGtow1, "pbgtow");
		Advance();
	}
	STATE(39){	//AT_LZ
		if (GetDistance(PRConst, LZ_nav) < 80){
			ClearObjectives();
			AddObjective(LandingZone, WHITE);
			AdvanceIn(10);
		}
		else{
			SetState(39, 15);
		}
	}
	STATE(40){
		Goto(PRProc, "Processor", 1);
		SetPlan("FS06_s3.aip", 6);
		Advance();
	}
	STATE(41){	//AT_DEPLOY
		if (GetDistance(PRProc, "Processor") < 8){
			SetAnimation(PRProc, "deploy", 1);
			AdvanceIn(5);
		}
	}
	STATE(42){
		StartAnimation(PRProc);
		AdvanceIn(10);
	}
	STATE(43){
		Replace(PRProc, "pbproc");
		SetScrap(2, 40);
		SetPlan("FS06_p1.aip", 2);
		PhaerRhanAI = true;
		CheckSwarm = true;
		if (IsAround(PRescort1)){
			Patrol(PRescort1, "Patrol_1", 1);
		}
		if (IsAround(PRescort2)){
			Patrol(PRescort2, "Patrol_1", 1);
		}
		if (IsAround(PRescort3)){
			Patrol(PRescort3, "Patrol_1", 1);
		}
		AdvanceIn(5);
	}
	STATE(44){	//PR_MAIN
		if (PrecBuilt && !IsAround(Prec)){
			PrecBuilt = false;
			Prec = NULL;
		}
		if (PfactBuilt && !IsAround(Pfact)){
			PfactBuilt = false;
			Pfact = NULL;
		}
		if (ParmoBuilt && !IsAround(Parmo)){
			ParmoBuilt = false;
			Parmo = NULL;
		}
		
		if (!PrecBuilt){
			Build(PRConst, "pbrecy", 1);
			SetState(45);
		}
		else if (!PfactBuilt){
			Build(PRConst, "pbfact", 1);
			SetState(46);
		}
		else if (!ParmoBuilt){
			Build(PRConst, "pbarmo", 1);
			SetState(47);
		}
		else{
			SetState(44, 5);
		}

		if (PfactBuilt && !SpawnReinforcements){
			ClearObjectives();
			AddObjective(BaseUp, WHITE);
			SpawnReinforcements = true;
		}
	}
	STATE(45){	//BUILD_CYARD
		Dropoff(PRConst, "PRcyard", 1);
		SetState(48, 5);
	}
	STATE(46){	//BUILD_FACT
		Dropoff(PRConst, "PRfact", 1);
		SetState(48, 5);
	}
	STATE(47){	//BUILD_ARMO
		Dropoff(PRConst, "PRArm", 1);
		SetState(48, 5);
	}
	STATE(48){
		if (GetCurrentCommand(PRConst) == CMD_NONE){
			SetState(44);
		}
	}

	CheckStuffIsAlive();
	if (SpawnReinforcements){
		DoSpawnReinforcements();
	}
}

void fs06Mission::CheckStuffIsAlive(void){
	if (CheckOverlord && !IsAround(PRConst) && !MissionFailed){
		FailMission(GetTime() + 10, "FS06fail1.des");
	}
	if (CheckProc && !IsAround(PRProc) && !MissionFailed){
		FailMission(GetTime() + 10, "FS06fail2.des");
		MissionFailed = true;
	}
	if (CheckSwarm && !IsAround(SwarmRec) && !MissionSucceeded){
		SucceedMission(GetTime() + 10, "FS06win.des");
		MissionSucceeded = true;
	}
}

//replaces the mobile version of a PR building with the stationary one
void fs06Mission::Replace(Handle &h, char* odf){
	if (IsAround(h)){
		Vector oldPos = GetPosition(h);
		oldPos.y = TerrainFindFloor(oldPos.x, oldPos.z);	//otherwise the buildings spawn in the air
		int teamNum = GetTeamNum(h);
		RemoveObject(h);
		h = BuildObject(odf, teamNum, oldPos);
	}
}

void fs06Mission::DoSpawnReinforcements(){
	char* odfs[] = { 
		"ivwalk_BD", 
		"ivwalk_BD", 
		"ivwalk_BD", 
		"ivserv_BD", 
		"ivserv_BD", 
		"ivrdev_BD", 
		"ivrdev_BD", 
		"ivrdev_BD", 
		"ivtnk4_BD", 
		"ivtnk4_BD", 
		"ivtur_BD", 
		"ivtur_BD", 
		"ivtur_BD", 
		"ivtur_BD" };
	int group[] = {7,7,7,8,8,9,9,9,6,6,5,5,5,5};
	if (GetTime() > ReinforcementTimer && ReinforcementCount < SIZEOF(odfs)){
		Handle h = BuildObject(odfs[ReinforcementCount], 1, "Friends");
		SetGroup(h, group[ReinforcementCount]);
		Goto(h, "LZ", 0);
		ReinforcementCount++;
		ReinforcementTimer = GetTime() + 2;
	}
}