#ifndef _HW2_ARRAY2D_H_
#define _HW2_ARRAY2D_H_

#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __2D_ARRAY_DEFINITIONS__
#define __2D_ARRAY_DEFINITIONS__

// converts convert row and column to corresponding array index
#define indexofArray(arr, row, col) ((row) * (arr->num_cols) + (col))

// boolean values
#define true 1
#define false 0

#endif // 2D_ARRAY_DEFINITIONS

// [ --- Variable types --- ]
typedef json_t* Json_ptr;
typedef void* Array_t;
typedef int boolean;

// [ --- Struct types --- ]
typedef struct arr_2d {
    Array_t *data;
    int size;
    int num_rows;
    int num_cols;
} Array2DStruct, *Array2D;

// [ --- Function types --- ]
typedef void(*ElDeserializeFnPtr)(Array2D array, Json_ptr data);
typedef Json_ptr(*ElSerializeFnPtr)(Array2D array);
typedef void(*payloadFreeFunctionPtr)(Array_t payload_ptr);

// [ --- Function Declarations --- ]
Array2D AllocateArray2D(void);
boolean DestroyArray2D(Array2D array, payloadFreeFunctionPtr freeFn);

void Deserialize(Array2D array, char *filepath,
                 ElDeserializeFnPtr deserialize_function);

void Serialize(Array2D array, char *filepath,
               ElSerializeFnPtr serialize_function);

boolean GetElement(Array2D array, int idx, Array_t ptr);

boolean SetElement(Array2D array, int idx, Array_t setElem);

boolean Swap(Array2D array, int idx1, int idx2);

int GetNumRows(Array2D array);
int GetNumCols(Array2D array);
int GetSize(Array2D array);

#endif // _HW2_ARRAY2D_H_
