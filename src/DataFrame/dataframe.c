#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dataframe.h"
#include "../DynamicArray/dynamic_array.h"

/* -------------------------------------------------------------------------
 * Helper: Duplicate string safely
 * ------------------------------------------------------------------------- */
static char* safeStrdup(const char* src) {
    if (!src) return NULL;
    size_t len = strlen(src) + 1;
    char* copy = (char*)malloc(len);
    if (!copy) return NULL;
    memcpy(copy, src, len);
    return copy;
}

/* -------------------------------------------------------------------------
 * Series Functions
 * ------------------------------------------------------------------------- */

void seriesInit(Series* s, const char* name, ColumnType type) {
    if (!s) return;

    s->name = safeStrdup(name);
    s->type = type;
    daInit(&s->data, 8); // arbitrary initial capacity
}

/************************************************
 * seriesFree -- do NOT manually free each string.
 ************************************************/
void seriesFree(Series* s) {
    if (!s) return;

    // Just free the DynamicArray as a whole:
    daFree(&s->data);

    // Free the series name
    free(s->name);
    s->name = NULL;
}




size_t seriesSize(const Series* s) {
    if (!s) return 0;
    return daSize(&s->data);
}

void seriesAddInt(Series* s, int value) {
    if (!s || s->type != DF_INT) return;
    daPushBack(&s->data, &value, sizeof(int));
}

void seriesAddDouble(Series* s, double value) {
    if (!s || s->type != DF_DOUBLE) return;
    daPushBack(&s->data, &value, sizeof(double));
}


void seriesAddString(Series* s, const char* str) {
    if (!s || s->type != DF_STRING || !str) return;

    // Let daPushBack handle the copy:
    size_t len = strlen(str) + 1;
    daPushBack(&s->data, str, len);

    // No extra malloc/free needed
    // Because daPushBack does:
    //   1) malloc(len)
    //   2) memcpy(...)
    // so the string is fully owned by the dynamic array.
}

bool seriesGetInt(const Series* s, size_t index, int* outValue) {
    if (!s || s->type != DF_INT || !outValue) return false;
    if (index >= daSize(&s->data)) return false;
    const int* valPtr = (const int*)daGet(&s->data, index);
    if (!valPtr) return false;
    *outValue = *valPtr;
    return true;
}

bool seriesGetDouble(const Series* s, size_t index, double* outValue) {
    if (!s || s->type != DF_DOUBLE || !outValue) return false;
    if (index >= daSize(&s->data)) return false;
    const double* valPtr = (const double*)daGet(&s->data, index);
    if (!valPtr) return false;
    *outValue = *valPtr;
    return true;
}

bool seriesGetString(const Series* s, size_t index, char** outStr) {
    if (!s || s->type != DF_STRING || !outStr) return false;
    if (index >= daSize(&s->data)) return false;
    const char* valPtr = (const char*)daGet(&s->data, index);
    if (!valPtr) return false;
    // Return a fresh copy to the caller
    *outStr = safeStrdup(valPtr);
    return (*outStr != NULL);
}

void seriesPrint(const Series* s) {
    if (!s) return;
    printf("Series \"%s\" (", s->name);
    switch (s->type) {
        case DF_INT:    printf("int");    break;
        case DF_DOUBLE: printf("double"); break;
        case DF_STRING: printf("string"); break;
    }
    printf("), size = %zu\n", seriesSize(s));

    // Print the contents
    for (size_t i = 0; i < seriesSize(s); i++) {
        switch (s->type) {
            case DF_INT: {
                int val;
                if (seriesGetInt(s, i, &val)) {
                    printf("  [%zu] %d\n", i, val);
                }
                break;
            }
            case DF_DOUBLE: {
                double val;
                if (seriesGetDouble(s, i, &val)) {
                    printf("  [%zu] %f\n", i, val);
                }
                break;
            }
            case DF_STRING: {
                char* str = NULL;
                if (seriesGetString(s, i, &str)) {
                    printf("  [%zu] \"%s\"\n", i, str);
                    free(str);
                }
                break;
            }
        }
    }
}

