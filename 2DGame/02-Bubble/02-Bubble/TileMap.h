#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "Texture.h"
#include "ShaderProgram.h"


// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	~TileMap();

	void render() const;
	void free();
	
	int getTileSize() const { return tileSize; }
	glm::vec2 getMapSize() const { return mapSize; }

	// Brick treatment
	vector<vector<int>> getBrickIndex() const { return brickIndex; }
	bool getBrickIndexPosition(int i, int j) const { return brickIndex[i][j] == 2; }


	// QMBlock treatment
	vector<vector<int>> getQMBlockIndex() const { return qmBlockIndex; }
	bool getQMBlockIndexPosition(int i, int j) const { return qmBlockIndex[i][j] == 2; }
	void inactivateQMBlock(int i, int j) { qmBlockIndex[i][j] = 3; }

	// Collision
	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, float* posX) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, float* posX) const;
	bool collisionMoveUp(const glm::vec2 &pos, const glm::ivec2 &size, float *posY);
	bool collisionMoveDown(const glm::vec2 &pos, const glm::ivec2 &size, float *posY) const;
	
private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	int nTiles;
	

	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;

	// Special map blocks index
	vector<vector<int>> brickIndex; // 0 = no brick, 1 = brick, 2 = breaking
	vector<vector<int>> qmBlockIndex; // 0 = no block, 1 = unused block, 2 = used block

};


#endif // _TILE_MAP_INCLUDE


