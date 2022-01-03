// Required geometrical information
label surfaceID(0);
forAll (mesh.boundary(), patchI)
{
    if (mesh.boundary()[patchI].name() == "bottomWall")
    {
        surfaceID = patchI;
    }
}

// Coordinates of cell centers and faces
const vectorField Cf(mesh.Cf().internalField());
const vectorField Cc(mesh.C().internalField());

// Steps for getting IDs in y-coordinates
int steps = 175;

const polyPatch& bouPatch = mesh.Cf().boundaryField()[surfaceID].patch().patch();

// Get global IDs of faces in boundary patch
labelList patchFaceIDs(bouPatch.size());
forAll (bouPatch,faceI)
{
    patchFaceIDs[faceI] = bouPatch.start()+faceI;
}

// Get IDs of primary adjacent cells
const labelList& adjacentCellIDs = bouPatch.faceCells();

// Vectors for face IDs and cell IDs
std::vector<label> faceIDs;
std::vector<label> cellIDs;

// Get IDs of first cell face
forAll (patchFaceIDs, faceI)
{
    faceIDs.push_back(
        mesh.cells()[adjacentCellIDs[faceI]].opposingFaceLabel
        (
            patchFaceIDs[faceI],mesh.faces()
        )
    );
}

// Get IDs of second cell center
label globFace = -1;
forAll (patchFaceIDs, faceI)
{
    globFace = faceIDs[faceI];

    if (mesh.owner()[globFace] == adjacentCellIDs[faceI])
    {
        cellIDs.push_back(mesh.neighbour()[globFace]);
    }
    else
    {
        cellIDs.push_back(mesh.owner()[globFace]);
    }
}

// Get IDs of cell centers and faces iteratively
for (int i = 0; i < steps; i++)
{
    forAll (patchFaceIDs, faceI)
    {
        faceIDs.push_back(
            mesh.cells()[cellIDs[i*patchFaceIDs.size() + faceI]].opposingFaceLabel
            (
                faceIDs[i*patchFaceIDs.size() + faceI],mesh.faces()
            )
        );
    }

    globFace = -1;
    forAll (patchFaceIDs, faceI)
    {
        globFace = faceIDs[(i+1)*patchFaceIDs.size() + faceI];

        if (mesh.owner()[globFace] == cellIDs[i*patchFaceIDs.size() + faceI])
        {
            cellIDs.push_back(mesh.neighbour()[globFace]);
        }
        else
        {
            cellIDs.push_back(mesh.owner()[globFace]);
        }
    }
}
