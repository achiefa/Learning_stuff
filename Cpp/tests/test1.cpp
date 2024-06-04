#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <NNAD/FeedForwardNN.h>
#include <vector>

typedef nnad::FeedForwardNN<double> NNAD;
typedef std::vector<double> data;


class Observable {
  public:
  Observable (NNAD* nn) : _nn(nn) {
    _nout = _nn->GetArchitecture().back();
    _np = _nn->GetParameterNumber();
  }
  virtual ~Observable() {}
  void Initialise(data x) {
    if(!Evaluate(x))
      throw std::runtime_error("Something went wrong with the evaluation of the observable");
  }

  protected:
  virtual bool Evaluate(data &x) = 0;
  NNAD* _nn;
  int _nout;
  int _np;
};


template <int rank>
class BasicObservable : public Observable {
  public:
  typedef Eigen::Tensor<double, rank, Eigen::ColMajor> Tensor;
  BasicObservable(NNAD* nn) : Observable(nn) {
    InitialiseTensor();
  }
  BasicObservable() {};
  virtual ~BasicObservable () {}
  Tensor GetTensor() { return _tensor; }
  void SetTensor(Tensor T) { _tensor = T; }

  protected:
  Tensor _tensor;
  void Compute(const data &x) { _tensor = ComputeImpl(x); }
  virtual Tensor ComputeImpl(const data &x) = 0;
  Eigen::array<Eigen::Index, rank> _shape;

  private:
  bool Evaluate(data &x) override {
    Compute(x);
    return true;
  }
  void InitialiseTensor() {
    _tensor = Tensor (_shape);
    _tensor.setZero();
  }
};


// It might be a decorator
template <int rank>
class CombinedObservable {
  public:
  typedef Eigen::Tensor<double, rank, Eigen::ColMajor> Tensor;

  CombinedObservable(NNAD* nn) {}
  virtual ~CombinedObservable () {}

  auto Evaluate(){
    for(size_t i=0; i<RuleList.size(); i++){
      auto res = contract(RuleList[i]);
      const auto& d = res.dimensions();
      Observable* New_O = new BasicObservable<d.size>();
      New_O->SetTensor(res);

    }
  }


  private:
  struct Rule {
    Observable* O1;
    Observable* O2;
    std::vector<Eigen::IndexPair<int>> IndexContraction;
  };

  auto contract(Rule rule) {
    if(rule.O1 != nullptr){
      auto T1 = rule.O1->GetTensor();
      auto T2 = rule.O2->GetTensor();
      auto T1_T2 = T1.contract(T2, rule.IndexContraction);
      return T1_T2;
    }
  }

  std::vector<Rule> RuleList;
};





class dNN : public BasicObservable<2> {
  public:
  dNN (NNAD* nn) : BasicObservable(nn) {
    _shape = Eigen::array<Eigen::Index, 2 >{{_nout, _np}};
  }

  private:  
  Tensor ComputeImpl(const data &x) override {
    // .data() is needed because returns a direct pointer to the memory array used internally by the vector
    std::vector<double> d_nn = _nn->Derive(x);
    Eigen::TensorMap< Eigen::Tensor<double, 2, Eigen::ColMajor> > temp (d_nn.data(), _nout, _np + 1); // Col-Major

    // Get rid of the first column (the outputs) and stores only first derivatives
    Eigen::array<Eigen::Index, 2> offsets = {0, 1};
    Eigen::array<Eigen::Index, 2> extents = {_nout, _np};
    return temp.slice(offsets, extents);
  }
};



int main() {
  const std::vector<int> arch{3, 5, 5, 3};
  NNAD* nn = new NNAD(arch, 0, nnad::OutputFunction::ACTIVATION, true);
  data X {0.1, 2.3, 4.5};

  Observable* Test = new dNN{nn};
  dNN TestD (nn);

  Test -> Initialise(X);
  TestD.Initialise(X);

  dNN* ptrTest = static_cast<dNN*>(Test);
  auto T = ptrTest->GetTensor();
  auto TD = TestD.GetTensor();

  std::cout << T  << std::endl;
  std::cout << std::endl;
  std::cout << "______________________" << std::endl;
  std::cout << TD  << std::endl;





  delete nn;
  delete ptrTest;
  return 0;
}