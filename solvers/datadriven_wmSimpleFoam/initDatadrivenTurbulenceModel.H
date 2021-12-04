// The best model of 1D mapping
std::string bestModel1D_name = "best_training_1D/best_traced_model.pt";
torch::jit::script::Module bestModel1D = torch::jit::load(bestModel1D_name);
std::cout << "Module loaded\n";

// Non-dimensionalized hardcoded min-max values
double yMin = 4.6867e-06;
double yMax = 0.0014;
double avgUMin = 0.0209;
double avgUMax = 0.9293;

double slopewallMin = 9.0939; // log
double slopewallMax = 9.9870; // log
double slopefaceMin = 0.0; // log
double slopefaceMax = 9.9831; // log
double faceUxMin = 0.0417;
double faceUxMax = 1.0003;
