// Interpolate face values from cell center velocities
surfaceVectorField U_face = fvc::interpolate(U);
surfaceVectorField U_sngrad = fvc::snGrad(U);

// Average velocity from integral fitting method
scalarField averageU(mag(mesh.Cf().internalField()));
forAll (averageU, faceI) // For U, it is actually a cell center value. (cellI)
{
    averageU[faceI] = 0;

    if (faceI == 0)
    {
        averageU[faceI] = U[faceI].x();
    }

    else
    {
        averageU[faceI] = U[0].x()*y_cf[0].y();

        for (int index = 1; index <= faceI; index++)
        {
            averageU[faceI] += U[index].x()*(y_cf[index].y() - y_cf[index-1].y());
        }

        averageU[faceI] = averageU[faceI]/y_cf[faceI].y();
    }
}

// Input feature 2 : integral average velocities
outputFile << runTime.timeName() << "sec_avgU";
forAll (U_face, faceI)
{
    if (y_cf[faceI].y()*maxUtau/nu_ <= 400)
    {        
        outputFile << ", " << averageU[faceI];
    }
}
outputFile << "\n";

// True output label 1 : dU/dy at the wall
outputFile << runTime.timeName() << "sec_dUdy_wall";
forAll (U_face, faceI)
{
    if (y_cf[faceI].y()*maxUtau/nu_ <= 400)
    {        
        outputFile << ", " << mag(U_sngrad.boundaryField()[surfaceID][0]);
    }
}
outputFile << "\n";

// True output label 2 : dU/dy at the faces
outputFile << runTime.timeName() << "sec_dUdy_face";
forAll (U_face, faceI)
{
    if (y_cf[faceI].y()*maxUtau/nu_ <= 400)
    {        
        outputFile << ", " << U_sngrad[faceI].x();
    }
}
outputFile << "\n";

// True output label 3 : velocity at the faces
outputFile << runTime.timeName() << "sec_Ux_face";
forAll (U_face, faceI)
{
    if (y_cf[faceI].y()*maxUtau/nu_ <= 400)
    {        
        outputFile << ", " << U_face[faceI].x();
    }
}
outputFile << "\n";
