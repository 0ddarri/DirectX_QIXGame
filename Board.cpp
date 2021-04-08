#include "DXUT.h"
#include "Board.h"
#include "BaseItem.h"
#include "Global.h"

void Board::FloodFill(int _Posx, int _PosY, int _index)
{
	if (clearedTile[_Posx][_PosY] == true)
		return;
	clearedTile[_Posx][_PosY] = true;

	if (!(tile[_Posx][_PosY]->state == NONE))
	{
		return;
	}

	if (_index == 0)
		clearPlace1.emplace_back(tile[_Posx][_PosY]);
	else
		clearPlace2.emplace_back(tile[_Posx][_PosY]);

	FloodFill(_Posx + 1, _PosY, _index);
	FloodFill(_Posx - 1, _PosY, _index);
	FloodFill(_Posx, _PosY + 1, _index);
	FloodFill(_Posx, _PosY - 1, _index);
}

void Board::BoardInit()
{
	t = new Tile;
	t->SetTexture(L"Pixel.png");
	t->visible = false;

	for (int i = 0; i < 50; ++i)
	{
		for (int l = 0; l < 50; ++l)
		{
			tile[i][l] = new Tile;
			tile[i][l]->SetTexture(t->texture);
			tile[i][l]->position = { 321 + (float)i * 13, 41 + (float)l * 13 };
			tile[i][l]->indexX = i;
			tile[i][l]->indexY = l;
			tile[i][l]->state = NONE;
			PollutionPlace.emplace_back(tile[i][l]->position);
		}
	}

	NodapInit(rand() % 31 + 10, rand() % 31 + 10);
	NodapInit(rand() % 31 + 10, rand() % 31 + 10);
	NodapInit(rand() % 31 + 10, rand() % 31 + 10);
	SetEdge();
}

void Board::BoardUpdate()
{
	delay += Time::deltaTime;
	if (delay > PlayerMNG::GetIns()->player->speed && !PlayerMNG::GetIns()->player->isDie)
	{
		MovePlayerState();
		delay = 0;
	}
	SetColorByState();

	CheckCut();
	IfIsCut();
	CheckFill();
}

void Board::DeleteBoard()
{
	clearPlace1.clear();
	clearPlace2.clear();

	AllClearPlaceExceptEDGE.clear();
	AllClearPlace.clear();
	pathlist.clear();
	AllClearPlacePos.clear();
	PollutionPlace.clear();
	//score = 0;

	PlayerPosX = 0;
	PlayerPosY = 0;
}

void Board::SetEdge()
{
for (int i = 0; i < 50; ++i) // �ܰ��� state EDGE�� ����
{
	tile[i][0]->state = EDGE;
	tile[i][49]->state = EDGE;
	tile[0][i]->state = EDGE;
	tile[49][i]->state = EDGE;
}

for (int i = 0; i < 50; i++)
{
	for (int j = 0; j < 50; j++)
	{
		if (tile[i][j]->state == EDGE)
		{
			AllClearPlace.push_back(tile[i][j]);
			AllTile.remove(tile[i][j]);
		}
			AllClearPlacePos.push_back(tile[i][j]->position);
	}
}
}

void Board::CheckCut()
{
	if (isCut == false && tile[PlayerPosX][PlayerPosY]->state == NONE)
	{
		isCut = true;
		StartPlayerCutX = PlayerBackPosX;
		StartPlayerCutY = PlayerBackPosY;
	}
}

