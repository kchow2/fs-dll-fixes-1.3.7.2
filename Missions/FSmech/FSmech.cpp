#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class FSmech : public SPMission 
{
public:
	FSmech(void)
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
	void SetupMech(void);
	void SpawnPRAttackers(void);

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
		Swarm,
		Phaer,
		Bomb,
		Serv,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		pr_attack_timer,
		f_last;

	// handles
	Handle
		h_first,
		friend1,
		enemy1,
		enemy2,
		enemy3,
		enemy4,
		attacker,
		player,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		pr_attack_state,
		i_last;
};
DLLBase * BuildMission(void)
{
	return new FSmech();
}

void FSmech::Setup(void)
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

	PreloadODF("Obfact_P");
	PreloadODF("sbrecy00");
	PreloadODF("ibrecy_m");
	PreloadODF("fbrecy_m");
	PreloadODF("ivrecyBD");
}

void FSmech::AddObject(Handle h)
{
	if (GetTeamNum(h) != 1){
		SetSkill(h, 3);
	}
}



void FSmech::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START{
		Ally(5, 6);
		FreeCamera();
		IFace_Exec("FSmech.cfg");
		IFace_Activate("INFO");
		Advance();
	}
	STATE(1){	//WINEXIT
		if (IFace_GetInteger("end.end") != 0){
			Phaer = IFace_GetInteger("Phaer.count") == 1 ? true : false;
			Swarm = IFace_GetInteger("Swarm.count") == 1 ? true : false;
			Bomb = IFace_GetInteger("Bomb.count") == 1 ? true : false;
			Serv = IFace_GetInteger("Serv.count") == 1 ? true : false;
			SetupMech();
			IFace_Deactivate("INFO");
			FreeFinish();
			Advance();
		}
	}
	STATE(2){
		if (Phaer){
			SpawnPRAttackers();
		}
		
		if (!IsAround(friend1)){
			FailMission(GetTime() + 5, "onelose.des");
			Advance();
		}
		else if (!IsAround(enemy1) && !IsAround(enemy2) && 
			!IsAround(enemy3) && !IsAround(enemy4)){
			SucceedMission(GetTime() + 5, "onewin.des");
			Advance();
		}
	}
}

void FSmech::SetupMech(void){
	//player
	friend1 = BuildObject("Obfact_P", 1, "Recy");
	SetScrap(1, 40);
	
	//whether the swarm is allied with the other enemy teams
	enemy4 = BuildObject("sbrecy00", 3, "RecyclerEnemy");
	if (Swarm){
		Ally(3, 6);
		Ally(3, 5);
		Ally(3, 4);
		Ally(3, 2);
		SetPlan("SwvMec.aip", 3);
	}
	else{
		UnAlly(3, 6);
		UnAlly(3, 5);
		UnAlly(3, 4);
		UnAlly(3, 2);
		SetPlan("SwvAll.aip", 3);
	}

	//whether the edf is allowed to use their bomber
	enemy2 = BuildObject("ibrecy_m", 5, "rece");
	if (Bomb){
		if (Serv){
			SetPlan("EDFvMBS.aip", 5);
		}
		else{
			SetPlan("EDFvMecB.aip", 5);
		}
	}
	else{
		if (Serv){
			SetPlan("EDFvMecS.aip", 5);
		}
		else{
			SetPlan("EDFvMec.aip", 5);
		}
	}

	//scion
	enemy1 = BuildObject("fbrecy_m", 6, "recs");
	if (Serv){
		SetPlan("ScivMecS.aip", 6);
	}
	else{
		SetPlan("ScivMec.aip", 6);
	}
	//BD
	enemy3 = BuildObject("ivrecyBD", 4, "recBD");
	if (Serv){
		SetPlan("BDvMecS.aip", 4);
	}
	else{
		SetPlan("BDvMec.aip", 4);
	}

	Ally(6, 5);
	Ally(5, 4);
	Ally(6, 4);
	Ally(2, 6);
	Ally(2, 4);
	Ally(2, 5);

	SetScrap(6, 40);
	SetScrap(5, 40);
	SetScrap(4, 40);
	SetScrap(3, 40);

	SetTeamColor(5, 12, 64, 0);
}

void FSmech::SpawnPRAttackers(void){
	if (pr_attack_state == 0){
		//Phaer rahn attackers are spawned in waves with a random delay
		float f = GetRandomFloat(1.0f);
		if (f < .5f){
			pr_attack_timer = GetTime() + 300;
		}
		else if (f < 0.75f){
			pr_attack_timer = GetTime() + 500;
		}
		else{
			pr_attack_timer = GetTime() + 700;
		}
		pr_attack_state++;
	}
	else if (pr_attack_state == 1 && pr_attack_timer < GetTime()){
		float f = GetRandomFloat(1.0f);
		if (f < 0.5f){
			attacker = BuildObject("PvScout", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvScout", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvtank", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvtank", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvtank", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvtank", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvmisl", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvmisl", 2, "PRspawn_1");
			Attack(attacker, friend1, 1);
			pr_attack_state = 0;
		}
		else if (f < 0.75f){
			attacker = BuildObject("pvtank", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvtank", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvtank", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvHatnk", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvHatnk", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvHatnk", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvHatnk", 2, "PRspawn_2");
			Attack(attacker, friend1, 1);
			pr_attack_state = 0;
		}
		else if (f < 0.875f){
			attacker = BuildObject("pvwalk", 2, "PRspawn_3");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvwalk", 2, "PRspawn_3");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvwalk", 2, "PRspawn_3");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvwalk", 2, "PRspawn_3");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvHatnk", 2, "PRspawn_3");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvHatnk", 2, "PRspawn_3");
			Attack(attacker, friend1, 1);
			pr_attack_state = 0;
		}
		else{
			attacker = BuildObject("pvwalk", 2, "PRspawn_4");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvwalk", 2, "PRspawn_4");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("pvatank", 2, "PRspawn_4");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvRckt", 2, "PRspawn_4");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvRckt", 2, "PRspawn_4");
			Attack(attacker, friend1, 1);
			attacker = BuildObject("PvRckt", 2, "PRspawn_4");
			Attack(attacker, friend1, 1);
			pr_attack_state = 0;
		}
	}
}