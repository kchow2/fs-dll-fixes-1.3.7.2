#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs09Mission : public SPMission 
{
public:
	fs09Mission(void)
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
	void SpawnReinforcements(void);

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
		AAGroup1Alive,
		AAGroup2Alive,
		AAGroup3Alive,
		AAGroup4Alive,
		Reinforcements1,
		Reinforcements2,
		Reinforcements3,
		Reinforcements4,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		Reinforcements1Time,
		Reinforcements2Time,
		Reinforcements3Time,
		Reinforcements4Time,
		f_last;

	// handles
	Handle
		h_first,
		player,
		drop,
		escort1,
		escort2,
		escort3,
		escort4,
		recy,
		attacker,
		swarmAA1,
		swarmAA2,
		swarmAA3,
		swarmAA4,
		swarmAA5,
		swarmAA6,
		swarmAA7,
		swarmAA8,
		swarmAA9,
		swarmAA10,
		reinforce,
		swarmrec,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		i_last;

	char
		*Orders1 = "Protect the Recycler convoy as\nit makes its way through the\njungle. The more swarm AA units\nyou destroy, the more dropships\nwe can send in.",
		*AA1Dead = "Well done, you've taken out a\ngroup of AA units. A Dropship\n with reinforcements is on it's\nway down to you.",
		*AtBase = "Congratulations, you made it\nthrough. Now establish a base\n and take out the swarm\ninstallation to the\nnorth west.";
};
DLLBase * BuildMission(void)
{
	return new fs09Mission();
}

void fs09Mission::Setup(void)
{
//  bools
	MissionFailed = false;
	AAGroup1Alive = true;
	AAGroup2Alive = true;
	AAGroup3Alive = true;
	AAGroup4Alive = true;
	Reinforcements1 = false;
	Reinforcements2 = false;
	Reinforcements3 = false;
	Reinforcements4 = false;

//  integers
	mission_state = 0;

	//should help get rid of lag spikes when things are spawned in later
	PreloadODF("sbrecy00");
}

void fs09Mission::AddObject(Handle h)
{

}

