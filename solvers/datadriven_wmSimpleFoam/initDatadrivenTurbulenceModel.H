// The best model of 1D mapping
std::string bestModel1D_name = "best_training_1D/best_traced_model.pt";
torch::jit::script::Module bestModel1D = torch::jit::load(bestModel1D_name);
std::cout << "Module loaded\n";

std::vector<double> featureMinValues{};
std::vector<double> featureMaxValues{};
std::vector<double> labelMinValues{};
std::vector<double> labelMaxValues{};

// Non-dimensionalized hardcoded min-max values
double yMin = 4.6867e-06;
double yMax = 0.0014;
double avgUMin = 0.0196;
double avgUMax = 0.9465;

double slopewallMin = 9.0295; // log
double slopewallMax = 10.0416; // log
double slopefaceMin = 0.0; // log
double slopefaceMax = 10.0367; // log
double faceUxMin = 0.0391;
double faceUxMax = 1.0003;
