#include <Eigen/Eigen>
#include <boost/numpy.hpp>
// #include <glog/logging.h>
#include <numpy/arrayobject.h>

namespace bp = boost::python;
namespace np = boost::numpy;

using namespace Eigen;

template <typename SCALAR>
struct NumpyEquivalentType {};

template <> struct NumpyEquivalentType<double> {enum { type_code = NPY_DOUBLE };};
template <> struct NumpyEquivalentType<int> {enum { type_code = NPY_INT };};
template <> struct NumpyEquivalentType<float> {enum { type_code = NPY_FLOAT };};
template <> struct NumpyEquivalentType<std::complex<double> > {enum { type_code = NPY_CDOUBLE };};

template <typename SourceType, typename DestType >
static void copy_array(const SourceType* source, DestType* dest,
                       const npy_int &nb_rows, const npy_int &nb_cols,
    const bool &isSourceTypeNumpy = false, const bool &isDestRowMajor = true,
    const bool& isSourceRowMajor = true,
    const npy_int &numpy_row_stride = 1, const npy_int &numpy_col_stride = 1)
{
  // determine source strides
  int row_stride = 1, col_stride = 1;
  if (isSourceTypeNumpy) {
    row_stride = numpy_row_stride;
    col_stride = numpy_col_stride;
  } else {
    if (isSourceRowMajor) {
      row_stride = nb_cols;
    } else {
      col_stride = nb_rows;
    }
  }

  if (isDestRowMajor) {
    for (int r=0; r<nb_rows; r++) {
      for (int c=0; c<nb_cols; c++) {
        *dest = source[r*row_stride + c*col_stride];
        dest++;
      }
    }
  } else {
    for (int c=0; c<nb_cols; c++) {
      for (int r=0; r<nb_rows; r++) {
        *dest = source[r*row_stride + c*col_stride];
        dest++;
      }
    }
  }
}


template<class MatType> // MatrixXf or MatrixXd
struct EigenMatrixToPython {
  static PyObject* convert(const MatType& mat) {
    npy_intp shape[2] = { mat.rows(), mat.cols() };
    PyArrayObject* python_array = (PyArrayObject*)PyArray_SimpleNew(
        2, shape, NumpyEquivalentType<typename MatType::Scalar>::type_code);

    copy_array(mat.data(),
               (typename MatType::Scalar*)PyArray_DATA(python_array),
               mat.rows(),
               mat.cols(),
               false,
               true,
               MatType::Flags & Eigen::RowMajorBit);
    return (PyObject*)python_array;
  }
};


template<typename MatType>
struct EigenMatrixFromPython {
  typedef typename MatType::Scalar T;

  EigenMatrixFromPython() {
    bp::converter::registry::push_back(&convertible,
                                       &construct,
                                       bp::type_id<MatType>());
  }

  static void* convertible(PyObject* obj_ptr) {
    if (!PyArray_Check(obj_ptr)) {
      // LOG(ERROR) << "PyArray_Check failed";
      return 0;
    }
    if (PyArray_NDIM(obj_ptr) > 2) {
      // LOG(ERROR) << "dim > 2";
      return 0;
    }
    if (PyArray_ObjectType(obj_ptr, 0) != NumpyEquivalentType<typename MatType::Scalar>::type_code) {
      // LOG(ERROR) << "types not compatible";
      return 0;
    }
    int flags = PyArray_FLAGS(obj_ptr);
    if (!(flags & NPY_ARRAY_C_CONTIGUOUS)) {
      // LOG(ERROR) << "Contiguous C array required";
      return 0;
    }
    if (!(flags & NPY_ARRAY_ALIGNED)) {
      // LOG(ERROR) << "Aligned array required";
      return 0;
    }
    return obj_ptr;
  }

