#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <stddef.h>
#include <stdbool.h>
#include "../DynamicArray/dynamic_array.h"

/* -------------------------------------------------------------------------
 * Enums, Structs, and Type Definitions
 * ------------------------------------------------------------------------- */

// Basic type enumeration for Series columns.
typedef enum {
    DF_INT,
    DF_DOUBLE,
    DF_STRING
} ColumnType;

// A Series represents a single column: a name, a type, and a DynamicArray for data.
typedef struct {
    char*        name;
    ColumnType   type;
    DynamicArray data;     // Each element in 'data' is a copy of the item (int/double/char array).
} Series;

// A DataFrame holds an array of Series (columns) plus a row count.
typedef struct {
    DynamicArray columns;  // DynamicArray of Series
    size_t       nrows;    // Number of rows (all series must be the same length)
} DataFrame;

/* -------------------------------------------------------------------------
 * Series Functions
 * ------------------------------------------------------------------------- */

// Initialize a Series with a name and type.
void seriesInit(Series* s, const char* name, ColumnType type);

// Free internal memory in a Series.
void seriesFree(Series* s);

// Get the size (number of elements) of the Series.
size_t seriesSize(const Series* s);

// Add integer data to the Series. (Only valid if s->type == DF_INT)
void seriesAddInt(Series* s, int value);

// Add double data to the Series. (Only valid if s->type == DF_DOUBLE)
void seriesAddDouble(Series* s, double value);

// Add string data to the Series. (Only valid if s->type == DF_STRING)
// Internally copies the string.
void seriesAddString(Series* s, const char* str);

// Retrieve an integer from a Series at a given row index.
bool seriesGetInt(const Series* s, size_t index, int* outValue);

// Retrieve a double from a Series at a given row index.
bool seriesGetDouble(const Series* s, size_t index, double* outValue);

// Retrieve a string from a Series at a given row index (copies into outStr).
// Caller is responsible for freeing *outStr if successful.
bool seriesGetString(const Series* s, size_t index, char** outStr);

// Utility print for the entire Series (for demonstration).
void seriesPrint(const Series* s);

/* -------------------------------------------------------------------------
 * DataFrame Functions
 * ------------------------------------------------------------------------- */

// Initialize a DataFrame (no columns, 0 rows).
void dfInit(DataFrame* df);

// Free internal memory in the DataFrame (including all Series).
void dfFree(DataFrame* df);

// Add a Series to the DataFrame. 
// If the DataFrame is empty, it just adds it and sets df->nrows = seriesSize.
// If not empty, the Series must have the same length as existing columns.
bool dfAddSeries(DataFrame* df, const Series* s);

// Number of columns in the DataFrame.
size_t dfNumColumns(const DataFrame* df);

// Number of rows in the DataFrame.
size_t dfNumRows(const DataFrame* df);

// Get a pointer to the Series at a given column index (NULL if out of range).
const Series* dfGetSeries(const DataFrame* df, size_t colIndex);

// Add a single row to the DataFrame by specifying data for each column.
//
// data[i] should be a pointer to an int/double/char* depending on the column type.
// Example usage for row insertion with 2 columns:
//    const void* rowData[2];
//    int valInt = 10;
//    double valDouble = 3.14;
//    rowData[0] = &valInt; 
//    rowData[1] = &valDouble;
//    dfAddRow(&df, rowData);
//
bool dfAddRow(DataFrame* df, const void** rowData);

// Print the entire DataFrame in a basic table format.
void dfPrint(const DataFrame* df);

// Print the first N rows of the DataFrame.
void dfHead(const DataFrame* df, size_t n);

// Print the last N rows of the DataFrame.
void dfTail(const DataFrame* df, size_t n);

// Example of a simple "describe" function, printing min, max, count, and mean
// for numeric columns.
void dfDescribe(const DataFrame* df);

void dfPlot(const DataFrame* df,
    size_t xColIndex,
    const size_t* yColIndices,
    size_t yCount,
    const char* plotType,
    const char* outputFile);

#endif // DATAFRAME_H
