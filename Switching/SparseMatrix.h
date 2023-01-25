#pragma once

#include "common.h"

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
    int* row;
    int* col;
    float* ele;
    int elementCount;

    SparseMatrixReal( int* row, int* col, float* ele, int elementCount) {
        this->row = row;
        this->col = col;
        this->ele = ele;
        this->elementCount = elementCount;
    }

    SparseMatrixReal(int elementCount) {
        this->row = 0;
        this->col = 0;
        this->ele = 0;
        this->elementCount = elementCount;
    }
};
    
