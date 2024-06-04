#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <NNAD/FeedForwardNN.h>


typedef nnad::FeedForwardNN<double> NNAD;
typedef std::vector<double> data;
template <int _RANK> using Tensor = Eigen::Tensor<double, _RANK>;

template <typename D> class Factory {
  public:
  template <typename... Args> static D* create(Args&&... args) { return new D(std::forward<Args>(args)...); }
  static void destroy(D* d) { delete d; }
};

template <typename D, int _RANK> class Common {
  public:

  template <typename ... Args>
  Common(Args&& ... args) { 
    _initialise_tensor(args...);
    _d = _tensor.dimensions();
  }

  template <typename ... Args> void _initialise_tensor(Args&& ... args) {
    Tensor<_RANK> temp(std::forward<Args>(args) ...);
    temp.setZero();
    _tensor = temp;
    return;
  }
  Tensor<_RANK> _tensor; // Works
  typename Tensor<_RANK>::Dimensions _d;
  D* derived() { return static_cast<D*>(this); }
  Tensor<_RANK> GetTensor() { return _tensor; }
};


template <typename D, int _RANK> class BASIC : public Common<D, _RANK> {
  public:
  template <typename ... Args>
  BASIC(Args&& ... args) : Common<D, _RANK>(args...) {}
  ~BASIC() {}

  virtual Tensor<_RANK-1> algorithm_impl(const data &X, int a, NNAD* nn) = 0;

  void Evaluate(const data &X, int a, NNAD* nn) {
    if (a > this->_d[0] - 1)
      throw std::invalid_argument("The size you provided is greater than the actual size of the tensor.");
    this->_tensor.chip(a, 0) = algorithm_impl(X, a, nn);
    data_map[a] = X;

  }

  void Evaluate(const std::vector<data> &Xv, NNAD* nn) {
    for (size_t a = 0; a < Xv.size(); a++)
      Evaluate(Xv[a], a, nn);
  }

  bool is_computed() { return data_map.size() >= this->_d[0]; }

  std::map<int, data> data_map;
};


template <typename D, int _RANK> class COMBINED : public Common<D, _RANK> {
  public:
  template <typename ... Args>
  COMBINED(Args&& ... args) : Common<D, _RANK>(args...) {}
  ~COMBINED() {}

  template<typename ... Obs>
  void Evaluate(Obs* ... obs) {
    if (!check_observables(obs...))
      throw std::logic_error("Something");
    this->_tensor = contract(obs...);
  }

  template<typename ... Obs>
  Tensor<_RANK> contract(Obs* ... obs) { return static_cast<D*>(this)->contract_impl(obs...); };

  template<typename ... Obs>
  bool check_observables(Obs* ... obs) {
    for(const auto& o : {obs...})
      if (!o->is_computed())
        return false;
      return true;
  }
};


template <template <typename> class POLICY> 
class Observable : public POLICY< Observable<POLICY>> {
  public:
  template<typename ... Args>
  Observable(Args&& ... args) : POLICY< Observable< POLICY > >(args...) {}
  static std::string type_id;
};



// This one should be the factory method
/* template <template <typename> class POLICY> 
class Observable2 : public POLICY< Observable2<POLICY>>, public Factory < Observable2 < POLICY > > {
  public:
  template<typename ... Args>
  Observable2(Args&& ... args) : POLICY< Observable2< POLICY > >(args...) {}
}; */


template <typename D> class dNN : public BASIC<D, 3> {
  // At this point I need to specify the dimensions
  public:
  
  dNN(int size1, int size2, int size3) : BASIC<D, 3>(size1, size2, size3)  {}
   
  Tensor<2> algorithm_impl(const data &X, int a, NNAD* nn) override {
    // .data() is needed because returns a direct pointer to the memory array used internally by the vector
    int d1 = this->_d[1];
    int d2 = this->_d[2];
    Eigen::TensorMap< Eigen::Tensor<double, 2, Eigen::ColMajor> > temp (nn->Derive(X).data(), d1, d2 + 1); // Col-Major

    // Get rid of the first column (the outputs) and stores only first derivatives
    Eigen::array<Eigen::Index, 2> offsets = {0, 1};
    Eigen::array<Eigen::Index, 2> extents = {d1, d2};
    return temp.slice(offsets, extents);
  }
};
template<> std::string Observable<dNN>::type_id = "dNN";

