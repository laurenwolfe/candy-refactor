/*Class description*/

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

#include "array2d.h"

// Create a new Array2D and allocate space on the heap for its contents
Array2D AllocateArray2D(void) {
    Array2D array = (Array2D)malloc(sizeof(Array2DStruct));

    if (array == NULL) return (Array2D)NULL;

    return array;
}

// Clears the Array2D and frees all data elements using client-supplied function
boolean DestroyArray2D(Array2D array, payloadFreeFunctionPtr freeFnPtr) {
    payloadFreeFunctionPtr freeFn;

    if (array == NULL || freeFnPtr == NULL) {
        return false;
    }

    freeFn = freeFnPtr;

    // Free memory
    for (int i = 0; i < array->size; i++) {
        Array_t nextElem = array->data[i];
        freeFn(nextElem);
    }

    free(array->data);
    free(array);

    return true;
}

// Open json file and convert to jansson object.
// Unpack the top level values (rows, columns, array object).
// Pass array object to client function to deserialize.
void Deserialize(Array2D array, char *filepath,
                 ElDeserializeFnPtr deserialize_function) {
    Json_ptr j_obj, data;
    json_error_t error;
    int rows, cols;
    ElDeserializeFnPtr deserialize_fn;

    deserialize_fn = deserialize_function;

    j_obj = json_load_file(filepath, 0, &error);

    if (!j_obj) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return;
    }

    json_unpack(j_obj, "{s:i, s:i, s:o}", "rows", &rows,
                "columns", &cols, "data", &data);

    //insert deserialized values into Array2D
    array->num_rows = rows;
    array->num_cols = cols;
    array->size = (cols * rows);

    array->data = (Array_t)malloc(sizeof(Array_t) * (array->size));

    //Use client-provided function to insert data into array
    deserialize_fn(array, data);

    json_decref(j_obj);
}

// Writes from Array2D structure into a file in JSON format
void Serialize(Array2D array, char *filepath,
               ElSerializeFnPtr serialize_function) {
    Json_ptr data_arr, json_str;
    ElSerializeFnPtr serialize_fn;

    serialize_fn = serialize_function;
    data_arr = serialize_fn(array);

    json_str = json_pack("{s:i, s:i, s:o}", "rows", array->num_rows,
                         "columns", array->num_cols, "data", data_arr);

    json_dump_file(json_str, filepath, 0);

    json_decref(json_str);
}

Array_t GetElement(Array2D array, int idx) {
    if(array == NULL || idx < 0 || idx >= array->size) {
        return (void *)NULL;
    }

    return array->data[idx];
}

// Insert element at the given index
boolean SetElement(Array2D array, int idx, Array_t element) {
    //Validate client-supplied arguments
    if (array == NULL || idx < 0 || idx >= array->size || element == NULL) {
        return false;
    }

    array->data[idx] = element;
    return true;
}

// swaps the data contained in the two index arguments
boolean Swap(Array2D array, int idx1, int idx2) {
    Array_t tmp;

    //Validate client-supplied arguments
    if (array == NULL || idx1 < 0 || idx2 < 0 ||
            idx1 >= array->size || idx2 >= array->size) {
        return false;
    }

    tmp = array->data[idx1];
    array->data[idx1] = array->data[idx2];
    array->data[idx2] = tmp;

    return true;
}

// Returns the total height of the 2D array or -1 if array is NULL
int GetNumRows(Array2D array) {
    // Validate input
    if (array == NULL) return -1;

    return array->num_rows;
}

// Returns the total width of the 2D array or -1 if the array is NULL
int GetNumCols(Array2D array) {
    // Validate input
    if (array == NULL) return -1;

    return array->num_cols;
}

// Returns the total number of elements, or -1 if array is NULL
int GetSize(Array2D array) {
    // Validate input
    if (array == NULL) return -1;

    return array->size;
}