/* -------------------------------------------------------------------------
 * DataFrame Functions
 * ------------------------------------------------------------------------- */

void dfInit(DataFrame* df) {
    if (!df) return;
    daInit(&df->columns, 4); // Start with capacity 4 columns
    df->nrows = 0;
}

void dfFree(DataFrame* df) {
    if (!df) return;
    // Free all Series inside the DataFrame
    for (size_t i = 0; i < daSize(&df->columns); i++) {
        Series* s = (Series*)daGetMutable(&df->columns, i);
        seriesFree(s);
    }
    // Free the array of columns
    daFree(&df->columns);
    df->nrows = 0;
}

bool dfAddSeries(DataFrame* df, const Series* s) {
    if (!df || !s) return false;

    // If no existing columns, set nrows = seriesSize(s) and just add.
    if (daSize(&df->columns) == 0) {
        df->nrows = seriesSize(s);
        // We need to copy the Series struct as well as re-copy internal data (since it's a separate Series).
        // Easiest approach: create a local copy by first initializing a new Series, then pushing all elements.
        Series newSeries;
        seriesInit(&newSeries, s->name, s->type);
        // Copy over all elements from s:
        for (size_t i = 0; i < seriesSize(s); i++) {
            switch (s->type) {
                case DF_INT: {
                    int val;
                    seriesGetInt(s, i, &val);
                    seriesAddInt(&newSeries, val);
                } break;
                case DF_DOUBLE: {
                    double val;
                    seriesGetDouble(s, i, &val);
                    seriesAddDouble(&newSeries, val);
                } break;
                case DF_STRING: {
                    char* str = NULL;
                    if (seriesGetString(s, i, &str)) {
                        seriesAddString(&newSeries, str);
                        free(str);
                    }
                } break;
            }
        }
        daPushBack(&df->columns, &newSeries, sizeof(Series));
        return true;
    } else {
        // We have existing columns. Check that the new series has the same size.
        if (seriesSize(s) != df->nrows) {
            fprintf(stderr, 
                "Error: new Series '%s' has %zu rows; existing DataFrame has %zu rows.\n", 
                s->name, seriesSize(s), df->nrows);
            return false;
        }
        // Copy series into a new local instance
        Series newSeries;
        seriesInit(&newSeries, s->name, s->type);
        for (size_t i = 0; i < seriesSize(s); i++) {
            switch (s->type) {
                case DF_INT: {
                    int val;
                    seriesGetInt(s, i, &val);
                    seriesAddInt(&newSeries, val);
                } break;
                case DF_DOUBLE: {
                    double val;
                    seriesGetDouble(s, i, &val);
                    seriesAddDouble(&newSeries, val);
                } break;
                case DF_STRING: {
                    char* str = NULL;
                    if (seriesGetString(s, i, &str)) {
                        seriesAddString(&newSeries, str);
                        free(str);
                    }
                } break;
            }
        }
        daPushBack(&df->columns, &newSeries, sizeof(Series));
        return true;
    }
}

size_t dfNumColumns(const DataFrame* df) {
    if (!df) return 0;
    return daSize(&df->columns);
}

size_t dfNumRows(const DataFrame* df) {
    if (!df) return 0;
    return df->nrows;
}

const Series* dfGetSeries(const DataFrame* df, size_t colIndex) {
    if (!df) return NULL;
    if (colIndex >= daSize(&df->columns)) return NULL;
    return (const Series*)daGet(&df->columns, colIndex);
}

