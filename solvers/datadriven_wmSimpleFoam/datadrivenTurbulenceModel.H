// feature tensor with dimension [adjacentCellIDs.size() x nInputFeatures]
int nInputFeatures = 2;
torch::Tensor featureTensor = torch::ones({adjacentCellIDs.size(), nInputFeatures}, torch::kFloat64);

forAll (oppFaceIDs, faceI)
{    
    featureTensor[faceI][0] = dist_fface[faceI]/2.0;
    featureTensor[faceI][1] = mag(U.internalField()[adjacentCellIDs[faceI]])/69.4;
    // Scaling to normalization
    featureTensor[faceI][0] = (featureTensor[faceI][0] - yMin) / (yMax - yMin + ROOTVSMALL);
    featureTensor[faceI][1] = (featureTensor[faceI][1] - avgUMin) / (avgUMax - avgUMin + ROOTVSMALL);
}

std::vector<torch::jit::IValue> inputResult{featureTensor};
torch::Tensor labelOutput = bestModel1D.forward(inputResult).toTensor();
auto labelAccessor = labelOutput.accessor<double,2>();

forAll (oppFaceIDs, faceI)
{
    // Rescale from normalization    
    labelAccessor[faceI][0] = labelAccessor[faceI][0]*(slopewallMax - slopewallMin) + slopewallMin;
    labelAccessor[faceI][1] = labelAccessor[faceI][1]*(slopefaceMax - slopefaceMin) + slopefaceMin;
    labelAccessor[faceI][2] = labelAccessor[faceI][2]*(faceUxMax - faceUxMin) + faceUxMin;

    // Rescale
    labelAccessor[faceI][2] = labelAccessor[faceI][2]*69.4;

    // Rescale from logarithmic function
    labelAccessor[faceI][0] = (Foam::exp(labelAccessor[faceI][0]) - 1.0)*69.4/2.0;
    labelAccessor[faceI][1] = (Foam::exp(labelAccessor[faceI][1]) - 1.0)*69.4/2.0;    
}
