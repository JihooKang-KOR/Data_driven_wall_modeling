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

// Get IDs of secondary adjacent cells
labelList secAdjacentCellIDs(adjacentCellIDs.size());

label globFace = -1;
forAll (oppFaceIDs, faceI)
{
    globFace = oppFaceIDs[faceI];

    if (mesh.owner()[globFace] == adjacentCellIDs[faceI])
    {
        secAdjacentCellIDs[faceI] = mesh.neighbour()[globFace];
    }
    else
    {
        secAdjacentCellIDs[faceI] = mesh.owner()[globFace];
    }
}

// Get IDs of faces of the cell that are opposite to the opponent face
labelList secOppFaceIDs(oppFaceIDs.size());

forAll (oppFaceIDs, faceI)
{
    secOppFaceIDs[faceI] =
        mesh.cells()[secAdjacentCellIDs[faceI]].opposingFaceLabel
        (
            oppFaceIDs[faceI],mesh.faces()
        );
}

// Get IDs of third adjacent cells
labelList thirdAdjacentCellIDs(adjacentCellIDs.size());

label globFaceThird = -1;
forAll (secOppFaceIDs, faceI)
{
    globFaceThird = secOppFaceIDs[faceI];
    
    if (mesh.owner()[globFaceThird] == secAdjacentCellIDs[faceI])
    {
        thirdAdjacentCellIDs[faceI] = mesh.neighbour()[globFaceThird];
    }
    else
    {
        thirdAdjacentCellIDs[faceI] = mesh.owner()[globFaceThird];
    }
}

// area of boundary faces
const scalarField magSf(mag(mesh.Sf().boundaryField()[surfaceID]));