bool dfAddRow(DataFrame* df, const void** rowData) {
    if (!df || !rowData) return false;

    size_t nCols = dfNumColumns(df);
    if (nCols == 0) {
        // No columns, can't really add a row. Or you could interpret that we do nothing.
        fprintf(stderr, "Error: DataFrame has no columns; can't add row.\n");
        return false;
    }
    // For each column, add the data item of the correct type.
    for (size_t c = 0; c < nCols; c++) {
        Series* s = (Series*)daGetMutable(&df->columns, c);
        if (!s) return false;
        switch (s->type) {
            case DF_INT: {
                const int* valPtr = (const int*)rowData[c];
                if (!valPtr) return false;
                seriesAddInt(s, *valPtr);
            } break;
            case DF_DOUBLE: {
                const double* valPtr = (const double*)rowData[c];
                if (!valPtr) return false;
                seriesAddDouble(s, *valPtr);
            } break;
            case DF_STRING: {
                const char* strPtr = (const char*)rowData[c];
                if (!strPtr) return false;
                seriesAddString(s, strPtr);
            } break;
        }
    }
    df->nrows += 1;
    return true;
}

/* -------------------------------------------------------------------------
 * DataFrame Printing Functions
 * ------------------------------------------------------------------------- */
/*
 * Helper: convert an integer to a string, return the length.
 */
static size_t intToStrLen(int value, char* buffer, size_t bufSize) {
    if (!buffer || bufSize == 0) return 0;
    int len = snprintf(buffer, bufSize, "%d", value);
    return (len > 0) ? (size_t)len : 0;
}

/*
 * Helper: convert a double to string (e.g. "%.3f"), return the length.
 */
static size_t doubleToStrLen(double value, char* buffer, size_t bufSize) {
    if (!buffer || bufSize == 0) return 0;
    int len = snprintf(buffer, bufSize, "%.3f", value);
    return (len > 0) ? (size_t)len : 0;
}

/*
 * Compute the digits needed to display the largest row index (nRows-1).
 */
static size_t computeIndexWidth(size_t nRows) {
    if (nRows <= 1) return 1; // at least 1 char
    size_t maxIndex = nRows - 1;
    size_t width = 0;
    while (maxIndex > 0) {
        maxIndex /= 10;
        width++;
    }
    return width;
}

/*
 * Helper to print a single row (r) with precomputed column widths.
 * If 'ellipsis' is true, we print "..." instead of actual row data
 * (used for the middle row in large DataFrames).
 */
static void printRow(
    const DataFrame* df, 
    size_t rowIndex, 
    size_t idxWidth, 
    const size_t* colWidths, 
    int nCols, 
    int ellipsis
) {
    char tempBuf[256];

    // Print row index (right-aligned).
    printf("%*zu  ", (int)idxWidth, rowIndex);

    for (int c = 0; c < nCols; c++) {
        const Series* s = dfGetSeries(df, c);
        if (!s) {
            // Should not happen, but in case
            printf("???  ");
            continue;
        }

        if (ellipsis) {
            // Just print "..." for each column cell
            // Typically pandas leaves it blank or 
            // prints "..." in all columns. We'll do the latter.
            // Right-align or left-align doesn't matter too much here.
            // We'll just center it. For simplicity, let's do left-align.
            // But if the column is wide, let's do it left anyway.
            printf("%-*s  ", (int)colWidths[c], "...");
            continue;
        }

        // Actually print the cell value
        switch (s->type) {
            case DF_INT: {
                int val;
                if (seriesGetInt(s, rowIndex, &val)) {
                    int len = snprintf(tempBuf, sizeof(tempBuf), "%d", val);
                    if (len < 0) len = 0;
                    // Right-align numeric
                    printf("%*s  ", (int)colWidths[c], tempBuf);
                } else {
                    printf("%*s  ", (int)colWidths[c], "?");
                }
            } break;

            case DF_DOUBLE: {
                double val;
                if (seriesGetDouble(s, rowIndex, &val)) {
                    int len = snprintf(tempBuf, sizeof(tempBuf), "%.3f", val);
                    if (len < 0) len = 0;
                    // Right-align numeric
                    printf("%*s  ", (int)colWidths[c], tempBuf);
                } else {
                    printf("%*s  ", (int)colWidths[c], "?");
                }
            } break;

            case DF_STRING: {
                char* str = NULL;
                if (seriesGetString(s, rowIndex, &str)) {
                    // Left-align strings
                    printf("%-*s  ", (int)colWidths[c], str);
                    free(str);
                } else {
                    printf("%-*s  ", (int)colWidths[c], "?");
                }
            } break;
        }
    }
    printf("\n");
}

