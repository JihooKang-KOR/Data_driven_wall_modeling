label surfaceID(0);
forAll (mesh.boundary(), patchI)
{
    if (mesh.boundary()[patchI].name() == "bottomWall")
    {
        surfaceID = patchI;
    }
}

// Employ a variable nu_ in order to use scalar instead of dimensionedScalar
const scalar nu_ = nu.value();

// Interpolate face values from cell center velocities
surfaceVectorField U_sngrad = fvc::snGrad(U);

// Cell face coordinates for y-axis
const vectorField y_cf(mesh.Cf().internalField());

string filename = "extractData.csv";

OFstream outputFile(filename);
outputFile.precision(12);
outputFile << "y_face";

// uTau value at the first cell center for the first iteration
scalar maxUtau = Foam::sqrt(nu_*0.5*(mag(U_sngrad.boundaryField()[surfaceID][0])
                 + U_sngrad[0].x()));

// Input feature 1 : y-coordinate at the faces
forAll(y_cf, faceI)
{
    if (y_cf[faceI].y()*maxUtau/nu_ <= 400) // yPlus <= 400
    {
        outputFile << ", " << y_cf[faceI].y();
    }
}
outputFile << "\n";
