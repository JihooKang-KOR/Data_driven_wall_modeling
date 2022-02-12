label surfaceID(0);
forAll (mesh.boundary(), patchI)
{
    if (mesh.boundary()[patchI].name() == "bottomWall")
    {
        surfaceID = patchI;
    }
}

// Cell center coordinates for y-axis
const vectorField y_cc(mesh.C().internalField());

string filename = "extractData_cellCenterVel.csv";

OFstream outputFile(filename);
outputFile.precision(12);

// y-coordinate at each cell center
outputFile << "y_center";
forAll (y_cc, cellI)
{
    if (y_cc[cellI].y() <= 0.0015)
    {
        outputFile << ", " << y_cc[cellI].y();
    }
}
outputFile << "\n";

// Input feature 1 : approximated y-coordinate at cell faces for each cell center velocity
outputFile << "y_face_approx";
forAll (y_cc, cellI)
{
    if (y_cc[cellI].y() <= 0.0015)
    {
        outputFile << ", " << 2*y_cc[cellI].y();
    }
}
outputFile << "\n";