/*
 * Print the DataFrame in a "pandas-like" style, *only* showing
 * the first 5 and last 5 rows if the dataset is large (over 10 rows).
 * Otherwise, print all rows. Align columns to emulate pandas look.
 */
void dfPrint(const DataFrame* df) {
    if (!df) {
        printf("NULL DataFrame pointer.\n");
        return;
    }

    size_t nCols = dfNumColumns(df);
    size_t nRows = dfNumRows(df);

    // If no columns or rows, just report an empty DataFrame.
    if (nCols == 0 || nRows == 0) {
        printf("Empty DataFrame\n");
        printf("Columns: %zu\n", nCols);
        printf("Index: %zu entries\n", nRows);
        return;
    }

    // 1) Precompute column widths.
    size_t* colWidths = (size_t*)calloc(nCols, sizeof(size_t));
    if (!colWidths) {
        fprintf(stderr, "Memory allocation failed for colWidths.\n");
        return;
    }

    char tempBuf[256];
    for (size_t c = 0; c < nCols; c++) {
        const Series* s = dfGetSeries(df, c);
        if (!s) {
            colWidths[c] = 3; // "???" fallback
            continue;
        }
        // Start with the column name length
        size_t colNameLen = strlen(s->name);
        colWidths[c] = (colNameLen > colWidths[c]) ? colNameLen : colWidths[c];

        // Check data to find maximum width
        for (size_t r = 0; r < nRows; r++) {
            switch (s->type) {
                case DF_INT: {
                    int val;
                    if (seriesGetInt(s, r, &val)) {
                        size_t length = intToStrLen(val, tempBuf, sizeof(tempBuf));
                        if (length > colWidths[c]) {
                            colWidths[c] = length;
                        }
                    }
                } break;

                case DF_DOUBLE: {
                    double val;
                    if (seriesGetDouble(s, r, &val)) {
                        size_t length = doubleToStrLen(val, tempBuf, sizeof(tempBuf));
                        if (length > colWidths[c]) {
                            colWidths[c] = length;
                        }
                    }
                } break;

                case DF_STRING: {
                    char* str = NULL;
                    if (seriesGetString(s, r, &str)) {
                        size_t length = strlen(str);
                        if (length > colWidths[c]) {
                            colWidths[c] = length;
                        }
                        free(str);
                    }
                } break;
            }
        }
    }

    // 2) Compute index width
    size_t idxWidth = computeIndexWidth(nRows);

    // 3) Print the column header line.
    //    pandas typically prints the index column header as blank.
    //    We'll follow that (just right-align the blank for idxWidth).
    printf("%*s  ", (int)idxWidth, "");
    for (size_t c = 0; c < nCols; c++) {
        const Series* s = dfGetSeries(df, c);
        if (s) {
            // Column names are typically left-aligned in pandas
            printf("%-*s  ", (int)colWidths[c], s->name);
        } else {
            printf("%-*s  ", (int)colWidths[c], "???");
        }
    }
    printf("\n");

    // 4) Decide how many rows to print:
    //    If nRows <= 10, print them all.
    //    If nRows > 10, print first 5, a "..." row, and last 5.
    if (nRows <= 10) {
        // Print all rows
        for (size_t r = 0; r < nRows; r++) {
            printRow(df, r, idxWidth, colWidths, (int)nCols, 0);
        }
    } else {
        // Print first 5
        for (size_t r = 0; r < 5; r++) {
            printRow(df, r, idxWidth, colWidths, (int)nCols, 0);
        }
        // Print ellipsis row (no real data, just placeholders)
        // Typically pandas prints empty index or some dotted line. 
        // We'll do the dotted line for the index, and "..." in each column.
        printf("%*s  ", (int)idxWidth, "");
        for (size_t c = 0; c < nCols; c++) {
            // Center or left doesn't matter much for an ellipsis marker.
            // We'll left-align. 
            printf("%-*s  ", (int)colWidths[c], "...");
        }
        printf("\n");
        // Print last 5
        for (size_t r = nRows - 5; r < nRows; r++) {
            printRow(df, r, idxWidth, colWidths, (int)nCols, 0);
        }
    }

    // 5) Clean up
    free(colWidths);
    printf("\n[%zu rows x %zu columns]\n", nRows, nCols);
}

