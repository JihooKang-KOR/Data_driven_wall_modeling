// The best model of 1D mapping
std::string bestModel1D_name = "normalized1DModel/normalized1DModel.pt";
torch::jit::script::Module bestModel1D = torch::jit::load(bestModel1D_name);

// The scaling module for the inputs in OpenFOAM
std::string scaleModule_name = "scaleModule/scaleModule.pt";
torch::jit::script::Module scaleModule = torch::jit::load(scaleModule_name);
std::cout << "Module loaded\n";