void fs09Mission::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START{
		recy = GetHandle("recy");
		escort1 = GetHandle("escort1");
		escort2 = GetHandle("escort2");
		escort3 = GetHandle("escort3");
		escort4 = GetHandle("escort4");
		swarmAA1 = GetHandle("swarmAA1");
		swarmAA2 = GetHandle("swarmAA2");
		swarmAA3 = GetHandle("swarmAA3");
		swarmAA4 = GetHandle("swarmAA4");
		swarmAA5 = GetHandle("swarmAA5");
		swarmAA6 = GetHandle("swarmAA6");
		swarmAA7 = GetHandle("swarmAA7");
		swarmAA8 = GetHandle("swarmAA8");
		swarmAA9 = GetHandle("swarmAA9");
		swarmAA10 = GetHandle("swarmAA10");
		SetScrap(1, 40);
		Goto(recy, "recpath_1", 1);
		SetObjectiveOn(recy);	//Added recy beacon on HUD
		AdvanceIn(2);
	}
	STATE(1){
		Follow(escort1, recy, 1);
		AdvanceIn(2);
	}
	STATE(2){
		Follow(escort2, recy, 1);
		AdvanceIn(2);
	}
	STATE(3){
		Follow(escort3, recy, 1);
		AdvanceIn(2);
	}
	STATE(4){
		Follow(escort4, recy, 1);
		ClearObjectives();
		AddObjective(Orders1, WHITE);
		attacker = BuildObject("svscA_D", 6, "swarm1");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(5){
		attacker = BuildObject("svscA_D", 6, "swarm1");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(6){
		attacker = BuildObject("svscout_L", 6, "swarm1");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(7){
		attacker = BuildObject("svscout_L", 6, "swarm1");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(8){
		attacker = BuildObject("svscout_L", 6, "swarm1");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(9){	//IN_TRANSIT
		attacker = BuildObject("svscout_L", 6, "swarm2");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(10){
		attacker = BuildObject("svscout_L", 6, "swarm2");
		Attack(attacker, recy, 1);
		AdvanceIn(50);
	}
	STATE(11){
		if (GetCurrentCommand(recy) == CMD_NONE){
			AdvanceIn(40);
		}
		else{
			SetState(9);
		}
	}
	STATE(12){
		Goto(recy, "recpath_2", 1);
		AdvanceIn(10);
	}
	STATE(13){	//IN_TRANSIT2
		attacker = BuildObject("svscout_L", 6, "swarm2");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(14){
		attacker = BuildObject("svscout_L", 6, "swarm2");
		Attack(attacker, recy, 1);
		AdvanceIn(50);
	}
	STATE(15){
		if (GetCurrentCommand(recy) == CMD_NONE){
			AdvanceIn(40);
		}
		else{
			SetState(13);
		}
	}
	STATE(16){
		Goto(recy, "recpath_3", 1);
		AdvanceIn(10);
	}
	STATE(17){	//IN_TRANSIT3
		attacker = BuildObject("svscout_J", 6, "swarm3");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(18){
		attacker = BuildObject("svtank_L", 6, "swarm3");
		Attack(attacker, recy, 1);
		AdvanceIn(50);
	}
	STATE(19){
		if (GetCurrentCommand(recy) == CMD_NONE){
			AdvanceIn(40);
		}
		else{
			SetState(17);
		}
	}
	STATE(20){
		Goto(recy, "recpath_4", 1);
		AdvanceIn(10);
	}
	STATE(21){	//IN_TRANSIT4
		attacker = BuildObject("svscout_J", 6, "swarm4");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(22){
		attacker = BuildObject("svtank_L", 6, "swarm4");
		Attack(attacker, recy, 1);
		AdvanceIn(2);
	}
	STATE(23){
		attacker = BuildObject("svinst_J", 6, "swarm4");
		Attack(attacker, recy, 1);
		AdvanceIn(50);
	}
	STATE(24){
		if (GetCurrentCommand(recy) == CMD_NONE){
			AdvanceIn(20);
		}
		else{
			SetState(21);
		}
	}
	STATE(25){
		Goto(recy, "base", 0);
		Goto(escort1, "base", 0);
		Goto(escort2, "base", 0);
		Goto(escort3, "base", 0);
		Goto(escort4, "base", 0);
		ClearObjectives();
		AddObjective(AtBase, GREEN);
		swarmrec = BuildObject("sbrecy00", 6, "swarmrec");
		SetScrap(6, 40);
		SetPlan("FS09_s1.aip", 6);
		Advance();
	}
	STATE(26){
		if (!IsAround(swarmrec)){
			SucceedMission(GetTime() + 10, "FS09win.des");
			Advance();
		}
	}
	CheckStuffIsAlive();
	SpawnReinforcements();
}

void fs09Mission::CheckStuffIsAlive(void){
	if (!IsAround(recy) && !MissionFailed){
		FailMission(GetTime() + 10, "FS09fail1.des");
		MissionFailed = true;
	}
	if (AAGroup1Alive){
		if (!IsAround(swarmAA1) &&
			!IsAround(swarmAA2) &&
			!IsAround(swarmAA3)){
			ClearObjectives();
			AddObjective(AA1Dead, GREEN);
			Reinforcements1Time = GetTime() + 15;
			AAGroup1Alive = false;
		}
	}
	if (AAGroup2Alive){
		if (!IsAround(swarmAA4) &&
			!IsAround(swarmAA5)){
			ClearObjectives();
			AddObjective(AA1Dead, GREEN);
			Reinforcements2Time = GetTime() + 15;
			AAGroup2Alive = false;
		}
	}
	if (AAGroup3Alive){
		if (!IsAround(swarmAA6) &&
			!IsAround(swarmAA7) &&
			!IsAround(swarmAA8)){
			ClearObjectives();
			AddObjective(AA1Dead, GREEN);
			Reinforcements3Time = GetTime() + 15;
			AAGroup3Alive = false;
		}
	}
	if (AAGroup4Alive){
		if (!IsAround(swarmAA9) &&
			!IsAround(swarmAA10)){
			ClearObjectives();
			AddObjective(AA1Dead, GREEN);
			Reinforcements4Time = GetTime() + 15;
			AAGroup4Alive = false;
		}
	}
}

void fs09Mission::SpawnReinforcements(void){
	if (!AAGroup1Alive && !Reinforcements1 && GetTime() > Reinforcements1Time){
		reinforce = BuildObject("ivsct_BD", 1, "troops");
		SetGroup(reinforce, 5);
		reinforce = BuildObject("ivsct_BD", 1, "troops");
		SetGroup(reinforce, 5);
		reinforce = BuildObject("ivserv_BD", 1, "troops");
		SetGroup(reinforce, 6);
		Reinforcements1 = true;
	}
	if (!AAGroup2Alive && !Reinforcements2 && GetTime() > Reinforcements2Time){
		reinforce = BuildObject("ivserv_BD", 1, "troops");
		SetGroup(reinforce, 6);
		reinforce = BuildObject("ivsct_BD", 1, "troops");
		SetGroup(reinforce, 5);
		reinforce = BuildObject("ivsct_BD", 1, "troops");
		SetGroup(reinforce, 5);
		Reinforcements2 = true;
	}
	if (!AAGroup3Alive && !Reinforcements3 && GetTime() > Reinforcements3Time){
		reinforce = BuildObject("ivtnk_BD", 1, "troops");
		SetGroup(reinforce, 7);
		reinforce = BuildObject("ivtnk_BD", 1, "troops");
		SetGroup(reinforce, 7);
		reinforce = BuildObject("ivtnk_BD", 1, "troops");
		SetGroup(reinforce, 7);
		Reinforcements3 = true;
	}
	if (!AAGroup4Alive && !Reinforcements4 && GetTime() > Reinforcements4Time){
		reinforce = BuildObject("ivltankBD", 1, "troops");
		SetGroup(reinforce, 8);
		reinforce = BuildObject("ivltankBD", 1, "troops");
		SetGroup(reinforce, 8);
		reinforce = BuildObject("ivltankBD", 1, "troops");
		SetGroup(reinforce, 8);
		Reinforcements4 = true;
	}
}