void Board::IfIsCut()
{
	if (isCut)
	{
		if (tile[PlayerBackPosX][PlayerBackPosY]->state == NONE)
		{
			tile[PlayerBackPosX][PlayerBackPosY]->state = PATH; // ĳ���� �̵��� �� ���� ��ġ�� PATH state�� ����
			tile[PlayerBackPosX][PlayerBackPosY]->direction = PlayerMNG::GetIns()->player->dir; // pixel�� direction�� player�� direction���� ����?
			pathlist.emplace_back(tile[PlayerBackPosX][PlayerBackPosY]); // path list ���ʿ� ����?
		}

		for (auto ik : EnemyMNG::GetIns()->enemylist)
		{
			RECT r;
			if (tile[PlayerPosX][PlayerPosY]->state == NODAP || tile[PlayerPosX][PlayerPosY]->state == PATH || IntersectRect(&r, &ik->GetRect(), &tile[PlayerPosX][PlayerPosY]->GetRect()) || ColToEnemyByPath()) // �÷��̾��� ��ġ�� PATH ���� ������
			{
				if (ItemMNG::GetIns()->isMoojuk == false)
				{
					PlayerPosX = StartPlayerCutX; // �÷��̾��� ��ġ�� �׸��� ������ ��ġ�� �̵�
					PlayerPosY = StartPlayerCutY;
					isCut = false; // Ŀ�� �ߴ�

					// PATH �ʱ�ȭ
					for (auto& it : pathlist)
					{
						it->state = NONE;
					}
					pathlist.clear();
					ItemMNG::GetIns()->isShield = false;
				}
			}
		}
	}
}

void Board::CheckFill()
{
	if (isCut && (tile[PlayerPosX][PlayerPosY]->state == EDGE) || (tile[PlayerPosX][PlayerPosY]->state == CLEARED)) // Ŀ�� �۾��߿� �÷��̾ ���� �����ų� �̹� �߸� ���� ����
	{
		isCut = false; // Ŀ�� �۾� �ߴ�
		for (auto& it : pathlist) // path list Ž��
		{

			if (it->direction == UPDOWN) //���� �������� �� Ŀ���� ���� -> �翷�� �������
			{
				// �翷�� ��������� �� �ȼ��� �߽����� Ž��
				if ((tile[it->indexX + 1][it->indexY]->state == NONE) &&
					(tile[it->indexX - 1][it->indexY]->state == NONE))
				{
					check = true;
				}
			}
			else //������ �������� �� Ŀ���� ���� -> ���Ʒ��� �������
			{
				// ���Ʒ��� ��������� �� �ȼ��� �߽����� Ž��
				if ((tile[it->indexX][it->indexY + 1]->state == NONE) &&
					(tile[it->indexX][it->indexY - 1]->state == NONE))
				{
					check = true;
				}
			}

			if (check == true) //Ž������� true��
			{
				if (it->direction == UPDOWN) // ���Ʒ��Τ� �� �� Ŀ���� ������
				{
					FloodFill(it->indexX + 1, it->indexY, 0); //floodfill
					FloodFill(it->indexX - 1, it->indexY, 1); //floodfill
				}
				else
				{
					FloodFill(it->indexX, it->indexY + 1, 0); //floodfill
					FloodFill(it->indexX, it->indexY - 1, 1); //floodfill
				}

				ClearPlace(clearPlace1, clearPlace2);
				ClearPlace(clearPlace2, clearPlace1);

				EnemyMNG::GetIns()->killEnemy();

				clearPlace1.clear(); // list �ʱ�ȭ
				clearPlace2.clear();

				for (int i = 0; i < 50; ++i)
				{
					for (int j = 0; j < 50; ++j)
					{
						clearedTile[i][j] = false;
						if (tile[i][j]->state == NONE)
							PollutionPlace.emplace_back(tile[i][j]->position);
						else if(tile[i][j]->state == CLEARED)
							AllClearPlacePos.push_back(tile[i][j]->position);
					}
				}
				break;
			}
		}
		check = false;

		for (auto& it : pathlist) // �÷��̾ ���������� clear
		{
			it->state = CLEARED;
			AllClearPlaceExceptEDGE.push_back(it);
			AllClearPlace.push_back(it);
		}
		pathlist.clear();
	}
}

bool Board::ClearPlace(list<Tile*> checkplace, list<Tile*> clearplace)
{
	RECT r;
	for (auto ik : checkplace)
	{
		if ((ik->GetRect().right > Boss::GetIns()->boss->position.x) && (ik->GetRect().left < Boss::GetIns()->boss->position.x) && (ik->GetRect().top < Boss::GetIns()->boss->position.y) && (ik->GetRect().bottom > Boss::GetIns()->boss->position.y))
		{
			for (auto im : clearplace)
			{
				im->state = CLEARED;
				AllClearPlace.push_back(im);
				AllClearPlaceExceptEDGE.push_back(im);
				score += 10;
				Sound::GetIns()->playSound(L"eat", false, -1000);
			}
		}
	}
	PollutionPlace.clear();
	return true;
}

