// Employ a variable nu_ in order to use scalar instead of dimensionedScalar
const scalar nu_ = nu.value();
// Reference length
const scalar l_ref = nu_/U_ref;
// Velocity at wall
scalarField U_wall(mag(U.boundaryField()[surfaceID]));

// Interpolate face values for cell center velocities
surfaceVectorField U_face = fvc::interpolate(U);
surfaceVectorField U_sngrad = fvc::snGrad(U);
surfaceScalarField p_face = fvc::interpolate(p);

//***********************************************************************/
// Wall Correction
//***********************************************************************/

// Gradient of the velocity at the wall patch
const scalarField magGradU(mag(U_sngrad.boundaryField()[surfaceID]));

// Distance between the first cell face opposite to the wall and the wall
scalarField dist_fface = mag(mesh.Cf().boundaryField()[surfaceID]);
forAll (oppFaceIDs, faceI)
{
    dist_fface[faceI] = mag(mesh.Cf()[oppFaceIDs[faceI]] 
                    - mesh.Cf().boundaryField()[surfaceID][faceI]);    
}

// Include data-driven turbulence model
#include "datadrivenTurbulenceModel.H"

// yPlus blending
scalarField dist_fcell(mesh.Cf().boundaryField()[surfaceID].patch().size());
forAll (adjacentCellIDs, cellI)
{
    dist_fcell[cellI] = mag(mesh.C()[adjacentCellIDs[cellI]] 
                    - mesh.C().boundaryField()[surfaceID][cellI]);
}
scalarField uTau(mesh.Cf().boundaryField()[surfaceID].patch().size());
scalarField yPlusCenter(mesh.Cf().boundaryField()[surfaceID].patch().size());
scalarField w(mesh.Cf().boundaryField()[surfaceID].patch().size());
forAll (patchFaceIDs, faceI)
{
    uTau[faceI] = Foam::sqrt(nu_*labelAccessor[faceI][0]*U_ref/l_ref);
    yPlusCenter[faceI] = dist_fcell[faceI]*uTau[faceI]/nu_;
    // No y+ blending
    w[faceI] = 0.0;
    // Sigmoid function    
    //w[faceI] = 1/(1 + Foam::exp(-3.0*(5 - yPlusCenter[faceI])));
    //w[faceI] = 1/(1 + Foam::exp(-0.95*(15 - yPlusCenter[faceI])));
}

// Normalization of velocity for blending
scalar gammaWall = 1.0;
scalarField U_tilde(mesh.Cf().boundaryField()[surfaceID].patch().size());
scalarField wall_b(mesh.Cf().boundaryField()[surfaceID].patch().size());
forAll (patchFaceIDs, faceI)
{
    // Between 0 and 1
    U_tilde[faceI] = (mag(U.internalField()[adjacentCellIDs[faceI]]) - U_wall[faceI])/(U_ref - U_wall[faceI]);
    // If U_tilde is greater than 1.0, no blending occurs.
    if (U_tilde[faceI] > 1.0)
    {
        U_tilde[faceI] = 1.0;
    }
    // Blending function for wall : If y+ is larger, more portion of the ML model slopes are applied to the numerator.
    wall_b[faceI] = Foam::pow(U_tilde[faceI], gammaWall)*(labelAccessor[faceI][0]*U_ref/l_ref) + Foam::pow((1 - U_tilde[faceI]), gammaWall)*magGradU[faceI];
}

// Correction of nuEff at the wall
scalarField& nuEff_wall = nuEff.boundaryFieldRef()[surfaceID];

// Blended wall correction with yPlus blending
forAll (patchFaceIDs, faceI)
{
    if (mesh.Cf().boundaryField()[surfaceID][faceI].x() < 0.998)
    {
        scalar blendWall = nu_*wall_b[faceI]/(magGradU[faceI] + ROOTVSMALL);
        nuEff_wall[faceI] = w[faceI]*nuEff_wall[faceI] + (1 - w[faceI])*blendWall;
    }
}

//***********************************************************************/
// First Cell Face Correction
//***********************************************************************/

// Gradient of the velocity at the first cell face opposite to the wall
scalarField tmp_magGradUf(mag(U_sngrad.boundaryField()[surfaceID]));    
forAll (oppFaceIDs, faceI)
{
    // Set an arbitrary value in order to avoid zero denominator at the variable tnuf and 
    // overflow of the exponential term in the function spaldings_law for the first time step
    if (mag(U_sngrad.internalField()[oppFaceIDs[faceI]]) == 0)
    {
        tmp_magGradUf[faceI] = 1e2;
    }        
    else
    {
        tmp_magGradUf[faceI] = mag(U_sngrad.internalField()[oppFaceIDs[faceI]]);
    }        
}
    
scalarField magGradUf(tmp_magGradUf);

scalar gammaFace = 1.0;
scalarField face_b(mesh.Cf().boundaryField()[surfaceID].patch().size());
forAll (patchFaceIDs, faceI)
{
    // Reversed blending function for 1st face : If y+ is smaller, more portion of the ML model slopes are applied to the denominator
    // in order to avoid exceedingly small magGradUf values. (ex. U_tilde = 0 -> no face correction is applied.)
    face_b[faceI] = Foam::pow(U_tilde[faceI], gammaFace)*magGradUf[faceI] + Foam::pow((1 - U_tilde[faceI]), gammaFace)*(labelAccessor[faceI][1]*U_ref/l_ref);
}

// Blended 1st face correction with yPlus blending
forAll (oppFaceIDs, faceI)
{
    if (mesh.Cf().boundaryField()[surfaceID][faceI].x() < 0.998)
    {
        scalar blendFace = nuEff[oppFaceIDs[faceI]]*(labelAccessor[faceI][1]*U_ref/l_ref)/(face_b[faceI] + ROOTVSMALL);
        nuEff[oppFaceIDs[faceI]] = w[faceI]*nuEff[oppFaceIDs[faceI]] + (1 - w[faceI])*blendFace;
    }
}
