#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs01Mission : public SPMission 
{
public:
	fs01Mission(void)
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
	void CheckPlayerDistanceToConvoy(void);
	void SwarmAttack(void);
	void SendWaves(void);
	void SendWaves2(void);

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
		ConvoyDistanceWarning,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		PrevWarningTime,
		SwarmAttackTimer,
		f_last;

	// handles
	Handle
		h_first,
		Attacker,
		Attacker0,
		Attacker1,
		Attacker2,
		Attacker3,
		Attacker4,
		Attacker5,
		Attacker6,
		Attacker7,
		Attacker8,
		Attackers[6],
		MyRecycler,
		Wingman_1,
		Wingman_2,
		Leader,
		RepTeam,
		Player,
		Hangar,
		Outpost,
		Bones,
		Service_1,
		Service_2,
		GunTow,
		GunTow2,
		LastWalk,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		SwarmAttackState,
		AttackerCount,
		WaveCount,
		i_last;

	char *msgOrders_1 = "Lt. Harker, we have\nlost contact with outpost\none. Escort a repair team\nto the outpost. We need \nthat com bunker back online.",
		*msgOrders_2 = "We're under attack. They're\neverywhere. Protect that repair\nteam at all costs. Ensure they\nget safely back to base .",
		*msgOrders_3 = "Build a scavenger and start making\nsome turrets a large enemy force\nis inbound.",
		*msgObsevation = "Something is wrong here.\nAll the trees are dying\nand there's no sign of any\nanimal life at all.",
		*msgComms = "We've lost comms with base.\nAll this dust is screwing things\nup. Continue with the mission\nwe'll contact base when we get\nthe bunker online.",
		*msgOutpost = "What the hell has happened here?\nThe outpost is totally destroyed.\nBaker,Davidson and Mckenzie\ncheck the perimeter for hostiles.",
		*msgWarning_1 = "Warning: Stay in Formation.",
		*msgObjective_1 = "Well done. The Repair Team\nhas arrived back safely.",
		*msgWinText = "Well done. You have survived.\nFor now !!";

	Vector SwarmSP1 = { 1628, 2, 1586 },
		SwarmSP2 = { -1715, -5, 1532 },
		Serv_1 = { -1236, 2, -1728 },
		Serv_2 = { -1294, 2, -1730 };
};
DLLBase * BuildMission(void)
{
	return new fs01Mission();
}

void fs01Mission::Setup(void)
{
//  bools
	ConvoyDistanceWarning = false;
  //  floats
	PrevWarningTime = 0;

//  integers
	mission_state = 0;
	SwarmAttackState = 0;
}

void fs01Mission::AddObject(Handle h)
{

}

