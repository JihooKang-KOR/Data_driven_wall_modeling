// This header file is created by referring to 
// https://github.com/AndreWeiner/phd_openfoam/blob/master/solvers/sgsSpeciesFoam/findCellFaceIDs.H

// Required geometrical information
label surfaceID(0);
forAll (mesh.boundary(), patchI)
{
    if (mesh.boundary()[patchI].name() == "bottomWall")
    {
        surfaceID = patchI;
    }
}

const polyPatch& bouPatch = mesh.Cf().boundaryField()[surfaceID].patch().patch();

// Get global IDs of faces in boundary patch
labelList patchFaceIDs(bouPatch.size());
forAll (bouPatch,faceI)
{
    patchFaceIDs[faceI] = bouPatch.start()+faceI;
}

// Get IDs of primary adjacent cells
const labelList& adjacentCellIDs = bouPatch.faceCells();

// Get IDs of faces of the cell that are opposite to the patch face
labelList oppFaceIDs(patchFaceIDs.size());

forAll (patchFaceIDs, faceI)
{
    oppFaceIDs[faceI] =
        mesh.cells()[adjacentCellIDs[faceI]].opposingFaceLabel
        (
            patchFaceIDs[faceI],mesh.faces()
        );
}
