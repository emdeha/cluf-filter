#pragma once


#include "Tile.h"

#include <string>
#include <vector>


struct KeyPair;

class Map
{
private:
	Tile **map;
	KeyPair *keyDoorPairs;

	int rowsCount;
	int colsCount;
	int keyDoorPairsCount;

public:
	Map(); 
	~Map();

	bool LoadMap(const std::string &mapFileName);
	void DebugPrintMap() const;
	
public:
	Tile* GetStartTile() const;
	Tile* GetExitTile() const;
	Tile* GetFirstTileWithSymbol(char symbol) const;
	Tile* GetKeyForDoor(char symbol) const;
	Tile* GetDoorForKey(char symbol) const;
	std::vector<Tile*> GetDoorTiles() const;
	std::vector<std::pair<Tile*, Tile*>> GetKeyDoorPairs() const;

	int GetCols() const;
	int GetRows() const;

	bool IsDoorLocked(char doorSymbol) const;
	bool IsDoor(char symbol) const;
	bool IsKey(char symbol) const;
	bool IsKeyRetrieved(char symbol) const;

	void UnlockDoors();
	void LockDoors();
	void UnlockDoor(char symbol);
	void UnlockDoorWithKey(char symbol);

	void UnsetTilesVisited();

private:
	Tile** AllocateTiles(int rows, int cols);
	KeyPair* AllocateKeyDoorPairs(int size);
	void FreeMap(Tile **&toFree, int rows);
	void FreeKeys(KeyPair *toFree);

	bool ReadMapKeysSize(std::ifstream &mapFile, int &rows, int &cols, int &pairsCount);
	bool ReadMap(std::ifstream &mapFile, Tile **inMap);
	bool ReadKeys(std::ifstream &mapFile, KeyPair *inKeyDoorPairs);

	void SetAsOwnerFor(Tile **ownerMap, int rows, int cols);

private:
	Map(const Map &other);
	Map& operator=(const Map &other);

	friend class Tile;
};


struct KeyPair
{
	char key;
	char door;
	bool isDoorLocked;
};