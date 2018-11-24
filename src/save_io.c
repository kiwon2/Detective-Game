﻿#include "save_io.h"
#include "main.h"

int InitSave(Player **save)
{
	*save = (Player *)calloc(SAVESIZE, sizeof(Player));
	if (save == NULL)
	{
		return FAIL;
	}
	ExecuteFree(*save);
	return SUCCESS;
}

int GetClearedQuizCount(Player save[], int id)
{
	int i = 0, result = 0;

	for (i = 0; i < QUIZCOUNT; i++)
	{
		result += (save[id].cleared & (1 << i)) && 1;
	}
	return result;
}

void PrintSaveList(Player save[])
{
	int i = 0;

	CLS;
	for (i = 0; i < SAVESIZE; i++)
	{
		printf("%d번째 세이브\n", i);
		printf("맞힌 퀴즈 개수: %d\n", GetClearedQuizCount(save, i));
		printf("플레이 시간: %d\n\n", save[i].playTime);
	}
	return;
}

int Save(Player *player, Player save[], int id)
{
	FILE *saveFile = fopen("save.sav", "wb");
	uint8_t hash = 0;
	int i = 0;

	if (player != NULL && save != NULL)
	{
		save[id].cleared = player->cleared;
		save[id].playTime = player->playTime;
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}

	if (saveFile == EOF)
	{
		return FAIL;
	}
	sha256(save, sizeof(Player) * SAVESIZE, &hash);
	
	for (i = 0; i < SAVESIZE; i++)
	{
		fprintf("%d %d\n", save[i].cleared, save[i].playTime);
	}
	fprintf("%d", hash);
	close(saveFile);
	return SUCCESS;
}

int LoadFromFile(Player save[])
{
	FILE *saveFile = fopen("save.sav", "rb");
	int i = 0;
	uint8_t fileHash = 0, newHash = 0;

	if (saveFile == NULL)
	{
		return FAIL;
	}

	for (i = 0; i < SAVESIZE; i++)
	{
		if (fscanf(saveFile, "%d %d\n", &(save[i].cleared), &(save[i].playTime)) <= 0)
		{
			fclose(saveFile);
			return FAIL;
		}
	}
	if (fscanf("%d", &fileHash) <= 0)
	{
		fclose(saveFile);
		return FAIL;
	}
	fclose(saveFile);
	sha256(save, sizeof(Player) * SAVESIZE, &newHash);
	if (fileHash != newHash)
	{
		return FAIL;
	}
	return SUCCESS;
}

void ExecuteFree(Player save[])
{
	static int count = 0;
	static Player *saved = NULL;

	if (count == 0)
	{
		saved = save;
		count = 1;
	}
	else
	{
		free(saved);
	}
	return;
}