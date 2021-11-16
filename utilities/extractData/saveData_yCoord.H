label surfaceID(0);
forAll (mesh.boundary(), patchI)
{
    if (mesh.boundary()[patchI].name() == "bottomWall")
    {
        surfaceID = patchI;
    }
}

// Cell face coordinates for y-axis
const vectorField y_cf(mesh.Cf().internalField());

// Cell center coordinates for y-axis
const vectorField y_cc(mesh.C().internalField());

string filename = "extractData.csv";

OFstream outputFile(filename);
outputFile.precision(12);
outputFile << "y_face";

// Input feature 1 : y-coordinate at the faces
forAll(y_cf, faceI)
{
    if (y_cf[faceI].y() <= 0.005)
    {
        outputFile << ", " << y_cf[faceI].y();
    }
}
outputFile << "\n";