void fs01Mission::Execute(void)
{
	Player = GetPlayerHandle();
	
	MISSION_START{
		SetScrap(1, 0);
		ClearObjectives();
		AddObjective(msgOrders_1, WHITE);
		Outpost = GetHandle("Outpost");
		GunTow2 = GetHandle("GunTow2");
		GunTow = GetHandle("GunTow");
		Bones = GetHandle("Bones");
		Wingman_1 = GetHandle("Wingman_1");
		Wingman_2 = GetHandle("Wingman_2");
		RepTeam = GetHandle("RepTeam");
		Leader = GetHandle("Leader");
		MyRecycler = GetHandle("MyRecycler");
		Hangar = GetHandle("Hangar");
		Goto(RepTeam, "Rep_Path", 1);	
		AdvanceIn(10);
	}
	STATE(1){		
		Follow(Leader, RepTeam);
		Follow(Wingman_1, Leader);
		Follow(Wingman_2, Leader);
		SetObjectiveOn(RepTeam);
		Goto(RepTeam, "PatrolRte_1", 1);
		AdvanceIn(40);
	}
	STATE(2){
		ClearObjectives();
		AddObjective(msgObsevation, WHITE);
		Advance();
	}
	STATE(3){
		CheckPlayerDistanceToConvoy();
		if (GetDistance(RepTeam, Bones) < 100){
			ClearObjectives();
			AddObjective(msgComms, WHITE);
			Goto(RepTeam, "PatrolRte_2", 1);
			Advance();
		}		
	}
	STATE(4){
		CheckPlayerDistanceToConvoy();
		if (GetDistance(RepTeam, Outpost) < 80){
			ClearObjectives();
			AddObjective(msgOutpost, WHITE);
			Patrol(Wingman_1, "Perimeter_Path");
			Follow(Wingman_2, Wingman_1);
			AdvanceIn(20);
		}
	}
	STATE(5){
		Attacker = BuildObject("svscL_D", 5, SwarmSP1);
		Attack(Attacker, Wingman_1);
		AdvanceIn(35);
	}
	STATE(6){
		ClearObjectives();
		AddObjective(msgOrders_2, WHITE);
		Attack(Wingman_1, Attacker, 0);
		Follow(Wingman_2, Wingman_1, 0);
		AdvanceIn(10);
	}
	STATE(7){
		for (int i = 0; i < 6; i++){
			Handle h = BuildObject("svscL_D", 5, SwarmSP1);
			Attack(h, RepTeam);
		}
		
		Service_1 = BuildObject("ivserv_BD", 1, Serv_1);
		Goto(Service_1, "Rep_Path", 1);
		Service_2 = BuildObject("ivserv_BD", 1, Serv_2);
		Goto(Service_2, "Rep_Path", 1);
		AdvanceIn(20);
	}
	STATE(8){
		Goto(RepTeam, "EscPath_1", 1);
		SwarmAttackTimer = GetTime();
		SwarmAttackState = 0;
		Advance();
	}
	STATE(9){
		CheckPlayerDistanceToConvoy();
		SwarmAttack();
		if (GetDistance(RepTeam, Hangar) < 80){
			Advance();
		}
	}
	STATE(10){
		Goto(Leader, "Rep_Path", 1);
		ClearObjectives();
		AddObjective(msgObjective_1, GREEN);
		AdvanceIn(10);
	}
	STATE(11){
		SetObjectiveOff(RepTeam);
		Goto(Service_1, GunTow2, 0);
		Service(Service_2, Leader, 0);
		AddScrap(1, 40);
		ClearObjectives();
		AddObjective(msgOrders_3, WHITE);
		AdvanceIn(10);
	}
	STATE(12){
		Defend2(Leader, GunTow2);
		SwarmAttackTimer = GetTime();
		SwarmAttackState = 0;
		Advance();
	}
	STATE(13){
		SendWaves();
		if (WaveCount >= 5){
			LastWalk = BuildObject("svwlkL_D", 5, "attackers_2");
			Goto(LastWalk, MyRecycler);
			Advance();
		}
	}
	STATE(14){	//LAST_WALK_POS
		AdvanceIn(15);
	}
	STATE(15){
		Attacker = BuildObject("svscA_D", 5, "attackers_2");
		Goto(Attacker, MyRecycler, 1);
		AdvanceIn(2);
	}
	STATE(16){
		Attacker = BuildObject("svscA_D", 5, "attackers_2");
		Goto(Attacker, MyRecycler, 1);
		AdvanceIn(2);
	}
	STATE(17){
		Attacker = BuildObject("svscA_D", 5, "attackers_2");
		Goto(Attacker, MyRecycler, 1);
		AdvanceIn(2);
	}
	STATE(18){
		Attacker = BuildObject("svscJ_D", 5, "attackers_1");
		Goto(Attacker, Hangar, 1);
		AdvanceIn(2);
	}
	STATE(19){
		Attacker = BuildObject("svscJ_D", 5, "attackers_1");
		Goto(Attacker, Hangar, 1);
		if (IsAround(GunTow)){
			AdvanceIn(2);
		}
		else if (IsAround(GunTow2)){
			SetState(23);
		}
		else{
			SetState(25);
		}
	}
	STATE(20){	//ATTACK_TOWER_1
		Attacker = BuildObject("svscA_D", 5, "attackers_1");
		Goto(Attacker, GunTow, 1);
		AdvanceIn(2);
	}
	STATE(21){
		Attacker = BuildObject("svscA_D", 5, "attackers_1");
		Goto(Attacker, GunTow, 1);
		if (!IsAround(GunTow2)){
			SetState(25);
		}
		else{
			Advance();
		}
	}
	STATE(22){	//ATTACK_TOWER_2
		Attacker = BuildObject("svscA_D", 5, "attackers_2");
		Goto(Attacker, GunTow2, 1);
		AdvanceIn(2);
	}
	STATE(23){
		Attacker = BuildObject("svscA_D", 5, "attackers_2");
		Goto(Attacker, GunTow2, 1);
		AdvanceIn(2);
	}
	STATE(24){	//CHECK_WALK
		if (!IsAround(LastWalk)){
			AdvanceIn(100);
		}
		else{
			SetState(14);
		}
	}
	STATE(25){
		ClearObjectives();
		AddObjective(msgWinText, GREEN);
		SucceedMission(GetTime() + 16, "Winner.des");
		Advance();
	}
}

