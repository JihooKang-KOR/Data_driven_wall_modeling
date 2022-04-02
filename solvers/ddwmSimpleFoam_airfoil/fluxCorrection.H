scalar gammaPhi = 1.0;
scalarField Uface_b(mesh.Cf().boundaryField()[surfaceID].patch().size());
forAll (patchFaceIDs, faceI)
{
    // Typical blending for convective flux    
    Uface_b[faceI] = Foam::pow(U_tilde[faceI], gammaPhi)*mag(labelAccessor[faceI][2]*(U_ref - U_wall[faceI]) + U_wall[faceI]) + Foam::pow((1 - U_tilde[faceI]), gammaPhi)*mag(U_face[oppFaceIDs[faceI]]);
}

// Convective flux correction at the 1st face
forAll (oppFaceIDs, faceI)
{
    if (mesh.Cf().boundaryField()[surfaceID][faceI].x() < 0.998)
    {
        scalar blendPhi = phi[oppFaceIDs[faceI]]*(Uface_b[faceI])/(mag(U_face[oppFaceIDs[faceI]]) + ROOTVSMALL);
        phi[oppFaceIDs[faceI]] = w_face[faceI]*phi[oppFaceIDs[faceI]] + (1 - w_face[faceI])*blendPhi;
    }
}
