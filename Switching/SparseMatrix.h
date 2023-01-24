#pragma once

#include "common.h"

class SparseMatrixComplex {
public:
    dim3 size;
    int* row;
    int* col;
    complex* ele;
    int elementCount;

    SparseMatrixComplex(dim3 size, int* row, int* col, complex* ele, int elementCount) {
        this->size = size;
        this->row = row;
        this->col = col;
        this->ele = ele;
        this->elementCount = elementCount;
    }

    SparseMatrixComplex(dim3 size, int elementCount) {
        this->size = size;
        this->row = 0;
        this->col = 0;
        this->ele = 0;
        this->elementCount = elementCount;
    }
};

class SparseMatrixReal {
public:
    dim3 size;
    int* row;
    int* col;
    real* ele;
    int elementCount;

    SparseMatrixReal(dim3 size, int* row, int* col, real* ele, int elementCount) {
        this->size = size;
        this->row = row;
        this->col = col;
        this->ele = ele;
        this->elementCount = elementCount;
    }

    SparseMatrixReal(dim3 size, int elementCount) {
        this->size = size;
        this->row = 0;
        this->col = 0;
        this->ele = 0;
        this->elementCount = elementCount;
    }
};
    
