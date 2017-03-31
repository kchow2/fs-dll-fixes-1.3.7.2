#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs03Mission : public SPMission 
{
public:
	fs03Mission(void)
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
	void SpawnWaves(void);
	void CheckRecy(void);

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
		WaveTimer,
		f_last;

	// handles
	Handle
		h_first,
		Recycler,
		Player,
		EnemyRec1,
		EnemyRec2,
		Tech,
		Turr1,
		Turr2,
		Turr3,
		Turr4,
		Turr5,
		Turr6,
		Rckt1,
		Rckt2,
		Rckt3,
		Rckt4,
		Walk1,
		Walk2,
		Walk3,
		Sct1,
		Sct2,
		Sct3,
		Tnk1,
		Tnk2,
		Attacker,
		tow1,
		tow2,
		tow3,
		tow4,
		def1,
		def2,
		def3,
		def4,
		def5,
		Swalk1,
		Swalk2,
		Swalk3,
		Swalk4,
		star_nav,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		AttackerCount,
		AttackWaveState,
		i_last;

	char
		*startxt = "With Lt. Harker dead,\nyou are now in charge\nof the recycler. However,\nwe need to upgrade it first.\nGet those scavs deployed\nwhile you're waiting.",
		*techtxt = "OK the recycler\nhas now been upgraded.\nGet it deployed and\nbegin the assault on\nthe enemy installation\nto the north-west.",
		*rec1txt = "Excellent work. The enemy\ninstallation has been neutralized and\nour forces are now able to\nmove out of the canyon.\nNow let's retake that StarPort.",
		*allies = "Commander, you'd better get some\ndefences up here. We've\ngot a large enemy force\nfurther up the canyon, and\nthey're closing fast.",
		*WinText = "Well done. You have\ncleared the way\nto the Starport.",
		*lastorders = "You have eliminated all enemy\nbases in this area. Destroy the\nenemy force blocking the way\nto the starport, and\nany extra units that arrive.";
};
DLLBase * BuildMission(void)
{
	return new fs03Mission();
}

void fs03Mission::Setup(void)
{
//  bools
	MissionFailed = false;
//  integers
	mission_state = 0;

	PreloadODF("sbrecy_b");
}

void fs03Mission::AddObject(Handle h)
{

}