void fs01Mission::CheckStuffIsAlive(void){
	
	if (!IsAround(Leader)){
		FailMission(GetTime() + 10, "failtext.des");
		SetState(99);
	}
	if (mission_state < 11 && !IsAround(RepTeam)){
		FailMission(GetTime() + 10, "failtext2.des");
		SetState(99);
	}
	if (!IsAround(MyRecycler)){
		FailMission(GetTime() + 10, "failtext3.des");
		SetState(99);
	}
	if (!IsAround(Hangar)){
		FailMission(GetTime() + 10, "failtext.des");
		SetState(99);
	}
}

void fs01Mission::CheckPlayerDistanceToConvoy(void){
	if (GetTime() - PrevWarningTime > 10 && GetDistance(Leader, Player) > 400){
		ClearObjectives();
		AddObjective(msgWarning_1, RED);
		PrevWarningTime = GetTime();
		ConvoyDistanceWarning = true;
	}

	if (ConvoyDistanceWarning && GetDistance(Player, Leader) < 400){
		ClearObjectives();
		ConvoyDistanceWarning = false;
	}
}

void fs01Mission::SwarmAttack(void){
	if (GetTime() > SwarmAttackTimer){
		switch (SwarmAttackState){
		case 0:
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 35;
			break;
		case 1:
		case 2:
		case 3:
			Attacker = BuildObject("svscL_D", 5, SwarmSP1);
			Attack(Attacker, RepTeam);
			SwarmAttackTimer = GetTime() + 2;
			SwarmAttackState++;
			break;
		case 4:
			Attacker = BuildObject("svscJ_D", 5, SwarmSP1);
			Attack(Attacker, RepTeam);
			SwarmAttackTimer = GetTime() + 2;
			SwarmAttackState = 0;
			break;
		case 5:
			Attacker = BuildObject("svscJ_D", 5, SwarmSP1);
			Attack(Attacker, RepTeam);
			SwarmAttackState = 0;
			break;
		}
	}
}

void fs01Mission::SendWaves(void){
	if (GetTime() > SwarmAttackTimer){
		switch (SwarmAttackState){
		case 0:
			Attacker1 = BuildObject("svscJ_D",5,"attackers_2");
			Attack(Attacker1, MyRecycler, 0);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 3;
			break;
		case 1:
			Attacker2 = BuildObject("svscA_D", 5, "attackers_2");
			Goto(Attacker2, MyRecycler, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 3;
			break;
		case 2:
			Attacker3 = BuildObject("svscA_D", 5, "attackers_2");
			Goto(Attacker3, MyRecycler, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 3;
			break;
		case 3:
			Attacker6 = BuildObject("svscA_D", 5, "attackers_1");
			Goto(Attacker6, Hangar, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 3;
			break;
		case 4:
			Attacker7 = BuildObject("svwlkL_D", 5, "attackers_1");
			Goto(Attacker7, Hangar, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 3;
			break;
		case 5:
			Attacker8 = BuildObject("svwlkL_D", 5, "attackers_1");
			SetMaxHealth(Attacker8, 5800);
			SetCurHealth(Attacker8, 5800);
			Goto(Attacker8, Hangar, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 50;
			break;
		case 6:
			WaveCount++;
			SwarmAttackState = 0;
			break;
		}
		
	}
}

/*void fs01Mission::SendWaves2(void){
	if (GetTime() > SwarmAttackTimer){
		switch (SwarmAttackState){
		case 0:
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 15;
			break;
		case 1:
		case 2:
			Attacker = BuildObject("svscA_D", 5, "attackers_2");
			Attack(Attacker, MyRecycler, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 2;
			break;
		case 3:
			Attacker = BuildObject("svscA_D", 5, "attackers_2");
			Goto(Attacker, MyRecycler, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 2;
			break;
		case 4:
		case 5:
			Attacker = BuildObject("svscJ_D", 5, "attackers_1");
			Goto(Attacker, Hangar, 1);
			SwarmAttackState++;
			SwarmAttackTimer = GetTime() + 2;
			break;
		case 6:
		case 7:
			if (IsAround(GunTow)){
				Attacker = BuildObject("svscJ_D", 5, "attackers_1");
				Attack(Attacker, GunTow, 1);
			}
			else if (IsAround(GunTow2)){
				Attacker = BuildObject("svscJ_D", 5, "attackers_1");
				Attack(Attacker, GunTow2, 1);
			}
			SwarmAttackState=0;
			SwarmAttackTimer = GetTime() + 2;
			break;
		}
	}
}*/