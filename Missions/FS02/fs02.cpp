#include "..\Shared\SPMission.h"

#define MISSION_START if (mission_state == 0)
#define STATE(X) else if (mission_state == X && GetTime() > next_event_time)

class fs02Mission : public SPMission 
{
public:
	fs02Mission(void)
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
	void EvacuatePilots(void);
	void CheckStuffIsAlive(void);
	void DisplayEvacProgress(void);

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
		APC1Away,
		APC2Away,
		APC3Away,
		//PilotsEvacuated,
		MissionFailed,
		b_last;

	// floats
	float
		f_first,
		next_event_time,
		PilotTimer,
		f_last;

	// handles
	Handle
		h_first,
		Recy,
		Player,
		Leader,
		Barracks,
		apc_1,
		apc_2,
		apc_3,
		EvacApc,
		TempPilot,
		Attacker,
		Enemy_1,
		Enemy_2,
		Enemy_3,
		Enemy_4,
		Enemy_5,
		Enemy_6,
		Enemy_7,
		Enemy_8,
		Enemy_9,
		Outpost,
		Outpost_2,
		turr_1,
		turr_2,
		turr_3,
		turr_4,
		bones_1,
		Tank_1,
		Tank_2,
		Tank_3,
		Serv_1,
		h_last;

	// integers
	int
		i_first,
		mission_state,
		EvacPilotState,
		PilotCount,
		i_last;

	Vector swarm1 = { 1637, 2, -1203 },
		swarm2 = { -1710, 3, 1575 },
		swarm3 = { -319, 2, 1496 },
		swarm4 = { 1704, 2, -335 },
		swarm5 = { 1628, 2, 1586 },
		Pilots = { -1079, 2, -1726 },
		Reinf = { -1262, -14, -1841 };

	char *Brief_1 = "The enemy have launched\na major offensive. Many\nof our outposts have been overrun\nand we have lost control\nof the starport.",
		*Brief_2 = "We are regrouping our\nforces at the central command\ncomplex. We are evacuating all\nnon essential personnel via APC.",
		*Brief_3 = "Without dropships from the\nstarport we are unable to\nairlift the recycler out.\nThis means you'll have to\nescort it to safety.",
		*Orders_1 = "Protect the APCs while\nthey evacuate our troops\nthen escort the recycler back\nto central command.",
		*PilotsMsg = "Well done. Our troops\nhave arrived safely.\nNow you must escort the\nrecycler through the canyons.\nGood luck.",
		*Warning_1 = "Enemy turrets are blocking\n the way ahead. Clear\nthem out so we can proceed.",
		*Turrets = "Well done. The turrets\nhave been eliminated.\nWe may continue.",
		*Force = "A large enemy force\nis blocking the way ahead.\nWe must neutralize them\nbefore we can move on .",
		*WinText = "Well done the recycler\nhas made it through.\nThe rest of the journey\nto central command is\nclear of enemy forces.",

		//Added some new messages that tell you why you failed the mission
		*Fail1Text = "One of the APCs has been destroyed.",
		*Fail2Text = "One of the pilots has been killed.",
		*Fail3Text = "The Recycler has been destroyed.",
		*Fail4Text = "The Lt has been killed.",
		//as well as some progress on the evactuation
		*ApcAway = "APC %d is away.",
		*EvacProgressText = "APC %d has loaded %d of 5 pilots.";
};
DLLBase * BuildMission(void)
{
	return new fs02Mission();
}

void fs02Mission::Setup(void)
{
	mission_state = 0;
	APC1Away = false;
	APC2Away = false;
	APC3Away = false;
	MissionFailed = false;
}

void fs02Mission::AddObject(Handle h)
{

}

