#include "../Include/FlyWin32.h"
struct{
	VIEWPORTid vID;
	SCENEid sID, sID2;
	OBJECTid cID, tID, lID, spID[3];
	TEXTid textID;
	void load(){
		textID = FAILED_ID;
		vID = FyCreateViewport(0, 0, 800, 600);
		FnViewport vp;
		vp.ID(vID);
		textID = FyCreateText("Trebuchet MS2", 18, FALSE, FALSE);
	}
	void GameAI(int skip){

	}
	int Movement(BYTE code, BOOL4 value){
		if (code == FY_Z)return 2;
		return 1;
	}
	void RenderIt(int skip){
		
		FnScene scene;
		scene.ID(sID);
		cID = scene.CreateObject(CAMERA);
		lID = scene.CreateObject(LIGHT);
		
		FnCamera camera;
		camera.ID(cID);
		camera.Rotate(X_AXIS, 90.0f, LOCAL);
		camera.Translate(0.0f, 10.0f, 100.0f, LOCAL);
		
		FnLight light;
		light.ID(lID);
		light.SetName("MainLight");
		light.Translate(-50.0f, -50.0f, 50.0f, GLOBAL);
		
		sID2 = FyCreateScene(1);
		scene.ID(sID2);
		scene.SetSpriteWorldSize(800, 600);
		
		FnSprite sp[3];
	float sp_color[3][3] = {{0.8, 0.0, 0.4},{0.8, 0.0, 0.4},{0.8, 0.0, 0.4}};
		spID[0] = scene.CreateObject(SPRITE);
		sp[0].ID(spID[0]);
		sp[0].SetSize(200, 40);
		sp[0].SetColor(sp_color[0]);
		sp[0].SetPosition(300, 200, 0);
		
		FnViewport vp;
		vp.ID(vID);
		vp.Render3D(cID, TRUE, TRUE);
        vp.RenderSprites(sID2, FALSE, TRUE);
		
		FnText text;
		text.ID(textID);
		text.Begin(vID);
		char S[256];
		sprintf(S, "Lyubu xxxxx Donzo is irreversible!!!\n");
		text.Write(S, 300, 200, 255, 255, 0);
		sprintf(S, "press z to start.\n");
		text.Write(S, 300, 400, 255, 255, 0);
		text.End();
	}
}MM;