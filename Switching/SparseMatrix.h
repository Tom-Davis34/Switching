#pragma once

#include "common.h"
#include <iostream>


class SparseMatrixComplex {
public:
    int* row;
    int* col;
    cmplx* ele;
    int elementCount;

    SparseMatrixComplex( int* row, int* col, cmplx* ele, int elementCount) {
        this->row = row;
        this->col = col;
        this->ele = ele;
        this->elementCount = elementCount;
    }

    SparseMatrixComplex( int elementCount) {
        this->row = 0;
        this->col = 0;
        this->ele = 0;
        this->elementCount = elementCount;
    }
};

class SparseMatrixReal {
public:
    int rowCount;
    vector<int> row;
    vector<int> col;
    vector<float> ele;
    int elementCount;

    SparseMatrixReal(int rowCount, vector<int> row, vector<int> col, vector<float> ele, int elementCount) {
        this->rowCount = rowCount;
        this->row = row;
        this->col = col;
        this->ele = ele;
        this->elementCount = elementCount;
    }

    vector<float> mul(vector<float> vec) {
        vector<float> newVec = vector<float>(vec.size());

        for (int rowId = 0; rowId < rowCount; rowId++) {
            float result = 0;
            int eleIndex = row[rowId];
            int lastEleIndex = row[rowId + 1];

            while (eleIndex < lastEleIndex) {
                int colIndex = col[eleIndex];
                result += vec[colIndex] * ele[eleIndex];
                eleIndex++;
            }

            newVec[rowId] = result;
        }

        return newVec;
    }

    float get(int rowId, int colId) {
        int eleIndex = row[rowId];
        int lastEleIndex = row[rowId + 1];

        while (eleIndex < lastEleIndex) {
            int colIndex = col[eleIndex];
            if (colIndex == colId) {
                return ele[eleIndex];
            }

            eleIndex++;
        }

        return 0.0f;
    }


    void logSquareFull() {

        for (size_t i = 0; i < rowCount; i++)
        {
            for (size_t j = 0; j < rowCount; j++)
            {
                //char buff[100];
                //snprintf(buff, sizeof(buff), "%s", "Hello");

                std::printf("%3.3f, ", get(i,j));
            }

            std::cout << "\n";
        }
    }
};
    
