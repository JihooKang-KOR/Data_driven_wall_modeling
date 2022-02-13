// Feature tensor with dimension [adjacentCellIDs.size() x nInputFeatures]
int nInputFeatures = 2;
torch::Tensor featureTensor = torch::ones({adjacentCellIDs.size(), nInputFeatures}, torch::kFloat64);

forAll (oppFaceIDs, faceI)
{    
    featureTensor[faceI][0] = dist_fface[faceI];
    featureTensor[faceI][1] = mag(U.internalField()[adjacentCellIDs[faceI]]);
}

// Mapping tensor
std::vector<torch::jit::IValue> inputResult{featureTensor};
torch::Tensor labelOutput = distanceBasedModel.forward(inputResult).toTensor();
auto labelAccessor = labelOutput.accessor<double,2>();

// Input scaling tensor to check how many points are extrapolated (greater than 5%)
torch::Tensor scaleInput = scaleModule.forward(inputResult).toTensor();
auto scaleAccessor = scaleInput.accessor<double,2>();

// Count variables for checking extrapolation
int extraCount_y = 0;
int extraCount_U_center = 0;
forAll (oppFaceIDs, faceI)
{
    if (scaleAccessor[faceI][0] > 1.05 || scaleAccessor[faceI][0] < -0.05)
    {
        extraCount_y++;
    }

    if (scaleAccessor[faceI][1] > 1.05 || scaleAccessor[faceI][1] < -0.05)
    {
        extraCount_U_center++;
    }
}

// Print how many points are extrapolated in the solver log file
Info << "Extrapolation points for y coordinate at faces = " << extraCount_y
<< " of " << nuEff.boundaryField()[surfaceID].size() << " points" << endl;

Info << "Extrapolation points for cell centered velocity = " << extraCount_U_center
<< " of " << nuEff.boundaryField()[surfaceID].size() << " points" << endl << endl;
