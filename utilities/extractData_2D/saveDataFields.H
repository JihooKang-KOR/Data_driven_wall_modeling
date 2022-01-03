// Interpolate face values from cell center velocities
surfaceVectorField U_face = fvc::interpolate(U);
surfaceVectorField U_sngrad = fvc::snGrad(U);

// Vector for average velocity in x-direction
std::vector<scalar> averageU;

// Calculate average velocity
for (int i = 0; i < steps; i++)
{
    forAll (patchFaceIDs, faceI) // For U, it is actually a cell center value. (cellI)
    {
        scalar sum = 0;

        if (i == 0)
        {
            averageU.push_back(U[adjacentCellIDs[faceI]].x());
        }

        else
        {
            sum = U[adjacentCellIDs[faceI]].x()*Cf[faceIDs[faceI]].y();

            for (int index = 0; index < i; index++)
            {
                sum += U[cellIDs[index*patchFaceIDs.size() + faceI]].x()
                *(Cf[faceIDs[(index + 1)*patchFaceIDs.size() + faceI]].y() - 
                Cf[faceIDs[index*patchFaceIDs.size() + faceI]].y());
            }            
            averageU.push_back(sum/Cf[faceIDs[(i + 1)*patchFaceIDs.size() + faceI]].y());
        }
    }
}

forAll (patchFaceIDs, faceI)
{
    outputFile_wall << Cc[adjacentCellIDs[faceI]].x() << ", ";
    outputFile_wall << Cc[adjacentCellIDs[faceI]].y() << ", ";
    outputFile_wall << U[adjacentCellIDs[faceI]].x() << ", ";
    outputFile_wall << mag(U_sngrad.boundaryField()[surfaceID][faceI]) << ", ";
    outputFile_wall << "\n";
}

for (int i = 0; i < steps; i++)
{
    forAll (patchFaceIDs, faceI)
    {
        outputFile_face << Cf[faceIDs[i*patchFaceIDs.size() + faceI]].x() << ", ";
        outputFile_face << Cf[faceIDs[i*patchFaceIDs.size() + faceI]].y() << ", ";
        outputFile_face << averageU[i*patchFaceIDs.size() + faceI] << ", ";
        outputFile_face << mag(U_sngrad[faceIDs[i*patchFaceIDs.size() + faceI]]) << ", ";
        outputFile_face << U_face[faceIDs[i*patchFaceIDs.size() + faceI]].x();
        outputFile_face << "\n";
    }
}