  static void construct(PyObject* obj_ptr,
                        bp::converter::rvalue_from_python_stage1_data* data) {
    const int R = MatType::RowsAtCompileTime;
    const int C = MatType::ColsAtCompileTime;

    using bp::extract;

    PyArrayObject *array = reinterpret_cast<PyArrayObject*>(obj_ptr);
    int ndims = PyArray_NDIM(obj_ptr);

    int dtype_size = (PyArray_DESCR(obj_ptr))->elsize;
    int s1 = PyArray_STRIDE(obj_ptr, 0);
    // CHECK_EQ(0, s1 % dtype_size);
    int s2 = 0;
    if (ndims > 1) {
      s2 = PyArray_STRIDE(obj_ptr, 1);
      // CHECK_EQ(0, s2 % dtype_size);
    }


    int nrows = R;
    int ncols = C;
    if (ndims == 2) {
      if (R != Eigen::Dynamic) {
        // CHECK_EQ(R, array->dimensions[0]);
      } else {
        nrows = array->dimensions[0];
      }

      if (C != Eigen::Dynamic) {
        // CHECK_EQ(C, array->dimensions[1]);
      } else {
        ncols = array->dimensions[1];
      }
    } else {
      // CHECK_EQ(1, ndims);
      // Vector are a somehow special case because for Eigen, everything is
      // a 2D array with a dimension set to 1, but to numpy, vectors are 1D
      // arrays
      // So we could get a 1x4 array for a Vector4

      // For a vector, at least one of R, C must be 1
      // CHECK(R == 1 || C == 1);

      if (R == 1) {
        if (C != Eigen::Dynamic) {
          // CHECK_EQ(C, array->dimensions[0]);
        } else {
          ncols = array->dimensions[0];
        }
        // We have received a 1xC array and want to transform to VectorCd,
        // so we need to transpose
        // TODO: An alternative is to add wrappers for RowVector, but maybe
        // implicit transposition is more natural
        std::swap(s1, s2);
      } else {
        if (R != Eigen::Dynamic) {
          // CHECK_EQ(R, array->dimensions[0]);
        } else {
          nrows = array->dimensions[0];
        }
      }
    }

    T* raw_data = reinterpret_cast<T*>(PyArray_DATA(array));

    typedef Map<Matrix<T, Dynamic, Dynamic, RowMajor>, Aligned,
                Stride<Dynamic, Dynamic>> MapType;

    void* storage=((bp::converter::rvalue_from_python_storage<MatType>*)
                   (data))->storage.bytes;

    new (storage) MatType;
    MatType* emat = (MatType*)storage;
    // TODO: This is a (potentially) expensive copy operation. There should
    // be a better way
    *emat = MapType(raw_data, nrows, ncols,
                Stride<Dynamic, Dynamic>(s1/dtype_size, s2/dtype_size));
    data->convertible = storage;
  }
};

#define EIGEN_MATRIX_CONVERTER(Type) \
  EigenMatrixFromPython<Type>();  \
  bp::to_python_converter<Type, EigenMatrixToPython<Type> >();

#define MAT_CONV(R, C, T) \
  typedef Matrix<T, R, C> Matrix ## R ## C ## T; \
  EIGEN_MATRIX_CONVERTER(Matrix ## R ## C ## T);

// This require a MAT_CONV for that Matrix type to be registered first
#define MAP_CONV(R, C, T) \
  typedef Map<Matrix ## R ## C ## T> Map ## R ## C ## T; \
  EIGEN_MATRIX_CONVERTER(Map ## R ## C ## T);

#define T_CONV(R, C, T) \
  typedef Transpose<Matrix ## R ## C ## T> Transpose ## R ## C ## T; \
  EIGEN_MATRIX_CONVERTER(Transpose ## R ## C ## T);


#define BLOCK_CONV(R, C, BR, BC, T) \
  typedef Block<Matrix ## R ## C ## T, BR, BC> Block ## R ## C ## BR ## BC ## T; \
  EIGEN_MATRIX_CONVERTER(Block ## R ## C ## BR ## BC ## T);

static const int X = Eigen::Dynamic;

void SetupEigenConverters() {
  import_array();

  EIGEN_MATRIX_CONVERTER(Matrix2f);
  EIGEN_MATRIX_CONVERTER(Matrix2d);
  EIGEN_MATRIX_CONVERTER(Matrix3f);
  EIGEN_MATRIX_CONVERTER(Matrix3d);
  EIGEN_MATRIX_CONVERTER(Matrix4f);
  EIGEN_MATRIX_CONVERTER(Matrix4d);

  EIGEN_MATRIX_CONVERTER(Vector2f);
  EIGEN_MATRIX_CONVERTER(Vector3f);
  EIGEN_MATRIX_CONVERTER(Vector4f);
  EIGEN_MATRIX_CONVERTER(Vector2d);
  EIGEN_MATRIX_CONVERTER(Vector3d);
  EIGEN_MATRIX_CONVERTER(Vector4d);

  EIGEN_MATRIX_CONVERTER(VectorXi);

  MAT_CONV(2, 3, double);
  MAT_CONV(X, 3, double);
  MAT_CONV(X, X, double);
  MAT_CONV(X, 1, double);
  MAT_CONV(1, 4, double);
  MAT_CONV(1, X, double);
  MAT_CONV(3, 4, double);
  MAT_CONV(2, X, double);
}