/* -------------------------------------------------------------------------
 * DataFrame Query Functions
 * ------------------------------------------------------------------------- */
void dfHead(const DataFrame* df, size_t n) {
    if (!df) return;
    printf("==== dfHead(%zu) ====\n", n);
    size_t numRows = dfNumRows(df);
    size_t limit = (n < numRows) ? n : numRows;

    printf("(Showing first %zu of %zu rows)\n", limit, numRows);
    for (size_t r = 0; r < limit; r++) {
        printf("Row %zu: ", r);
        size_t nCols = dfNumColumns(df);
        for (size_t c = 0; c < nCols; c++) {
            const Series* s = dfGetSeries(df, c);
            if (!s) continue;
            switch (s->type) {
                case DF_INT: {
                    int val;
                    seriesGetInt(s, r, &val);
                    printf("%s=%d ", s->name, val);
                } break;
                case DF_DOUBLE: {
                    double val;
                    seriesGetDouble(s, r, &val);
                    printf("%s=%.3f ", s->name, val);
                } break;
                case DF_STRING: {
                    char* str = NULL;
                    if (seriesGetString(s, r, &str)) {
                        printf("%s=\"%s\" ", s->name, str);
                        free(str);
                    }
                } break;
            }
        }
        printf("\n");
    }
}

void dfTail(const DataFrame* df, size_t n) {
    if (!df) return;
    printf("==== dfTail(%zu) ====\n", n);
    size_t numRows = dfNumRows(df);
    if (n > numRows) n = numRows;
    size_t start = (numRows > n) ? (numRows - n) : 0;

    printf("(Showing last %zu of %zu rows)\n", n, numRows);
    for (size_t r = start; r < numRows; r++) {
        printf("Row %zu: ", r);
        size_t nCols = dfNumColumns(df);
        for (size_t c = 0; c < nCols; c++) {
            const Series* s = dfGetSeries(df, c);
            if (!s) continue;
            switch (s->type) {
                case DF_INT: {
                    int val;
                    seriesGetInt(s, r, &val);
                    printf("%s=%d ", s->name, val);
                } break;
                case DF_DOUBLE: {
                    double val;
                    seriesGetDouble(s, r, &val);
                    printf("%s=%.3f ", s->name, val);
                } break;
                case DF_STRING: {
                    char* str = NULL;
                    if (seriesGetString(s, r, &str)) {
                        printf("%s=\"%s\" ", s->name, str);
                        free(str);
                    }
                } break;
            }
        }
        printf("\n");
    }
}

