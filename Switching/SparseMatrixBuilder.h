#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "common.h"
#include "SparseMatrix.h"
#include "Circuit.h"
#include "CircuitBreaker.h"
#include "Disconnector.h"
#include <cassert>

class SparseMatrixRealBuilder {
public:
    vector<map<int, float>> rows;

    SparseMatrixRealBuilder(int rowsNum) {
        for (size_t i = 0; i < rowsNum; i++)
        {
            rows.push_back(map<int, float>());
        }
    };


    void plus(int row, int col, float ele) {
        map<int, float> rowMap = rows[row];

        float matrixElement = rows[row][col];
        if (rows[row].find(col) == rows[row].end()) {
            rows[row][col] = ele;
        }
        else {
            rows[row][col] += ele;
        }
    }

    void divide(int row, int col, float ele) {
        map<int, float> rowMap = rows[row];

        float matrixElement = rows[row][col];
        if (rows[row].find(col) == rows[row].end()) {
            rows[row][col] = 1/ele;
        }
        else {
            if (ele != 0.0 ) {
                rows[row][col] = rows[row][col] / ele;
            }
        }
    }

    void set(int row, int col, float ele) {
        map<int, float> rowMap = rows[row];

        float matrixElement = rows[row][col];
        if (rows[row].find(col) == rows[row].end()) {
            rows[row][col] = ele;
        }
        else {
            rows[row][col] = ele;
        }
    }

    SparseMatrixReal build() {
        int num = 0;
        for (int i = 0; i < rows.size(); i++)
        {
            num += rows[i].size();
        }

        int cursor = 0;

        vector<int>* rowVector = new vector<int>(rows.size() + 1);
        vector<int>* colVector = new vector<int>(num);
        vector<float>* eleVector = new vector<float>(num);

        for (int i = 0; i < rows.size(); i++)
        {
            (*rowVector)[i] = cursor;
            map<int, float> row = rows[i];

            //for (map<int, float>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            //{
            //    if ((iter->first) == i ){
            //        colVector[(size_t)cursor] = (iter->first);
            //        eleVector[(size_t)cursor] = (iter->second);
            //        cursor++;
            //    }
            //}

            for (map<int, float>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            {
                (*colVector)[cursor] = (iter->first);
                (*eleVector)[cursor] = (iter->second);
                fprintf(stderr, "[%d, %d]: %f\n", i, colVector[cursor], eleVector[cursor]);
                cursor++;
            }
        }

        (*rowVector)[rows.size()] = cursor;

        return SparseMatrixReal(rows.size(), (*rowVector), (*colVector), (*eleVector), cursor);
    }
};

class SparseMatrixComplexBuilder {
public:
    vector<map<int, cmplx>> rows;

    SparseMatrixComplexBuilder(int rowsNum) {
        for (size_t i = 0; i < rowsNum; i++)
        {
            rows.push_back(map<int, cmplx>());
            rows[i][i] = cmplx(0);
        }
    };

    void plus(int row, int col, cmplx ele) {
        map<int, cmplx> rowMap = rows[row];

        cmplx matrixElement = rows[row][col];
        if (rows[row].find(col) == rows[row].end()) {
            rows[row][col] = ele;
        }
        else {
            rows[row][col] += ele;
        }
    }

    void subtract(int row, int col, cmplx ele) {

        //assert(row != 1 && col != 2);

        map<int, cmplx>* rowMap = &(rows[row]);

        if ((*rowMap).find(col) == (*rowMap).end()) {
            (*rowMap)[col] = ele * -1.0f;
        }
        else {
            (*rowMap)[col] -= ele;
        }
    }

    void addCircuits(const vector<Circuit> &lines, const vector<int> &nodeToSuperNode) {
        for (int k = 0; k < lines.size(); k++) {
            Circuit cir = lines[k];
            int i = nodeToSuperNode[cir.fbus];
            int j = nodeToSuperNode[cir.tbus];
            float chrg = cir.lineCharge;
            cmplx ladm = cir.admittance;
            float tr = cir.transformerRatio;
            float tap2 = tr * tr;
            plus(i, i, ladm + cmplx(0.0f, 0.5f * chrg));
            plus(j, j, ladm + cmplx(0.0f, 0.5f * chrg));
            subtract(i, j, ladm);
            subtract(j, i, ladm);
        }
    }

    SparseMatrixComplex build() {
        int num = 0;
        for (int i = 0; i < rows.size(); i++)
        {
            num += rows[i].size();
        }

        int cursor = 0;

        int* rowVector = new int[rows.size() + 1];
        int* colVector = new int[num];
        cmplx* eleVector = new cmplx[num];

        for (int i = 0; i < rows.size(); i++)
        {
            rowVector[i] = cursor;
            map<int, cmplx> row = rows[i];

            //for (map<int, complex>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            //{
            //    if ((iter->first) == i ){
            //        colVector[(size_t)cursor] = (iter->first);
            //        eleVector[(size_t)cursor] = (iter->second);
            //        cursor++;
            //    }
            //}

            for (map<int, cmplx>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            {
                    colVector[cursor] = (iter->first);
                    eleVector[cursor] = (iter->second);
                    fprintf(stderr, "[%d, %d]: %f + %fj\n", i, colVector[cursor], eleVector[cursor].real(), eleVector[cursor].imag());
                    cursor++;
            }
        }

        rowVector[rows.size()] = cursor;

        return SparseMatrixComplex( &rowVector[0] , &colVector[0], &eleVector[0], cursor);
    }
};



