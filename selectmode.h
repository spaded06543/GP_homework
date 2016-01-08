#include "../Include/FlyWin32.h"
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
	int character_number;
	int default_character;
	int select_character;
	
	void load(){
		character_number = 4;
		default_character = 0;
		select_character = default_character;

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
	
	void GameAI(int skip){
	}
	
	int Movement(BYTE code, BOOL4 value){
		int ret = 3;
		if (value){
			switch (code){
			case FY_Z:
				ret = 4;
				break;
			case FY_UP:
				select_character += 1;
				break;
			case FY_DOWN:
				select_character -= 1;
				break;
			default:
				break;
			}

			if (select_character == character_number){
				select_character = 0;
			}
			else if (select_character == -1){
				select_character = character_number - 1;
			}
		}
		return ret;
	}
	
	void RenderIt(int skip){
		
		FnViewport vp;
		FnSprite sp;
		int i;
		for (i = 0; i < character_number; i++){
			sp.ID(character_sp.at(i));
			if (i == select_character){
				sp.Show(TRUE);
			}
			else{
				sp.Show(FALSE);
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
		sprintf(S, "Choose the character you want\nPress \"Z\" to select character.\n");
		sprintf(character_info, "Character : \nAttack : \nBlood : \nSpeed : \n"); // TODO : need characterinformation
		text.Write(S, positionX, positionY, red, green, blue); // 
		text.Write(character_info, 300, 350, red, green, blue); // print infomation 
		text.End();
	}
}SM;