// The distance based ML model model from 1D mapping
std::string distanceBasedModel_name = "distanceBasedModel/distanceBasedModel.pt";
torch::jit::script::Module distanceBasedModel = torch::jit::load(distanceBasedModel_name);

// The scaling module for the inputs in OpenFOAM
std::string scaleModule_name = "scaleModule_wallCorr/scaleModule_wallCorr.pt";
torch::jit::script::Module scaleModule = torch::jit::load(scaleModule_name);
std::cout << "Module loaded\n";
