// Interpolate face values from cell center velocities
surfaceVectorField U_sngrad = fvc::snGrad(U);

// Input feature 2 : velocities at each cell center
outputFile << runTime.timeName() << "sec_Ux_center";
forAll (y_cc, cellI)
{
    if (y_cc[cellI].y() <= 0.0015)
    {
        outputFile << ", " << mag(U[cellI]);
    }
}
outputFile << "\n";

// True output label : dU/dy at the wall
outputFile << runTime.timeName() << "sec_dUdy_wall";
forAll (y_cc, cellI)
{
    if (y_cc[cellI].y() <= 0.0015)
    {        
        outputFile << ", " << mag(U_sngrad.boundaryField()[surfaceID][0]);
    }
}
outputFile << "\n";
