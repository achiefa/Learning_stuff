#include <NNAD/FeedForwardNN.h>
#include <cxxabi.h>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

/** \defgroup Static implementation for NN observables
 *
 * Testing implementation of observables using the Policy design
 * pattern in a pure-static (compile-time) way.
 */

typedef nnad::FeedForwardNN<double> NNAD;
typedef std::vector<double> data;
template <int _RANK> using Tensor = Eigen::Tensor<double, _RANK>;


// Demangler function
template <typename T> std::string type(T&& p) {
  int r;
  std::string name;
  char* mangled_name = abi::__cxa_demangle (typeid(p).name(), 0, 0, &r);
  name += mangled_name;
  ::free(mangled_name);
  return name;
}

// Demangler function
template <typename D> std::string type2() {
  int r;
  std::string name;
  char* mangled_name = abi::__cxa_demangle (typeid(D).name(), 0, 0, &r);
  name += mangled_name;
  ::free(mangled_name);
  return name;
}

// Header file
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ----------------------------------------------------------------
//                       Base observable structure
//_________________________________________________________________
/**
 * @brief IObservable
 *
 * This is the common interface for all types of observables. It serves
 * as base class template meant for CRTP, and the first template argument
 * represents the derived class. This base class can also be interpreted as
 * a wrapper to the Eigen::Tensor class. The latter is a class template, and
 * in particular it requires the specification of the tensor rank at compile
 * time. Hence, `IObservable` inherits this dependency as well, and introduces
 * another template argument which specifies the rank of the tensor representing
 * the observable.
 *
 * Eigen::Tensor does not require compile-time information to set the size of
 * each dimension of the tensor once the rank is known. For this reason, the
 * constructor is a variadic template that allows multiple entries defining the
 * size of each dimension. The variadic private method `_initialise_tensor`
 * initialises the entries of the tensor to zero. The tensor is then stored
 * into a private attribute that can be accessed through the getter `GetTensor`
 *
 * @tparam D : The derived class used in the CRTP
 * @tparam _RANK : The rank of the tensor that stores the observable
 */
template <typename D, int _RANK> class IObservable {
public:
  IObservable() = default;
  template <typename... Args> IObservable(Args &&...args) {
    // TO-DO
    //  args must be integers, but no check is implemented.
    _initialise_tensor(args...);
    _d = _tensor.dimensions();
  }

  Tensor<_RANK> GetTensor() { return _tensor; }

  static std::string static_id(D&& d) { return type(d); }

  template<typename>
  static std::string static_id2() { return type2<D>(); }

  const std::string Get_id() { return type(*derived()); }

  static std::string id_base;
  static std::string id_base2;

protected:
  typename Tensor<_RANK>::Dimensions _d;
  Tensor<_RANK> _tensor;

private:
  D* derived() { return static_cast<D*>(this); }
  template <typename... Args> void _initialise_tensor(Args &&...args) {
    Tensor<_RANK> temp(std::forward<Args>(args)...);
    temp.setZero();
    _tensor = temp;
    return;
  }
};

/**
 * @brief Basic decorator
 *  
 * Decorate the IObservable class. It keeps the same signature for
 * the template arguments so that it can be used as base class
 * in the CRTP.
 *  
 * @tparam D: The derived class used in the CRTP
 * @tparam _RANK: The rank of the tensor that stores the observable
 */
template <typename D, int _RANK> class BASIC : public IObservable<D, _RANK> {
public:
  void Evaluate(const data &X, int a, NNAD *nn) {
    if (a > this->_d[0] - 1)
      throw std::invalid_argument("The size you provided is greater than the "
                                  "actual size of the tensor.");
    this->_tensor.chip(a, 0) = static_cast<D*>(this)->algorithm_impl(X, a, nn);
    data_map[a] = X;
  }

  void Evaluate(const std::vector<data> &Xv, NNAD *nn) {
    for (size_t a = 0; a < Xv.size(); a++)
      Evaluate(Xv[a], a, nn);
  }

  bool is_computed() { return data_map.size() >= this->_d[0]; }

  std::map<int, data> data_map;

  protected:
  BASIC() = default;
  template <typename... Args>
  BASIC(Args &&...args) : IObservable<D, _RANK>(args...) {}
  ~BASIC() {}
};

