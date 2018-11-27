#pragma once

#include "cereal/archives/binary.hpp"

#include <Eigen/Dense>

namespace cereal {

template <class Archive, class _Scalar, int _Rows, int _Cols,
        int _Options, int _MaxRows, int _MaxCols> inline
void save(Archive& archive, const Eigen::Array<_Scalar, _Rows, _Cols,
        _Options, _MaxRows, _MaxCols>& array) {
    int rows = array.rows();
    int cols = array.cols();

    archive(rows);
    archive(cols);
    archive(binary_data(array.data(), rows * cols * sizeof(_Scalar)));
}

template <class Archive, class _Scalar, int _Rows, int _Cols,
        int _Options, int _MaxRows, int _MaxCols> inline
void load(Archive& archive, Eigen::Array<_Scalar, _Rows, _Cols,
        _Options, _MaxRows, _MaxCols>& array) {
    int rows, cols;

    archive(rows);
    archive(cols);

    array.resize(rows, cols);
    archive(binary_data(array.data(),
            static_cast<std::size_t>(rows * cols * sizeof(_Scalar))));
}

template <class Archive, class _Scalar, int _Rows, int _Cols,
        int _Options, int _MaxRows, int _MaxCols> inline
void save(Archive& archive, const Eigen::Matrix<_Scalar, _Rows, _Cols,
        _Options, _MaxRows, _MaxCols>& matrix) {
    int rows = matrix.rows();
    int cols = matrix.cols();

    archive(rows);
    archive(cols);
    archive(binary_data(matrix.data(), rows * cols * sizeof(_Scalar)));
}

template <class Archive, class _Scalar, int _Rows, int _Cols,
        int _Options, int _MaxRows, int _MaxCols> inline
void load(Archive& archive, Eigen::Matrix<_Scalar, _Rows, _Cols,
        _Options, _MaxRows, _MaxCols>& matrix) {
    int rows, cols;

    archive(rows);
    archive(cols);

    matrix.resize(rows, cols);
    archive(binary_data(matrix.data(),
            static_cast<std::size_t>(rows * cols * sizeof(_Scalar))));
}

template <class Archive, class Scalar, int Dim, int Mode, int _Options> inline
void save(Archive& archive, const Eigen::Transform<Scalar, Dim, Mode,
        _Options>& transform) {
    archive(transform.matrix());
}

template <class Archive, class Scalar, int Dim, int Mode, int _Options> inline
void load(Archive& archive, Eigen::Transform<Scalar, Dim, Mode,
        _Options>& transform) {
    archive(transform.matrix());
}

}  // namespace cereal

