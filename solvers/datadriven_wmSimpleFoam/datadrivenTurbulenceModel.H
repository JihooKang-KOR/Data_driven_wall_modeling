// feature tensor with dimension [adjacentCellIDs.size() x nInputFeatures]
int nInputFeatures = 2;
torch::Tensor featureTensor = torch::ones({adjacentCellIDs.size(), nInputFeatures}, torch::kFloat64);

forAll (oppFaceIDs, faceI)
{    
    featureTensor[faceI][0] = dist_fface[faceI];
    featureTensor[faceI][1] = mag(U.internalField()[adjacentCellIDs[faceI]]);    
}

std::vector<torch::jit::IValue> inputResult{featureTensor};
torch::Tensor labelOutput = bestModel1D.forward(inputResult).toTensor();
auto labelAccessor = labelOutput.accessor<double,2>();
