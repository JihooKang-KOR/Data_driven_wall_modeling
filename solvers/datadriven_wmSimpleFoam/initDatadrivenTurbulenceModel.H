// The best model of 1D mapping
std::string bestModel1D_name = "finalmodel/final_model.pt";
torch::jit::script::Module bestModel1D = torch::jit::load(bestModel1D_name);
std::cout << "Module loaded\n";