void fs03Mission::Execute(void)
{
	Player = GetPlayerHandle();
	MISSION_START{
		Tech = GetHandle("Tech");
		Recycler = GetHandle("Recycler");
		Ally(5, 6);
		Ally(1, 2);
		SetScrap(1, 40);
		SetScrap(5, 20);
		SetScrap(6, 20);
		Turr1 = BuildObject("svturr", 6, "turr1");
		Turr2 = BuildObject("svturr", 6, "turr2");
		Turr3 = BuildObject("svturr", 6, "turr3");
		Turr4 = BuildObject("svturr", 6, "turr4");
		Turr5 = BuildObject("svturr", 6, "turr5");
		Turr6 = BuildObject("svturr", 6, "turr6");
		def1 = BuildObject("svturr", 6, "def1");
		def2 = BuildObject("svturr", 6, "def2");
		def3 = BuildObject("svturr", 6, "def3");
		def4 = BuildObject("svturr", 6, "def4");
		def5 = BuildObject("svturr", 6, "def5");
		tow1 = BuildObject("sbgtow", 5, "tow1");
		tow2 = BuildObject("sbgtow", 5, "tow2");
		tow3 = BuildObject("sbgtow", 5, "tow3");
		tow4 = BuildObject("sbgtow", 5, "tow4");
		ClearObjectives();
		AddObjective(startxt, WHITE);
		Goto(Recycler, "Rec_path1", 1);
		EnemyRec1 = BuildObject("sbrecy_b", 5, "RecyclerEnemy");
		EnemyRec2 = BuildObject("sbrecy_b", 6, "RecyclerEnemy2");
		AdvanceIn(30);
	}
	STATE(1){
		SetPlan("FS03.aip", 5);
		SetPlan("FS03b.aip", 6);
		Advance();
	}
	STATE(2){	//AT_TECH
		if (GetDistance(Recycler, Tech) < 50){
			AdvanceIn(40);
		}
	}
	STATE(3){
		Goto(Recycler, "Rec_path1", 0);
		ClearObjectives();
		AddObjective(techtxt, WHITE);
		Advance();
	}
	STATE(4){	//CHECK_ENEMY_REC
		if (!IsAround(EnemyRec2) && !MissionFailed){
			FailMission(GetTime() + 10, "FS03fail2.des");	//you need to destroy the NW recy before the NE one.
			MissionFailed = true;
		}
		else if (!IsAround(EnemyRec1)){
			ClearObjectives();
			AddObjective(rec1txt, GREEN);
			AdvanceIn(10);
		}
	}
	STATE(5){
		Rckt1 = BuildObject("ivrckt_BD", 2, "spawn_1");
		Rckt2 = BuildObject("ivrckt_BD", 2, "spawn_1");
		Rckt3 = BuildObject("ivrckt_BD", 2, "spawn_1");
		Rckt4 = BuildObject("ivrckt_BD", 2, "spawn_1");
		Goto(Rckt1, "attack_1", 1);
		Goto(Rckt2, "attack_1", 1);
		Goto(Rckt3, "attack_1", 1);
		Goto(Rckt4, "attack_1", 1);
		AdvanceIn(20);
	}
	STATE(6){
		ClearObjectives();
		AddObjective(allies, RGB(0, 0, 255));
		Walk1 = BuildObject("ivwalk_BD", 2, "spawn_1");
		Walk2 = BuildObject("ivwalk_BD", 2, "spawn_1");
		Walk3 = BuildObject("ivwalk_BD", 2, "spawn_1");
		Goto(Walk1, "attack_2", 1);
		Goto(Walk2, "attack_2", 1);
		Goto(Walk3, "attack_2", 1);
		AdvanceIn(15);
	}
	STATE(7){
		Sct1 = BuildObject("ivsct_BD", 2, "spawn_1");
		Sct2 = BuildObject("ivsct_BD", 2, "spawn_1");
		Sct3 = BuildObject("ivsct_BD", 2, "spawn_1");
		Goto(Sct1, "attack_1", 1);
		Goto(Sct2, "attack_1", 1); 
		Goto(Sct3, "attack_1", 1);
		AdvanceIn(10);
	}
	STATE(8){
		Tnk1 = BuildObject("ivtnk_BD", 2, "spawn_1");
		Tnk2 = BuildObject("ivtnk_BD", 2, "spawn_1");
		Goto(Tnk1, "attack_2", 1);
		Goto(Tnk2, "attack_2", 1);
		AdvanceIn(30);
	}
	STATE(9){
		Attacker = BuildObject("svscA_D", 5, "spawn_1");
		Goto(Attacker, Rckt1, 0);
		Attacker = BuildObject("svBrood_L", 5, "spawn_1");
		Goto(Attacker, Walk1, 0);
		AdvanceIn(5);
	}
	STATE(10){
		Attacker = BuildObject("svtank_L", 5, "spawn_1");
		Goto(Attacker, Rckt1, 0);
		Attacker = BuildObject("svtank_L", 5, "spawn_1");
		Goto(Attacker, Walk1, 0);
		AdvanceIn(10);
	}
	STATE(11){
		Attacker = BuildObject("svscA_D", 5, "spawn_1");
		Goto(Attacker, Rckt1, 0);
		Attacker = BuildObject("svscA_D", 5, "spawn_1");
		Goto(Attacker, Walk1, 0);
		AdvanceIn(15);
	}
	STATE(12){
		Attacker = BuildObject("svtank_L", 5, "spawn_1");
		Goto(Attacker, Rckt1, 0);
		Attacker = BuildObject("svtank_L", 5, "spawn_1");
		Goto(Attacker, Walk1, 0);
		Advance();
	}
	STATE(13){	//CHECK_ENEMY_REC2
		SpawnWaves();
		if (!IsAround(EnemyRec2)){
			AdvanceIn(10);
		}
		else{
			SetState(13, 20);
		}	
	}
	STATE(14){
		ClearObjectives();
		AddObjective(lastorders, GREEN);
		star_nav = BuildObject("ibnav", 1, "star_nav");
		SetObjectiveName(star_nav, "StarPort");
		SetObjectiveOn(star_nav);
		Goto(Rckt1, "star_nav", 1);
		Goto(Rckt2, "star_nav", 1);
		Goto(Rckt3, "star_nav", 1);
		Goto(Rckt4, "star_nav", 1);
		Goto(Walk1, "star_nav", 1);
		Goto(Walk2, "star_nav", 1);
		Goto(Walk3, "star_nav", 1);
		Goto(Sct1, "star_nav", 1);
		Goto(Sct2, "star_nav", 1);
		Goto(Sct3, "star_nav", 1);
		Goto(Tnk1, "star_nav", 1);
		Goto(Tnk2, "star_nav", 1);

		Swalk1 = BuildObject("Svwalk_J2", 5, "finalforce1");
		Swalk2 = BuildObject("Svwalk_L2", 5, "finalforce1");
		Attacker = BuildObject("Svinst_L2", 5, "finalforce1");
		Patrol(Attacker, "patrol_1", 1);
		Attacker = BuildObject("Svartl_L2", 5, "finalforce1");
		Patrol(Attacker, "patrol_1", 1);
		Attacker = BuildObject("Svinst_L2", 5, "finalforce1");
		Patrol(Attacker, "patrol_1", 1);
		Attacker = BuildObject("Svinst_L2", 5, "finalforce1");
		Patrol(Attacker, "patrol_1", 1);
		Attacker = BuildObject("Svinst_L2", 5, "finalforce1");
		Patrol(Attacker, "patrol_1", 1);

		Swalk3 = BuildObject("Svwalk_J2", 5, "finalforce2");
		Swalk4 = BuildObject("Svwalk_L2", 5, "finalforce2");
		Attacker = BuildObject("Svinst_L2", 5, "finalforce2");
		Patrol(Attacker, "patrol_2", 1);
		Attacker = BuildObject("Svartl_L2", 5, "finalforce2");
		Patrol(Attacker, "patrol_2", 1);
		Attacker = BuildObject("Svartl_L2", 5, "finalforce2");
		Patrol(Attacker, "patrol_2", 1);
		Attacker = BuildObject("Svartl_L2", 5, "finalforce2");
		Patrol(Attacker, "patrol_2", 1);
		Advance();
	}
	STATE(15){	//WALK_1
		AdvanceIn(60);
	}
	STATE(16){
		Attacker = BuildObject("svBrood_L", 5, "last_attackers");
		Goto(Attacker, Recycler, 0);
		AdvanceIn(20);
	}
	STATE(17){
		Attacker = BuildObject("svscA_D", 5, "last_attackers");
		Goto(Attacker, Recycler, 0);
		AdvanceIn(20);
	}
	STATE(18){
		if (!IsAround(Swalk1)){
			Advance();
		}
		else{
			SetState(15);
		}
	}
	STATE(19){	//WALK_2
		AdvanceIn(20);
	}
	STATE(20){
		Attacker = BuildObject("svscA_D", 5, "last_attackers");
		Goto(Attacker, Recycler, 0);
		if (!IsAround(Swalk2)){
			Advance();
		}
		else{
			SetState(19);
		}
	}
	STATE(21){	//WALK_3
		AdvanceIn(20);
	}
	STATE(22){
		Attacker = BuildObject("svBrood_L", 5, "last_attackers");
		Goto(Attacker, Recycler, 0);
		if (!IsAround(Swalk3)){
			Advance();
		}
		else{
			SetState(21);
		}
	}
	STATE(23){	//WALK_4
		AdvanceIn(20);
	}
	STATE(24){
		Attacker = BuildObject("svscA_D", 5, "last_attackers");
		Goto(Attacker, Recycler, 0);
		if (!IsAround(Swalk4)){
			AdvanceIn(20);
		}
		else{
			SetState(23);
		}
	}
	STATE(25){
		ClearObjectives();
		AddObjective(WinText, GREEN);
		SucceedMission(GetTime() + 16, "FS03win.des");
		Advance();
	}
}

