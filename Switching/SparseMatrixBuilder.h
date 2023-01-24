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

using namespace std;


class SparseMatrixRealBuilder {
public:
    vector<map<int, real>> rows;

    SparseMatrixRealBuilder(int rowsNum) {
        for (size_t i = 0; i < rowsNum; i++)
        {
            rows.push_back(map<int, real>());
        }
    };

    void plus(int row, int col, real ele) {
        map<int, real> rowMap = rows[row];

        real matrixElement = rows[row][col];
        if (rows[row].find(col) == rows[row].end()) {
            rows[row][col] = ele;
        }
        else {
            rows[row][col] += ele;
        }
    }

    void subtract(int row, int col, real ele) {

        //assert(row != 1 && col != 2);

        map<int, real>* rowMap = &(rows[row]);

        if ((*rowMap).find(col) == (*rowMap).end()) {
            (*rowMap)[col] = ele * -1.0f;
        }
        else {
            (*rowMap)[col] -= ele;
        }
    }

    void addCircuits(const vector<Circuit>& lines, const vector<int>& nodeToSuperNode, const vector<complex>& voltages) {
        for (int k = 0; k < lines.size(); k++) {
            Circuit cir = lines[k];
            int i = nodeToSuperNode[cir.fbus];
            int j = nodeToSuperNode[cir.tbus];
            real cap = cir.lineCharge;
            complex imp = Complex(1) / cir.admittance;
            real r = imp.x;
            real ind = imp.y;
            

        }
    }

    void addGens(const vector<Circuit>& lines, const vector<int>& nodeToSuperNode, const vector<complex>& voltages) {
        for (int k = 0; k < lines.size(); k++) {
            Circuit cir = lines[k];
            int i = nodeToSuperNode[cir.fbus];
            int j = nodeToSuperNode[cir.tbus];
            real chrg = cir.lineCharge;
            real ladm = cir.admittance;

        }
    }

    void addLoads(const vector<Circuit>& lines, const vector<int>& nodeToSuperNode, const vector<complex>& voltages) {
        for (int k = 0; k < lines.size(); k++) {
            Circuit cir = lines[k];
            int i = nodeToSuperNode[cir.fbus];
            int j = nodeToSuperNode[cir.tbus];
            real chrg = cir.lineCharge;
            real ladm = cir.admittance;

        }
    }

    SparseMatrixReal build() {
        int num = 0;
        for (int i = 0; i < rows.size(); i++)
        {
            num += rows[i].size();
        }

        int cursor = 0;

        int* rowVector = new int[rows.size() + 1];
        int* colVector = new int[num];
        real* eleVector = new real[num];

        for (int i = 0; i < rows.size(); i++)
        {
            rowVector[i] = cursor;
            map<int, real> row = rows[i];

            //for (map<int, real>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            //{
            //    if ((iter->first) == i ){
            //        colVector[(size_t)cursor] = (iter->first);
            //        eleVector[(size_t)cursor] = (iter->second);
            //        cursor++;
            //    }
            //}

            for (map<int, real>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            {
                colVector[cursor] = (iter->first);
                eleVector[cursor] = (iter->second);
                fprintf(stderr, "[%d, %d]: %f\n", i, colVector[cursor], eleVector[cursor]);
                cursor++;
            }
        }

        rowVector[rows.size()] = cursor;

        return SparseMatrixReal(dim3(rows.size(), rows.size()), &rowVector[0], &colVector[0], &eleVector[0], cursor);
    }
};

class SparseMatrixComplexBuilder {
public:
    vector<map<int, complex>> rows;

    SparseMatrixComplexBuilder(int rowsNum) {
        for (size_t i = 0; i < rowsNum; i++)
        {
            rows.push_back(map<int, complex>());
            rows[i][i] = Complex(0);
        }
    };

    void plus(int row, int col, complex ele) {
        map<int, complex> rowMap = rows[row];

        complex matrixElement = rows[row][col];
        if (rows[row].find(col) == rows[row].end()) {
            rows[row][col] = ele;
        }
        else {
            rows[row][col] += ele;
        }
    }

    void subtract(int row, int col, complex ele) {

        //assert(row != 1 && col != 2);

        map<int, complex>* rowMap = &(rows[row]);

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
            real chrg = cir.lineCharge;
            complex ladm = cir.admittance;
            real tr = cir.transformerRatio;
            real tap2 = tr * tr;
            plus(i, i, ladm + Complex(0.0f, 0.5f * chrg));
            plus(j, j, ladm + Complex(0.0f, 0.5f * chrg));
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
        complex* eleVector = new complex[num];

        for (int i = 0; i < rows.size(); i++)
        {
            rowVector[i] = cursor;
            map<int, complex> row = rows[i];

            //for (map<int, complex>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            //{
            //    if ((iter->first) == i ){
            //        colVector[(size_t)cursor] = (iter->first);
            //        eleVector[(size_t)cursor] = (iter->second);
            //        cursor++;
            //    }
            //}

            for (map<int, complex>::const_iterator iter = row.begin(); iter != row.end(); iter++)
            {
                    colVector[cursor] = (iter->first);
                    eleVector[cursor] = (iter->second);
                    fprintf(stderr, "[%d, %d]: %f + %fj\n", i, colVector[cursor], eleVector[cursor].realVal(), eleVector[cursor].imagVal());
                    cursor++;
            }
        }

        rowVector[rows.size()] = cursor;

        return SparseMatrixComplex(dim3(rows.size(), rows.size()), &rowVector[0] , &colVector[0], &eleVector[0], cursor);
    }
};