void Board::SetColorByState()
{
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (tile[i][j]->state == EDGE)
			{
				tile[i][j]->color = D3DCOLOR_RGBA(100,0,100,255);
			}
			else if (tile[i][j]->state == PATH)
			{
				tile[i][j]->color = D3DCOLOR_RGBA(0, 255, 255, 255);
			}
			else if (tile[i][j]->state == CLEARED)
			{
				tile[i][j]->color = D3DCOLOR_RGBA(100, 100, 255, tile[i][j]->alpha);
				if (tile[i][j]->alpha > 10)
					tile[i][j]->alpha -= 10;
			}
			else if (tile[i][j]->state == NONE)
			{
				tile[i][j]->color = D3DCOLOR_RGBA(90, 90, 90, 255);
			}
			else
			{
				tile[i][j]->color = D3DCOLOR_RGBA(0, 0, 0, 255);
			}
		}
	}
}

void Board::MovePlayerState()
{
	if (DXUTIsKeyDown('W'))
	{
		PlayerBackPosX = PlayerPosX; // backpos = playerpos
		PlayerBackPosY = PlayerPosY;
		PlayerPosY -= 1; // backpos �۾� �� �̵��� ������ġ�� ������
		if (PlayerPosY < 0)
			PlayerPosY = 0;
	}
	else if (DXUTIsKeyDown('S'))
	{
		PlayerBackPosX = PlayerPosX;
		PlayerBackPosY = PlayerPosY;
		PlayerPosY += 1;
		if (PlayerPosY > 49)
			PlayerPosY = 49;
	}
	else if (DXUTIsKeyDown('A'))
	{
		PlayerBackPosX = PlayerPosX;
		PlayerBackPosY = PlayerPosY;
		PlayerPosX -= 1;
		if (PlayerPosX < 0)
			PlayerPosX = 0;
	}
	else if (DXUTIsKeyDown('D'))
	{
		PlayerBackPosX = PlayerPosX;
		PlayerBackPosY = PlayerPosY;
		PlayerPosX += 1;
		if (PlayerPosX > 49)
			PlayerPosX = 49;
	}
	PlayerMNG::GetIns()->player->position.x = 320 + PlayerPosX * 13;
	PlayerMNG::GetIns()->player->position.y = 40 + PlayerPosY * 13;
}


bool Board::ColToEnemyByPath()
{
	RECT r;
	for (auto it : pathlist)
	{
		for (auto il : EnemyMNG::GetIns()->enemylist)
		{
			if (IntersectRect(&r, &it->GetRect(), &il->GetRect()) || IntersectRect(&r, &it->GetRect(), &Boss::GetIns()->boss->GetRect()))
			{
				if (ItemMNG::GetIns()->isShield == false && ItemMNG::GetIns()->isMoojuk == false)
				{
					PlayerMNG::GetIns()->player->hp--;
					Global::GetIns()->shakeScreen = 5000;
					Sound::GetIns()->playSound(L"HitEft", false, -1000);
				}
				return true;
			}
		}
	}
}

float Board::Lerp(float value1, float value2, float amount)
{
	return float((value1 + (value2 - value1) * amount));
}

void Board::NodapInit(int CenterX, int CenterY)
{
	tile[CenterX][CenterY]->state = NODAP;
	tile[CenterX-1][CenterY]->state = NODAP;
	tile[CenterX-2][CenterY]->state = NODAP;
	tile[CenterX][CenterY-1]->state = NODAP;
	tile[CenterX-1][CenterY-1]->state = NODAP;
	tile[CenterX-2][CenterY-1]->state = NODAP;
	//NodapSprite = new Sprite(L"NODAP.png", { 285 + (float)CenterX * 14, 10 + (float)CenterY * 14.2f }, { 0.07,0.07 });
}

