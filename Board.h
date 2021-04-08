#pragma once
#include "Sprite.h"
#include "Singleton.h"
#include "Tile.h"
#include "Player.h"
#include "Boss.h"
#include "BaseEnemy.h"
#include "Sound.h"

class Board : public Singleton<Board>
{
public:
	void FloodFill(int _Posx, int _PosY, int _index);

	bool clearedTile[50][50];

	Tile* t;
	Tile* tile[50][50];

	list<Tile*> clearPlace1;
	list<Tile*> clearPlace2;

	list<Tile*> AllClearPlaceExceptEDGE;
	list<Tile*> AllClearPlace;
	list<Tile*> pathlist;
	vector<vec2> AllClearPlacePos;
	vector<vec2> PollutionPlace;
	bool check = false; // Å½»ö°á°ú
	bool HPDOWN = false;

	int score = 0;
	list<Tile*> AllTile;

	void BoardInit();
	void BoardUpdate();
	void DeleteBoard();

	void SetEdge();


	int StartPlayerCutX;
	int StartPlayerCutY;

	int PlayerBackPosY = 0;
	int PlayerBackPosX = 0;

	int PlayerPosX = 0;
	int PlayerPosY = 0;
	bool isCut = false;
	void CheckCut();

	void IfIsCut();

	void CheckFill();
	bool ClearPlace(list<Tile*> checkplace, list<Tile*> clearplace);

	void SetColorByState();

	float delay = 0;
	void MovePlayerState();

	bool ColToEnemyByPath();

	float Lerp(float value1, float value2, float amount);

	void NodapInit(int CenterX, int CenterY);
	Sprite* NodapSprite;
};

