#include "VoronoiDiagram.h"
#include <random>

float VoronoiDiagram::getDistanceToEdge(float posX, float posY)
{
	int currentSectorX = (int)std::floor(posX / spaceScale);
	int currentSectorY = (int)std::floor(posY / spaceScale);

	float minDist = 1e9f;
	float closestNodeX = 0.0f, closestNodeY = 0.0f;
	int closestSectorX = 0, closestSectorY = 0;

	// »щем ближайший узел в радиусе 3х3 секторов
	for (int j = -1; j <= 1; ++j) {
		for (int i = -1; i <= 1; ++i) {
			int checkSectorX = currentSectorX + i;
			int checkSectorY = currentSectorY + j;
			float nX, nY;
			getNodePosition(checkSectorX, checkSectorY, nX, nY);

			float dist = std::sqrt((posX - nX) * (posX - nX) + (posY - nY) * (posY - nY));
			if (dist < minDist) {
				minDist = dist;
				closestNodeX = nX;
				closestNodeY = nY;
				closestSectorX = checkSectorX;
				closestSectorY = checkSectorY;
			}
		}
	}

	float minDist2 = 1e9f;
	float secondNodeX = 0.0f, secondNodeY = 0.0f;

	// »щем второй по близости узел в расширенном радиусе 5х5 секторов
	for (int j = -2; j <= 2; ++j) {
		for (int i = -2; i <= 2; ++i) {
			int checkSectorX = currentSectorX + i;
			int checkSectorY = currentSectorY + j;

			if (checkSectorX == closestSectorX && checkSectorY == closestSectorY) continue;

			float nX, nY;
			getNodePosition(checkSectorX, checkSectorY, nX, nY);

			float dist = std::sqrt((posX - nX) * (posX - nX) + (posY - nY) * (posY - nY));
			if (dist < minDist2) {
				minDist2 = dist;
				secondNodeX = nX;
				secondNodeY = nY;
			}
		}
	}

	// ¬ычисл€ем рассто€ние до серединного перпендикул€ра между двум€ узлами
	float midX = (closestNodeX + secondNodeX) * 0.5f;
	float midY = (closestNodeY + secondNodeY) * 0.5f;

	float dirX = secondNodeX - closestNodeX;
	float dirY = secondNodeY - closestNodeY;

	float len = std::sqrt(dirX * dirX + dirY * dirY);

	if (len == 0.0f) return 0.0f;
	dirX /= len;
	dirY /= len;

	float distToEdge = (posX - midX) * dirX + (posY - midY) * dirY;
	return std::abs(distToEdge);
}

int VoronoiDiagram::getRegionID(float posX, float posY)
{
	int currentSectorX = (int)std::floor(posX / spaceScale);
	int currentSectorY = (int)std::floor(posY / spaceScale);

	float minDist = 1e9f;
	int closestRegionID = 0;

	// »щем ближайший узел в радиусе 3х3 секторов
	for (int j = -1; j <= 1; ++j) {
		for (int i = -1; i <= 1; ++i) {
			int checkSectorX = currentSectorX + i;
			int checkSectorY = currentSectorY + j;
			float nX, nY;
			getNodePosition(checkSectorX, checkSectorY, nX, nY);

			float dist = std::sqrt((posX - nX) * (posX - nX) + (posY - nY) * (posY - nY));
			if (dist < minDist) {
				minDist = dist;
				closestRegionID = checkSectorX * 73856093 ^ checkSectorY * 19349663;
			}
		}
	}
	return std::abs(closestRegionID);
}

void VoronoiDiagram::getScaleNodeOffset(int sectorX, int sectorY, float& outX, float& outY)
{
	// веро€тно нужно сделать xor с секторами и сидом
	rng.seed(seed);
	outX = distribution(rng);
	outY = distribution(rng);
}

void VoronoiDiagram::getNodePosition(int sectorX, int sectorY, float& nodeX, float& nodeY)
{
	float offsetX, offsetY;
	getScaleNodeOffset(sectorX, sectorY, offsetX, offsetY);

	nodeX = (float)sectorX * spaceScale + offsetX * spaceScale;
	nodeY = (float)sectorY * spaceScale + offsetY * spaceScale;
}