void fs02Mission::Execute(void)
{
	Player = GetPlayerHandle();
	MISSION_START{
		apc_1 = GetHandle("apc_1");
		apc_2 = GetHandle("apc_2");
		apc_3 = GetHandle("apc_3");
		Outpost = GetHandle("Outpost");
		Outpost_2 = GetHandle("Outpost_2");
		bones_1 = GetHandle("bones_1");
		turr_1 = GetHandle("turr_1");
		turr_2 = GetHandle("turr_2");
		turr_3 = GetHandle("turr_3");
		turr_4 = GetHandle("turr_4");
		Leader = GetHandle("Leader");
		Recy = GetHandle("Recy");
		SetScrap(1, 0);
		Defend2(Leader, Recy);
		Goto(Recy, "bay", 1);
		SetObjectiveOn(Recy);
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(5);
	}
	STATE(1){
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		ClearObjectives();
		AddObjective(Brief_1, WHITE);
		AdvanceIn(10);
	}
	STATE(2){
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(5);
	}
	STATE(3){
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, apc_3, 0);
		AdvanceIn(5);
	}
	STATE(4){
		ClearObjectives();
		AddObjective(Brief_2, WHITE);
		AdvanceIn(5);
	}
	STATE(5){
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(5);
	}
	STATE(6){
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, apc_3, 0);
		AdvanceIn(5);
	}
	STATE(7){
		ClearObjectives();
		AddObjective(Brief_3, WHITE);
		AdvanceIn(5);
	}
	STATE(8){
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(5);
	}
	STATE(9){
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, apc_3, 0);
		ClearObjectives();
		AddObjective(Orders_1, WHITE);
		AdvanceIn(5);
	}
	STATE(10){
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(5);
	}
	STATE(11){
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, apc_3, 0);
		PilotCount = 0;
		EvacPilotState = 0;
		Advance();
	}
	STATE(12){
		EvacuatePilots();
		if (APC3Away)
			Advance();
	}
	STATE(13){	//CHECK_APC
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(6);
	}
	STATE(14){
		Attacker = BuildObject("svscJ_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		AdvanceIn(6);
	}
	STATE(15){
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		AdvanceIn(8);
	}
	STATE(16){
		if (GetDistance(apc_3, Outpost) < 60){
			RemoveObject(apc_1);
			RemoveObject(apc_2);
			RemoveObject(apc_3);
			ClearObjectives();
			AddObjective(PilotsMsg, GREEN);
			AdvanceIn(15);
		}
		else{
			SetState(13);
		}
	}
	STATE(17){
		Tank_1 = BuildObject("ivtnk_BD", 1, Reinf);
		Goto(Tank_1, Recy, 0);
		AdvanceIn(4);
	}
	STATE(18){
		Tank_2 = BuildObject("ivtnk_BD", 1, Reinf);
		Goto(Tank_2, Recy, 0);
		AdvanceIn(4);
	}
	STATE(19){
		Tank_3 = BuildObject("ivtnk_BD", 1, Reinf);
		Goto(Tank_3, Recy, 0);
		AdvanceIn(4);
	}
	STATE(20){
		Serv_1 = BuildObject("ivserv_BD", 1, Reinf);
		Goto(Serv_1, Recy, 0);
		AdvanceIn(10);
	}
	STATE(21){
		Goto(Recy, "recPath_1", 1);
		Advance();
	}
	STATE(22){	//CHECK_RECY_1
		Attacker = BuildObject("svscA_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		AdvanceIn(25);
	}
	STATE(23){
		Attacker = BuildObject("svscJ_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		if (GetDistance(Recy, bones_1) < 100){
			AdvanceIn(35);
		}
		else{
			SetState(22, 35);
		}
		
	}
	STATE(24){
		ClearObjectives();
		AddObjective(Warning_1, WHITE);
		AdvanceIn(15);
	}
	STATE(25){
		Attacker = BuildObject("svwlkL_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		Attacker = BuildObject("svwlkL_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		Attacker = BuildObject("svwlkL_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		Advance();
	}
	STATE(26){	//CHECK_TURRETS
		Attacker = BuildObject("svscJ_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		AdvanceIn(12);
	}
	STATE(27){
		if (IsAround(turr_1) || 
			IsAround(turr_2) || 
			IsAround(turr_3) || 
			IsAround(turr_4)){
			SetState(26);
		}
		else{
			ClearObjectives();
			AddObjective(Turrets, GREEN);
			AdvanceIn(15);
		}
	}
	STATE(28){
		Goto(Recy, "recPath_2", 1);
		Advance();
	}
	STATE(29){	//CHECK_RECY_2
		Attacker = BuildObject("svscJ_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		Attacker = BuildObject("svscA_D", 5, swarm3);
		Goto(Attacker, Recy, 0);
		AdvanceIn(22);
	}
	STATE(30){
		Attacker = BuildObject("svscJ_D", 5, swarm2);
		Goto(Attacker, Recy, 0);
		Attacker = BuildObject("svscA_D", 5, swarm3);
		Goto(Attacker, Recy, 0);
		AdvanceIn(22);
	}
	STATE(31){
		if (GetDistance(Recy, Outpost_2) < 100){
			ClearObjectives();
			AddObjective(Force, WHITE);
			AdvanceIn(10);
		}
		else{
			SetState(29);
		}
	}
	STATE(32){
		Enemy_1 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_1, Recy, 0);
		Enemy_2 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_2, Recy, 0);
		Enemy_3 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_3, Recy, 0);
		Enemy_4 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_4, Recy, 0);
		Enemy_5 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_5, Recy, 0);
		Enemy_6 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_6, Recy, 0);
		Enemy_7 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_7, Recy, 0);
		Enemy_8 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_8, Recy, 0);
		Enemy_9 = BuildObject("svwlkL_D", 5, swarm5);
		Goto(Enemy_9, Recy, 0);
		Advance();
	}
	STATE(33){	//CHECK_FORCES
		Attacker = BuildObject("svscL_D", 5, swarm4);
		Goto(Attacker, Recy, 0);
		AdvanceIn(12);
	}
	STATE(34){
		if (IsAround(Enemy_1) ||
			IsAround(Enemy_2) ||
			IsAround(Enemy_3) ||
			IsAround(Enemy_4) ||
			IsAround(Enemy_5) ||
			IsAround(Enemy_6) ||
			IsAround(Enemy_7) ||
			IsAround(Enemy_8) ||
			IsAround(Enemy_9)){
			SetState(33);
		}
		else{
			Goto(Recy, Outpost, 1);
			AdvanceIn(60);
		}
	}
	STATE(35){
		ClearObjectives();
		AddObjective(WinText, GREEN);
		SucceedMission(GetTime() + 16, "FS02win.des");
		Advance();
	}

	CheckStuffIsAlive();
}