void dfDescribe(const DataFrame* df) {
    if (!df) return;
    printf("==== dfDescribe() ====\n");

    size_t nCols = dfNumColumns(df);
    size_t nRows = dfNumRows(df);

    // For each numeric column, compute min, max, sum, etc.
    for (size_t c = 0; c < nCols; c++) {
        const Series* s = dfGetSeries(df, c);
        if (!s) continue;

        if (s->type == DF_INT) {
            // Gather stats for integer column
            if (nRows == 0) {
                printf("Column '%s': no data.\n", s->name);
                continue;
            }
            int minVal, maxVal, tmp;
            double sumVal = 0.0;
            seriesGetInt(s, 0, &minVal);
            maxVal = minVal;
            sumVal = minVal;

            for (size_t r = 1; r < nRows; r++) {
                seriesGetInt(s, r, &tmp);
                if (tmp < minVal) minVal = tmp;
                if (tmp > maxVal) maxVal = tmp;
                sumVal += tmp;
            }
            double meanVal = sumVal / nRows;
            printf("Column '%s' (int): count=%zu, min=%d, max=%d, mean=%.3f\n",
                   s->name, nRows, minVal, maxVal, meanVal);
        }
        else if (s->type == DF_DOUBLE) {
            // Gather stats for double column
            if (nRows == 0) {
                printf("Column '%s': no data.\n", s->name);
                continue;
            }
            double minVal, maxVal, tmp;
            double sumVal = 0.0;
            seriesGetDouble(s, 0, &minVal);
            maxVal = minVal;
            sumVal = minVal;

            for (size_t r = 1; r < nRows; r++) {
                seriesGetDouble(s, r, &tmp);
                if (tmp < minVal) minVal = tmp;
                if (tmp > maxVal) maxVal = tmp;
                sumVal += tmp;
            }
            double meanVal = sumVal / nRows;
            printf("Column '%s' (double): count=%zu, min=%f, max=%f, mean=%.3f\n",
                   s->name, nRows, minVal, maxVal, meanVal);
        }
        else if (s->type == DF_STRING) {
            // For string columns, we'll just print count.
            printf("Column '%s' (string): count=%zu\n", s->name, nRows);
        }
    }
}


/* -------------------------------------------------------------------------
 * DataFrame Plotting Functions
 * ------------------------------------------------------------------------- */

 // Forward declarations of internal helper to read numeric data
static bool getNumericValue(const Series* s, size_t index, double* outVal);

/**
 * A small utility: attempt to retrieve numeric data (int or double)
 * from a Series at a given row. Return as a double.
 */
static bool getNumericValue(const Series* s, size_t index, double* outVal) {
    if (!s || !outVal) return false;
    if (index >= daSize(&s->data)) return false;

    if (s->type == DF_INT) {
        int temp;
        if (!seriesGetInt(s, index, &temp)) return false;
        *outVal = (double)temp;
        return true;
    } else if (s->type == DF_DOUBLE) {
        double temp;
        if (!seriesGetDouble(s, index, &temp)) return false;
        *outVal = temp;
        return true;
    }
    return false;
}

/**
 * dfPlot Implementation (see prototype in dataframe.h)
 */
