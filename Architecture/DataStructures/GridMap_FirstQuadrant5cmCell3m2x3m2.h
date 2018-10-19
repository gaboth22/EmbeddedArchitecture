#ifndef GRIDMAP_FIRSTQUADRANT5CMCELL3M2X3M2_H
#define GRIDMAP_FIRSTQUADRANT5CMCELL3M2X3M2_H

#include "types.h"

typedef struct
{
    /*
     * This array represent the grid. Each point is a 5 cm by 5 cm square.
     * The whole array fits a 64*5 cm by 64*5 cm grid. For a total of 3.2 m by 3.2m
     */
    uint64_t grid[64];
} GridMap_FirstQuadrant5cmCell3m2x3m2_t;

/*
 * Sets a bit to 1 at the index given by x and y within the grid
 */
void GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y);

/*
 * Clears a bit to 0 at the index given by x and y within the grid
 */
void GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y);

/*
 * Gets the current value a given cell index
 */
uint8_t GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y);

/*
 * Gets a 4x2 matrix with the indices of the adjacent cells to a cell
 * returned int the input parameter indices
 */
void GridMap_FirstQuadrant5cmCell3m2x3m2_GetIndicesOfAdjacents(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y,
    uint8_t *indices);

/*
 * Initialize grid
 */
void GridMap_FirstQuadrant5cmCell3m2x3m2_Init(GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance);

#endif
