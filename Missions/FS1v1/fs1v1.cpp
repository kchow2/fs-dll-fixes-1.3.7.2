#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs1v1 : public SPMission 
{
public:
	fs1v1(void)
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
	void SetupIA(void);

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
		friend1,
		friend2,
		enemy1,
		player,
		newguy,
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
	return new fs1v1();
}

void fs1v1::Setup(void)
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
	PreloadODF("obfact_I");
}

void fs1v1::AddObject(Handle h)
{

}



void fs1v1::Execute(void)
{
	player = GetPlayerHandle();
	MISSION_START{
		Ally(5, 6);
		FreeCamera();
		IFace_Exec("FSinstant.cfg");
		IFace_Activate("INFO");
		Advance();
	}
	STATE(1){	//WINEXIT
		if (IFace_GetInteger("end.end") != 0){
			race1 = IFace_GetInteger("race.count");
			race2 = IFace_GetInteger("race2.count");
			SetupIA();
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
		else if (!IsAround(enemy1)){
			SucceedMission(GetTime() + 5, "onewin.des");
			Advance();
		}
	}
	
}

void fs1v1::SetupIA(void){
	/*RACE CODES:
	race1 (Player Race):
	1: Scion
	2: EDF
	3: Black Dog
	4: Swarm
	5: Phaer Rhan

	race2: (enemy race)
	1: Scion
	2: EDF
	3: Black Dog
	4: Swarm
	5: Mechana
	*/

	//Sanity check
	if (race1 < 1 || race1 > 5){
		ClearObjectives();
		AddObjective("Error: invalid selection for player race!", RED);
		return;
	}
	else if (race2 < 1 || race2 > 5){
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

	//Setup the enemy recycler
	enum{ SCI = 1, EDF, BD, SW, PR };
	const char* playerRaceCodes[] = { "", "SCI", "EDF", "BD", "SW", "PR" };
	const char* enemyRaceCodes[] = { "", "SCI", "EDF", "BD", "SW", "Mech" };
	char* enemySpawnLocs[] = { "", "recs", "rece", "recBD", "RecyclerEnemy", "M_Fact" };
	char* enemyRecyclers[] = { "", "fbrecy_m", "ibrecy_m", "bbrecy", "sbrecy00", "obfact_I" };

	enemy1 = BuildObject(enemyRecyclers[race2], 6, enemySpawnLocs[race2]);

	//set the appropriate AIPs
	char buf[100];
	sprintf_s(buf, "%sv%s.aip", enemyRaceCodes[race2], playerRaceCodes[race1]);
	SetPlan(buf, 6);

	SetScrap(6, 40);
}