void fs02Mission::EvacuatePilots(void){
	float delay = PilotCount < 5 ? 10 : PilotCount < 10 ? 8 : 6;
	if (EvacPilotState == 0){
		TempPilot = BuildObject("ispilo", 1, Pilots);
		PilotCount++;
		EvacApc = PilotCount < 6 ? apc_1 : PilotCount < 11 ? apc_2 : apc_3;
		Goto(TempPilot, EvacApc, 1);
		Attacker = BuildObject("svscA_D", 5, swarm1);
		Goto(Attacker, Recy, 0);
		PilotTimer = GetTime() + delay;
		EvacPilotState++;
	}
	else if (EvacPilotState == 1 && GetTime() > PilotTimer){
		Attacker = APC2Away ? BuildObject("svscA_D", 5, swarm2) : BuildObject("svscJ_D", 5, swarm2);
		Goto(Attacker, apc_3, 0);
		PilotTimer = GetTime() + delay;
		EvacPilotState++;
	}
	else if (EvacPilotState == 2 && GetTime() > PilotTimer){
		if (!IsAround(TempPilot) && !MissionFailed){
			ClearObjectives();
			AddObjective(Fail2Text, RED);
			FailMission(GetTime() + 10, "FS02fail2.des");
			MissionFailed = true;
		}
		else if (GetDistance(TempPilot, EvacApc) < 12){
			RemoveObject(TempPilot);
			if (PilotCount == 5){
				Goto(apc_1, Outpost, 1);
				APC1Away = true;
			}
			else if (PilotCount == 10){
				Goto(apc_2, Outpost, 1);
				APC2Away = true;
			}
			else if (PilotCount == 15){
				Goto(apc_3, Outpost, 1);
				APC3Away = true;
			}
			EvacPilotState = 0;
			DisplayEvacProgress();
		}
	}
}

void fs02Mission::DisplayEvacProgress(){
	char buf[100];
	ClearObjectives();
	if (APC1Away){
		sprintf_s(buf, ApcAway, 1);
		AddObjective(buf, GREEN);
	}
	else{
		sprintf_s(buf, EvacProgressText, 1, PilotCount);
		AddObjective(buf, WHITE);
	}
	if (APC1Away){
		if (APC2Away){
			sprintf_s(buf, ApcAway, 2);
			AddObjective(buf, GREEN);
		}
		else{
			sprintf_s(buf, EvacProgressText, 2, PilotCount - 5);
			AddObjective(buf, WHITE);
		}
	}
	if (APC2Away){
		if (APC3Away){
			sprintf_s(buf, ApcAway, 3);
			AddObjective(buf, GREEN);
		}
		else{
			sprintf_s(buf, EvacProgressText, 3, PilotCount - 10);
			AddObjective(buf, WHITE);
		}
	}
}

void fs02Mission::CheckStuffIsAlive(void){
	if (mission_state <= 16 && (!IsAround(apc_1) || !IsAround(apc_2) || !IsAround(apc_3)) && !MissionFailed){
		ClearObjectives();
		AddObjective(Fail1Text, RED);
		FailMission(GetTime() + 10, "FS02fail1.des");
		MissionFailed = true;
	}
	if (!IsAround(Recy) && !MissionFailed){
		ClearObjectives();
		AddObjective(Fail3Text, RED);
		FailMission(GetTime() + 10, "FS02fail3.des");
		MissionFailed = true;
	}
	if (!IsAround(Leader) && !MissionFailed){
		ClearObjectives();
		AddObjective(Fail4Text, RED);
		FailMission(GetTime() + 10, "FS02fail4.des");
		MissionFailed = true;
	}
}