void fs03Mission::SpawnWaves(void){
	if (AttackWaveState == 0){
		WaveTimer = GetTime() + 20;
		AttackerCount++;
		if (AttackerCount > 15){
			AttackWaveState = 99;
		}
		AttackWaveState++;
	}
	else if (AttackWaveState == 1 && GetTime() > WaveTimer){
		Attacker = BuildObject("svscA_D", 5, "spawn_1");
		Goto(Attacker, "attack_1", 1);
		WaveTimer = GetTime() + 5;
		AttackWaveState++;
	}
	else if (AttackWaveState == 2 && GetTime() > WaveTimer){
		Attacker = BuildObject("svBrood_L", 5, "spawn_1");
		Goto(Attacker, "attack_1", 1);
		WaveTimer = GetTime() + 5;
		AttackWaveState++;
	}
	else if (AttackWaveState == 3 && GetTime() > WaveTimer){
		Attacker = BuildObject("svtank_L", 5, "spawn_1");
		Goto(Attacker, "attack_1", 1);
		WaveTimer = GetTime() + 5;
		AttackWaveState++;
	}
	else if (AttackWaveState == 4 && GetTime() > WaveTimer){
		Attacker = BuildObject("svBrood_L", 5, "spawn_1");
		Goto(Attacker, Recycler, 0);
		WaveTimer = GetTime() + 5;
		AttackWaveState++;
	}
	else if (AttackWaveState == 5 && GetTime() > WaveTimer){
		Attacker = BuildObject("svtank_L", 5, "spawn_1");
		Goto(Attacker, Recycler, 0);
		AttackWaveState = 0;
	}
}

void fs03Mission::CheckRecy(void){
	if (!IsAround(Recycler) && !MissionFailed){
		FailMission(GetTime() + 10, "FS03fail.des");
	}
}