void dfPlot(const DataFrame* df,
            size_t xColIndex,
            const size_t* yColIndices,
            size_t yCount,
            const char* plotType,
            const char* outputFile)
{
    if (!df) {
        fprintf(stderr, "dfPlot Error: DataFrame is NULL.\n");
        return;
    }
    if (dfNumRows(df) == 0 || dfNumColumns(df) == 0) {
        fprintf(stderr, "dfPlot Error: DataFrame is empty.\n");
        return;
    }
    if (!yColIndices || yCount == 0) {
        fprintf(stderr, "dfPlot Error: Must provide at least one y column.\n");
        return;
    }
    if (!plotType) {
        plotType = "line"; // default
    }

    // Check if xColIndex == (size_t)-1 => use row index as X
    bool useIndexAsX = (xColIndex == (size_t)-1);

    // Validate all Y columns are numeric
    for (size_t i = 0; i < yCount; i++) {
        const Series* s = dfGetSeries(df, yColIndices[i]);
        if (!s) {
            fprintf(stderr, "dfPlot Error: Invalid yCol index %zu.\n", yColIndices[i]);
            return;
        }
        if (s->type != DF_INT && s->type != DF_DOUBLE) {
            fprintf(stderr, 
                    "dfPlot Error: Column '%s' is not numeric (type=%d). Cannot plot.\n", 
                    s->name, (int)s->type);
            return;
        }
    }

    // If using a real column as X, check that it's numeric
    if (!useIndexAsX) {
        const Series* sx = dfGetSeries(df, xColIndex);
        if (!sx) {
            fprintf(stderr, "dfPlot Error: Invalid xCol index %zu.\n", xColIndex);
            return;
        }
        if (sx->type != DF_INT && sx->type != DF_DOUBLE) {
            fprintf(stderr, 
                    "dfPlot Error: X column '%s' is not numeric. Cannot plot.\n",
                    sx->name);
            return;
        }
    }

    // Generate Python code in memory
    const char* pyFilename = "temp_plot.py";  // or use a unique name, or let user specify
    FILE* pyFile = fopen(pyFilename, "w");
    if (!pyFile) {
        fprintf(stderr, "dfPlot Error: Unable to open temp file '%s' for writing.\n", pyFilename);
        return;
    }

    fprintf(pyFile, "import matplotlib.pyplot as plt\n");
    fprintf(pyFile, "import sys\n\n");

    size_t nRows = dfNumRows(df);

    // 1) Create the X array in Python
    if (useIndexAsX) {
        // Use row indices 0..nRows-1
        fprintf(pyFile, "x = [");
        for (size_t r = 0; r < nRows; r++) {
            fprintf(pyFile, "%zu", r);
            if (r < nRows - 1) fprintf(pyFile, ", ");
        }
        fprintf(pyFile, "]\n");
    } else {
        // Use numeric column xColIndex
        const Series* sx = dfGetSeries(df, xColIndex);
        fprintf(pyFile, "x = [");
        for (size_t r = 0; r < nRows; r++) {
            double val = 0.0;
            getNumericValue(sx, r, &val);
            fprintf(pyFile, "%g", val);
            if (r < nRows - 1) fprintf(pyFile, ", ");
        }
        fprintf(pyFile, "]\n");
    }

    // 2) Create each Y array
    //    We'll name them y0, y1, etc., in Python code
    for (size_t i = 0; i < yCount; i++) {
        const Series* s = dfGetSeries(df, yColIndices[i]);
        fprintf(pyFile, "y%zu = [", i);
        for (size_t r = 0; r < nRows; r++) {
            double val = 0.0;
            getNumericValue(s, r, &val);
            fprintf(pyFile, "%g", val);
            if (r < nRows - 1) fprintf(pyFile, ", ");
        }
        fprintf(pyFile, "]\n");
    }

    // 3) Plot commands
    //    For each Y column, do e.g. plt.plot(x, y0, label='colName')
    //    or plt.scatter(...) if plotType == "scatter".
    for (size_t i = 0; i < yCount; i++) {
        const Series* s = dfGetSeries(df, yColIndices[i]);
        if (strcmp(plotType, "scatter") == 0) {
            // scatter
            fprintf(pyFile, "plt.scatter(x, y%zu, label=\"%s\")\n", i, s->name);
        } else {
            // line plot
            fprintf(pyFile, "plt.plot(x, y%zu, label=\"%s\")\n", i, s->name);
        }
    }

    // 4) Title, legend, etc.
    fprintf(pyFile, "plt.xlabel(\"%s\")\n", 
            useIndexAsX ? "Index" : dfGetSeries(df, xColIndex)->name);
    fprintf(pyFile, "plt.ylabel(\"Value\")\n");
    fprintf(pyFile, "plt.title(\"DataFrame Plot\")\n");
    fprintf(pyFile, "plt.legend()\n");

    // 5) If outputFile is not NULL, save to file; else show.
    if (outputFile && strlen(outputFile) > 0) {
        fprintf(pyFile, "plt.savefig(\"%s\")\n", outputFile);
        fprintf(pyFile, "print(\"Plot saved to %s\")\n", outputFile);
    } else {
        fprintf(pyFile, "plt.show()\n");
    }

    fclose(pyFile);

    // 6) Run the Python script
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "python3 \"%s\"", pyFilename);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "dfPlot Warning: system(\"%s\") returned %d.\n", cmd, ret);
    }

    // 7) Optionally remove the temporary script
    //    (comment out if you'd like to keep the script for debugging)
    remove(pyFilename);
}