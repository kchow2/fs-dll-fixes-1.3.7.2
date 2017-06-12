#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs3way : public SPMission 
{
public:
	fs3way(void)
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
	void Setup3Way(void);

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
		f_last;

	// handles
	Handle
		h_first,
		player,
		friend1,
		friend2,
		enemy1,
		enemy2,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		race1,
		race2,
		i_last;
};
DLLBase * BuildMission(void)
{
	return new fs3way();
}

void fs3way::Setup(void)
{
	//SetAutoGroupUnits(false);
	
/*
	Here's where you set the values at the start.  
*/

//  bools
	MissionFailed = false;
  //  floats

//  handles

//  integers
	mission_state = 0;

	//Preloading reduces lag spikes when things like recys, constructors are spawned in for the first time
	PreloadODF("fvrecy_m");
	PreloadODF("ivrecy_m");
	PreloadODF("bvrecy");
	PreloadODF("svrecy00");
	PreloadODF("tvrecy00");
	PreloadODF("fbrecy_m");
	PreloadODF("ibrecy_m");
	PreloadODF("bbrecy");
	PreloadODF("sbrecy00");
	PreloadODF("tbrecy00");
	PreloadODF("pvproc");
	PreloadODF("pvcons");
}

void fs3way::AddObject(Handle h){
	//all created enemy ships have skill of 3
	if (GetTeamNum(h) != 1){
		SetSkill(h, 3);
	}
}

void fs3way::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START{
		Ally(5, 6);
		FreeCamera();
		IFace_Exec("FS3way.cfg");
		IFace_Activate("INFO");
		Advance();
	}
	STATE(1){	//WINEXIT
		if (IFace_GetInteger("end.end") != 0){
			race1 = IFace_GetInteger("race.count");
			race2 = IFace_GetInteger("race2.count");
			Setup3Way();
			IFace_Deactivate("INFO");
			FreeFinish();
			Advance();
		}
	}
	STATE(2){
		if (!IsAround(friend1)){
			FailMission(GetTime() + 5, "onelose.des");
			Advance();
		}
		else if (!IsAround(enemy1) && !IsAround(enemy2)){
			SucceedMission(GetTime() + 5, "onewin.des");
			Advance();
		}
	}
}

void fs3way::Setup3Way(void){
	/*RACE CODES:
	race1 (Player Race):
	1: Scion
	2: EDF
	3: Black Dog
	4: Swarm
	5: Phaer Rhan

	race2: (enemy1+enemy2 race)
	1: Scion+Swarm
	2: Swarm+Black Dog
	3: Swarm+EDF
	4: Scion+Black Dog
	5: Scion+EDF
	6: Black Dog+EDF
	*/

	//Sanity check
	if (race1 < 1 || race1 > 5){
		ClearObjectives();
		AddObjective("Error: invalid selection for player race!", RED);
		return;
	}
	else if (race2 < 1 || race2 > 6){
		ClearObjectives();
		AddObjective("Error: invalid selection for enemy race!", RED);
		return;
	}

	//Setup the Player's Recycler  + starting ship
	Handle playerShip;
	switch (race1){
	case 1:
		friend1 = BuildObject("fvrecy_m", 1, "Recy");
		playerShip = BuildObject("fvscout", 1, "Recy");
		break;
	case 2:
		friend1 = BuildObject("ivrecy_m", 1, "Recy");
		playerShip = BuildObject("ivscout", 1, "Recy");
		break;
	case 3:
		friend1 = BuildObject("bvrecy", 1, "Recy");
		playerShip = BuildObject("bvscout", 1, "Recy");
		break;
	case 4:
		friend1 = BuildObject("tvrecy00", 1, "Recy");
		playerShip = BuildObject("tvscout_A", 1, "Recy");
		break;
	case 5:
		friend1 = BuildObject("pvproc", 1, "Recy");
		friend2 = BuildObject("pvcons", 1, "Cons");
		playerShip = BuildObject("pvscout", 1, "Recy");
		break;
	default:
		//should never reach here
		playerShip = BuildObject("ivscout", 1, "Recy");
		break;
	}

	//replace the player's starting ship
	Matrix newPos;
	GetPosition(friend1, newPos);
	newPos.posit.x += 40; newPos.posit.y += 50;
	SetPosition(playerShip, newPos);
	RemoveObject(GetPlayerHandle());
	SetAsUser(playerShip, 1);

	SetScrap(1, 40);

	//Setup the 2 enemy recyclers
	enum{ SCI = 1, EDF, BD, SW, PR };
	const char* raceCodes[] = { "", "SCI", "EDF", "BD", "SW", "PR"};
	char* enemySpawnLocs[] = { "", "recs", "rece", "recBD", "RecyclerEnemy" };
	char* enemyRecyclers[] = { "", "fbrecy_m", "ibrecy_m", "bbrecy", "sbrecy00" };
	int enemyRace1 = 1, enemyRace2 = 1;
	
	switch (race2){
	case 1: //sci+sw
		enemyRace1 = SCI;
		enemyRace2 = SW;
		break;
	case 2:	//sw+bd
		enemyRace1 = SW;
		enemyRace2 = BD;
		break;
	case 3:	//sw+edf
		enemyRace1 = SW;
		enemyRace2 = EDF;
		break;
	case 4:	//sci+bd
		enemyRace1 = SCI;
		enemyRace2 = BD;
		break;
	case 5:	//sci+edf
		enemyRace1 = SCI;
		enemyRace2 = EDF;
		break;
	case 6:	//bd+edf
		enemyRace1 = BD;
		enemyRace2 = EDF;
		break;
	default:
		break;
	}
	enemy1 = BuildObject(enemyRecyclers[enemyRace1], 6, enemySpawnLocs[enemyRace1]);
	enemy2 = BuildObject(enemyRecyclers[enemyRace2], 5, enemySpawnLocs[enemyRace2]);

	//set the appropriate AIPs
	char buf[100];
	sprintf_s(buf, "%sv%s.aip", raceCodes[enemyRace1], raceCodes[race1]);
	SetPlan(buf, 6);
	sprintf_s(buf, "%sv%s.aip", raceCodes[enemyRace2], raceCodes[race1]);
	SetPlan(buf, 5);

	SetScrap(6, 40);
	SetScrap(5, 40);
}