template <typename D> class ddNN : public BASIC<D, 4> {
  // At this point I need to specify the dimensions
  public:
  
  ddNN(int size1, int size2, int size3, int size4) : BASIC<D, 4>(size1, size2, size3, size4) {}
   
  Tensor<3> algorithm_impl(const data &X, int a, NNAD* nn) override {
    int d1 = this->_d[1];
    int d2 = this->_d[2];
    int d3 = this->_d[3];
    int temp = d1 + d2 + d3;
    std::vector<double> results_vec (temp, 1.1);
    // Store into ColMajor tensor
    // The order of the dimensions has been tested in "SecondDerivative", and worked out by hand.
    Eigen::TensorMap< Eigen::Tensor<double, 3, Eigen::ColMajor> > ddNN (results_vec.data(), d1, d2 + 1, d3);
    
    // Swap to ColMajor for compatibility and reshape
    Eigen::array<int, 3> new_shape{{0, 2, 1}};
    Eigen::Tensor<double, 3> ddNN_reshape = ddNN.shuffle(new_shape);

    // Get rid of the first column (the firs derivatives) and stores only second derivatives
    Eigen::array<Eigen::Index, 3> offsets_3 = {0, 0, 1};
    Eigen::array<Eigen::Index, 3> extents_3 = {d1, d2, d3};
    return ddNN_reshape.slice(offsets_3, extents_3);
  }
};
template<> std::string Observable<ddNN>::type_id = "ddNN";




template <typename D> class O2 : public COMBINED<D, 4> {
  // At this point I need to specify the dimensions
  public:
  
  O2(int size1, int size2, int size3, int size4) : COMBINED<D, 4>(size1, size2, size3, size4) {}

  Tensor<4> contract_impl(Observable<dNN>* dnn) {
    Eigen::array<Eigen::IndexPair<int>, 1> double_contraction = { Eigen::IndexPair<int>(2,2) };
    auto dnn_tensor = dnn->GetTensor();
    return dnn_tensor.contract(dnn_tensor, double_contraction);
  }

};
template<> std::string Observable<O2>::type_id = "O2";


template <typename D> class O3 : public COMBINED<D, 6> {
  // At this point I need to specify the dimensions
  public:
  
  O3(int size1, int size2, int size3, int size4, int size5, int size6) : 
  COMBINED<D, 6> (size1, size2, size3, size4, size5, size6) {}

  Tensor<6> contract_impl(Observable<dNN>* dnn, Observable<ddNN>* ddnn) {
    auto dnn_tensor = dnn->GetTensor();
    auto ddnn_tensor = ddnn->GetTensor();
    Eigen::array<Eigen::IndexPair<int>, 0> tensor_product = {  };
    Eigen::array<Eigen::IndexPair<int>, 2> first_contraction = { Eigen::IndexPair<int>(2,2), Eigen::IndexPair<int>(3,5) };
    auto d_mu_d_nu_f_ia = dnn_tensor.contract(dnn_tensor, tensor_product);
    return ddnn_tensor.contract(d_mu_d_nu_f_ia, first_contraction);
  }

};
template<> std::string Observable<O3>::type_id = "O3";


int main() {
  const std::vector<int> arch{3, 5, 5, 3};
  NNAD* nn = new NNAD(arch, 0, nnad::OutputFunction::ACTIVATION, true);
  data X {0.1, 2.3, 4.5};

  int size = 4;
  int nout = arch.back();
  int np = nn->GetParameterNumber();

  Observable<dNN> dnn (size, nout, np);

  dnn.Evaluate(X, 0, nn);
  std::cout << dnn.data_map.size() << std::endl;

  dnn.Evaluate(X, 0, nn);
  std::cout << dnn.data_map.size() << std::endl;

  dnn.Evaluate(X, 2, nn);
  dnn.Evaluate(X, 1, nn);
  dnn.Evaluate(X, 3, nn);
  std::cout << dnn.data_map.size() << std::endl;

  std::cout << dnn.is_computed() << std::endl;


  Observable<O2> NTK (size, size, nout,nout);
  NTK.Evaluate(&dnn);

  std::cout << NTK.GetTensor() << std::endl;

  //delete dnn;
  return 0;
} 