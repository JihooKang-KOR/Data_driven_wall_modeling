// Employ a variable nu_ in order to use scalar instead of dimensionedScalar
const scalar nu_ = nu.value();

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

// Correction of nuEff at the wall
scalarField& nuEff_wall = nuEff.boundaryFieldRef()[surfaceID];
forAll (patchFaceIDs, faceI)
{
    nuEff_wall[faceI] = nu_*(labelAccessor[faceI][0])/(magGradU[faceI] + ROOTVSMALL);
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

// Check if the activation of the 1st face correction executes for each x-coordinate
boolField actFaceCorr(mesh.Cf().boundaryField()[surfaceID].patch().size());

// Save magGradUf at a certain time step as fixedNumSlope
for (int i = 0; i < 10; i++)
{
    if (count == 30 + 50*i)    
    {
        if (fixedNumSlope.size() != 0)
        {
            std::vector<scalar>().swap(fixedNumSlope);
        }
        forAll (oppFaceIDs, faceI)
        {
            fixedNumSlope.push_back(magGradUf[faceI]);
        }
    }
}

// Correction of nuEff at the first cell face opposite to the wall
forAll (oppFaceIDs, faceI)
{
    scalar monoton = (mag(U[adjacentCellIDs[faceI]]) - mag(U[secAdjacentCellIDs[faceI]]))
                    *(mag(U[secAdjacentCellIDs[faceI]]) - mag(U[thirdAdjacentCellIDs[faceI]]));
    
    if ((magGradUf[faceI] > SMALL) && (monoton > 0))
    {
        nuEff[oppFaceIDs[faceI]] *= (labelAccessor[faceI][1])/(magGradUf[faceI] + ROOTVSMALL);
        actFaceCorr[faceI] = 1;
    }
    else
    {
        actFaceCorr[faceI] = 0;
    }

    /*
    if (count >= 31)
    {
        // Correction by the fixed velocity gradient value at the 1st face
        //nuEff[oppFaceIDs[faceI]] *= (labelAccessor[faceI][1])/(fixedNumSlope[faceI] + ROOTVSMALL);
        actFaceCorr[faceI] = 1;
    }
    else
    {
        //nuEff[oppFaceIDs[faceI]] *= (labelAccessor[faceI][1])/(magGradUf[faceI] + ROOTVSMALL);
        actFaceCorr[faceI] = 0;
    }*/
}


// write surface data to file
const vectorField Cf(mesh.Cf().boundaryField()[surfaceID]);

string filename1 = getenv("yp");
//string filename2 = "yp_surfaceFields";
string filename2 = "yp_surfaceFields_iter_";
string filename3 = std::to_string(count);
string filename4 = ".csv";
filename1 += filename2 + filename3 + filename4;
//filename1 += filename2 + filename4;
string path1 = "yplus_";
string path2 = getenv("yp");
path1 += path2;

OFstream outputFile(path1/filename1);
outputFile.precision(12);
outputFile << "ccx,nuEff_wall_norm,nuEff_face_norm,nuEff_wall,nuEff_1st_face,wall_slope,face_slope,nut_face,magGradU_wall,magGradU_face,fixedNumSlope,actFaceCorr,Ux_1st_cell,Uy_1st_cell,Ux_1st_face,Uy_1st_face,p_1st_cell,p_1st_face,scaled_y_face,scaled_avgU";

forAll (patchFaceIDs, faceI)
{
    outputFile << "\n";        
    outputFile << Cf[faceI].x() << ", "
                << nuEff.boundaryField()[surfaceID][faceI]/nu_ << ", "
                << nuEff[oppFaceIDs[faceI]]/nu_ << ", "
                << nuEff.boundaryField()[surfaceID][faceI] << ", "
                << nuEff[oppFaceIDs[faceI]] << ", "
                << labelAccessor[faceI][0] << ", "
                << labelAccessor[faceI][1] << ", "
                << nuEff[oppFaceIDs[faceI]] - nu_ << ", "                
                << magGradU[faceI] << ", "
                << magGradUf[faceI] << ", ";
    if (fixedNumSlope.size() != 0)
    {
        outputFile << fixedNumSlope[faceI] << ", ";
    }
    else
    {
        outputFile << magGradUf[faceI] << ", ";
    }
    outputFile << actFaceCorr[faceI] << ", "
                << U[adjacentCellIDs[faceI]][0] << ", "
                << U[adjacentCellIDs[faceI]][1] << ", "
                << U_face[oppFaceIDs[faceI]][0] << ", "
                << U_face[oppFaceIDs[faceI]][1] << ", "
                << p[adjacentCellIDs[faceI]] << ", "
                << p_face[oppFaceIDs[faceI]] << ", "
                << scaleAccessor[faceI][0] << ", "
                << scaleAccessor[faceI][1];
}

count++;
