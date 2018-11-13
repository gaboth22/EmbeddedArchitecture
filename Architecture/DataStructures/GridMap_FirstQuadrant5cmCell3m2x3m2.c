#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "utils.h"
#include "Uassert.h"

static void SetOrClearBitAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y,
    uint8_t value)
{
    if(x > 63 || y > 63 || x < 0 || y < 0)
        return;

    if(value >0)
    {
        instance->grid[y] |= (0x8000000000000000 >> x);
    }
    else
    {
        instance->grid[y] &=  ~(0x8000000000000000 >> x);
    }
}

void GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y)
{
    SetOrClearBitAtIndex(instance, x, y, 1);
}

void GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y)
{
    SetOrClearBitAtIndex(instance, x, y, 0);
}

uint8_t GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y)
{
    Uassert((x < 63));
    Uassert((y < 63));

    return (((instance->grid[y] << x) & 0x8000000000000000) > 0 ? 1: 0);
}

void GridMap_FirstQuadrant5cmCell3m2x3m2_GetIndicesOfAdjacents(
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance,
    int8_t x,
    int8_t y,
    uint8_t *indices)
{
    /* N */
    *indices = x;
    *(indices + 1) = ((y + 1) > 63) ? UINT8_MAX : (y + 1);

    /* W */
    *(indices + 2) = ((x - 1) < 0) ? UINT8_MAX : (x - 1);
    *(indices + 3) = y;

    /* E */
    *(indices + 4) = ((x + 1) > 63) ? UINT8_MAX : (x + 1);
    *(indices + 5) = y;

    /* S */
    *(indices + 6) = x;
    *(indices + 7) = ((y - 1) < 0) ? UINT8_MAX : (y - 1);
}

void GridMap_FirstQuadrant5cmCell3m2x3m2_Init(GridMap_FirstQuadrant5cmCell3m2x3m2_t *instance)
{
    uint8_t i = 0;
    for(i = 0; i < 64; i++)
    {
        instance->grid[i] = 0;
    }
}