template <typename D, int _RANK> class COMBINED : public IObservable<D, _RANK> {
public:
  template <typename... Obs> void Evaluate(Obs *...obs) {
    if (!check_observables(obs...))
      throw std::logic_error("Something");
    this->_tensor = contract(obs...);
  }

protected:
  template <typename... Args> COMBINED(Args &&...args) : IObservable<D, _RANK>(args...) {}
  ~COMBINED() {}

private:
  template <typename... Obs> Tensor<_RANK> contract(Obs *...obs) {
    return static_cast<D *>(this)->contract_impl(obs...);
  };

  template <typename... Obs> bool check_observables(Obs *...obs) {
    for (const auto &o : {obs...})
      if (!o->is_computed())
        return false;
    return true;
  }
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class dNN : public BASIC<dNN, 3> {
  // At this point I need to specify the dimensions
public:
  dNN() = default;
  dNN(int size1, int size2, int size3) : BASIC<dNN, 3>(size1, size2, size3) {}

  Tensor<2> algorithm_impl(const data &X, int a, NNAD *nn) {
    // .data() is needed because returns a direct pointer to the memory array
    // used internally by the vector
    Eigen::TensorMap<Eigen::Tensor<double, 2, Eigen::ColMajor>> temp(
        nn->Derive(X).data(), _d[1], _d[2] + 1); // Col-Major

    // Get rid of the first column (the outputs) and stores only first
    // derivatives
    Eigen::array<Eigen::Index, 2> offsets = {0, 1};
    Eigen::array<Eigen::Index, 2> extents = {_d[1], _d[2]};
    return temp.slice(offsets, extents);
  }
};
template<> std::string IObservable<dNN,3>::id_base = IObservable::static_id(dNN());
template<> std::string IObservable<dNN,3>::id_base2 = IObservable::static_id2<dNN>();

class ddNN : public BASIC<ddNN, 4> {
  // At this point I need to specify the dimensions
public:
  ddNN(int size1, int size2, int size3, int size4)
      : BASIC<ddNN, 4>(size1, size2, size3, size4) {}

  Tensor<3> algorithm_impl(const data &X, int a, NNAD *nn) {
    int d1 = _d[1];
    int d2 = _d[2];
    int d3 = _d[3];
    int temp = d1 + d2 + d3;
    std::vector<double> results_vec(temp, 1.1);
    // Store into ColMajor tensor
    // The order of the dimensions has been tested in "SecondDerivative", and
    // worked out by hand.
    Eigen::TensorMap<Eigen::Tensor<double, 3, Eigen::ColMajor>> ddNN(
        results_vec.data(), d1, d2 + 1, d3);

    // Swap to ColMajor for compatibility and reshape
    Eigen::array<int, 3> new_shape{{0, 2, 1}};
    Eigen::Tensor<double, 3> ddNN_reshape = ddNN.shuffle(new_shape);

    // Get rid of the first column (the firs derivatives) and stores only second
    // derivatives
    Eigen::array<Eigen::Index, 3> offsets_3 = {0, 0, 1};
    Eigen::array<Eigen::Index, 3> extents_3 = {d1, d2, d3};
    return ddNN_reshape.slice(offsets_3, extents_3);
  }
};

class O2 : public COMBINED<O2, 4> {
  // At this point I need to specify the dimensions
public:
  O2() = default;
  O2(int size1, int size2, int size3, int size4)
      : COMBINED<O2, 4>(size1, size2, size3, size4) {}

  Tensor<4> contract_impl(dNN *dnn) {
    Eigen::array<Eigen::IndexPair<int>, 1> double_contraction = {
        Eigen::IndexPair<int>(2, 2)};
    auto dnn_tensor = dnn->GetTensor();
    return dnn_tensor.contract(dnn_tensor, double_contraction);
  }
};
template<> std::string IObservable<O2,4>::id_base = IObservable::static_id(O2());
template<> std::string IObservable<O2,4>::id_base2 = IObservable::static_id2<O2>();

class O3 : public COMBINED<O3, 6> {
  // At this point I need to specify the dimensions
public:
  O3(int size1, int size2, int size3, int size4, int size5, int size6)
      : COMBINED<O3, 6>(size1, size2, size3, size4, size5, size6) {}

  Tensor<6> contract_impl(dNN *dnn, ddNN *ddnn) {
    auto dnn_tensor = dnn->GetTensor();
    auto ddnn_tensor = ddnn->GetTensor();
    Eigen::array<Eigen::IndexPair<int>, 0> tensor_product = {};
    Eigen::array<Eigen::IndexPair<int>, 2> first_contraction = {
        Eigen::IndexPair<int>(2, 2), Eigen::IndexPair<int>(3, 5)};
    auto d_mu_d_nu_f_ia = dnn_tensor.contract(dnn_tensor, tensor_product);
    return ddnn_tensor.contract(d_mu_d_nu_f_ia, first_contraction);
  }
};

int main() {
  const std::vector<int> arch{3, 5, 5, 3};
  NNAD *nn = new NNAD(arch, 0, nnad::OutputFunction::ACTIVATION, true);
  data X{0.1, 2.3, 4.5};

  int size = 4;
  int nout = arch.back();
  int np = nn->GetParameterNumber();

  dNN dnn(size, nout, np);

  dnn.Evaluate(X, 0, nn);
  std::cout << dnn.data_map.size() << std::endl;

  dnn.Evaluate(X, 0, nn);
  std::cout << dnn.data_map.size() << std::endl;

  dnn.Evaluate(X, 2, nn);
  dnn.Evaluate(X, 1, nn);
  dnn.Evaluate(X, 3, nn);
  std::cout << dnn.data_map.size() << std::endl;

  std::cout << dnn.is_computed() << std::endl;

  O2 NTK(size, size, nout, nout);
  NTK.Evaluate(&dnn);

  std::cout << dnn.Get_id() << std::endl;
  //std::cout << dNN::id << std::endl;
  std::cout << dNN::id_base << std::endl;
  std::cout << dNN::id_base2 << std::endl;
  std::cout << O2::id_base << std::endl;
  std::cout << O2::id_base2 << std::endl;
  

  // delete dnn;
  return 0;
}