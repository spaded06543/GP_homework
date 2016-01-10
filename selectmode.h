#include "../Include/FlyWin32.h"
#ifndef _battleC_h_
#define TYPESOFC 6
#include "battleC.h"
#endif
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>

struct{
	VIEWPORTid vID;
	SCENEid sID;
	OBJECTid cID, tID, spID;
	TEXTid textID;
	std::vector<OBJECTid> character_sp;
	int if_select;
	int character_number;
	int default_character;
	int select_character, select_enemy;
	int fighter_mode; // fighters number (1: 1 to 1 / 3: N to N)
	
	void load(){
		if_select = 0;
		character_number = 6;
		default_character = 0;
		select_character = default_character;
		select_enemy = default_character;
		fighter_mode = 1;
		textID = FAILED_ID;
		vID = FyCreateViewport(0, 0, 800, 600);
		FnViewport vp;
		vp.ID(vID);
		textID = FyCreateText("Trebuchet MS3", 18, FALSE, FALSE);
		FnScene scene;
		
		sID = FyCreateScene(1);
		scene.ID(sID);
		scene.SetSpriteWorldSize(800, 600);
		
		spID = scene.CreateObject(SPRITE);
		FnSprite sp;
		
		int i;
		char tmp[30];
		for(i = 0; i < character_number; i++){
			sprintf(tmp, "Image/%d", i);
			character_sp.push_back(scene.CreateObject(SPRITE));
			sp.ID(character_sp.at(i));
			sp.SetSize(120, 120);
			sp.SetImage(tmp, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, false, false);
			sp.SetPosition(300, 300, 0);	
		}
		if (character_sp.size() != character_number){
			printf("!!!!something wrong with vector!!!!\n");
			exit(3);
		}
	}
	
	void GameAI(int skip){}
	
	int Movement(BYTE code, BOOL4 value){
		int ret = 3;
		if (value){
			switch (code){
			case FY_X:
				if(if_select == 0){
					ret = 1;
				}
				else{
					if_select -= 1;
				}
				break;
			case FY_Z:
				if(if_select == 2){
					ret = 4;
				}
				else{
					if_select ++;
				}
				break;
			case FY_UP:
				if (if_select == 0){
					select_character += 1;
				}
				else if (if_select == 1){
					select_enemy += 1;
				}
				else{
					fighter_mode += 2;
				}
				break;
			case FY_DOWN:
				if (if_select == 0){
					select_character -= 1;
				}
				else if (if_select == 1){
					select_enemy -= 1;
				}
				else{
					fighter_mode -= 2;
				}
				break;
			default:
				break;
			}
			
			if (select_enemy == character_number){
				select_enemy = 0;
			}
			else if (select_enemy == -1){
				select_enemy = character_number - 1;
			}
			
			if (fighter_mode == -1){
				fighter_mode = 3;
			}
			else if (fighter_mode == 5){
				fighter_mode = 1;
			}

			if (select_character == character_number){
				select_character = 0;
			}
			else if (select_character == -1){
				select_character = character_number - 1;
			}
		}
		if (ret == 4) if_select = 0;
		return ret;
	}
	
	void RenderIt(int skip){
		FnViewport vp;
		
		if (if_select != 2){
			FnSprite sp;
			int i;
			for (i = 0; i < character_number; i++){
				sp.ID(character_sp.at(i));
				if(if_select > 0){
					if (i == select_enemy){
						sp.Show(TRUE);
					}
					else{
						sp.Show(FALSE);
					}
				}
				else{
					if (i == select_character){
						sp.Show(TRUE);
					}
					else{
						sp.Show(FALSE);
					}
				}
			}
			vp.ID(vID);
			vp.Render3D(cID, TRUE, TRUE);
			vp.RenderSprites(sID, TRUE, TRUE);

			FnText text;
			text.ID(textID);
			text.Begin(vID);
			char S[256], character_info[256];
			BYTE red = 255, green = 255, blue = 255;
			int positionX = 300, positionY = 100;
			if(if_select > 0){
				sprintf(S, "Choose the \"enemy\" you want\nPress \"Z\" to select character.\n Press \"X\" to go back.\n");
				sprintf(character_info, "Enemy : %s\nAttack : \t%f\nBlood : \t%d\nSpeed : \t%d\n", 
					cname[select_enemy].c_str(), CASL[select_enemy].speed,
					CASL[select_enemy].life, CASL[select_enemy].attnD);
				// TODO : need characterinformation	
			}
			else{
				sprintf(S, "Choose the character you want\nPress \"Z\" to select character.\n Press \"X\" to go back.\n");
				sprintf(character_info, "Character : %s\nAttack : \t%f\nBlood : \t%d\nSpeed : \t%d\n", 
					cname[select_character].c_str(), CASL[select_character].speed,
					CASL[select_character].life, CASL[select_character].attnD);
				// TODO : need characterinformation
			}
			text.Write(S, positionX, positionY, red, green, blue); // 
			text.Write(character_info, 300, 350, red, green, blue); // print infomation 
			text.End();
		}
		else{
			vp.ID(vID);
			vp.Render3D(cID, TRUE, TRUE);
			FnText text;
			text.ID(textID);
			text.Begin(vID);
			char S[256], mode_explain[256];
			BYTE red = 255, green = 255, blue = 255;
			
			sprintf(S, "Choose the game mode you want\nPress \"Z\" to select.\n Press \"X\" to go back.");
			if (fighter_mode == 1){
				sprintf(mode_explain, "1 on 1\nIn this mode, you will fight on your own.\nTarget: Defeat the enemy you choose.");
			}
			else{
				sprintf(mode_explain, "Group battle\nIn this mode, you will fight with your team.\nTarget: Defeat all enemies.");
			}
			text.Write(S, 300, 200, red, green, blue); // 
			text.Write(mode_explain, 300, 350, red, green, blue); // print infomation 
			text.End();
		}
	}
}SM;