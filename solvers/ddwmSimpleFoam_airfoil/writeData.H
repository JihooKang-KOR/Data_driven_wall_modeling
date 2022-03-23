// Calculate skin friction
const surfaceVectorField n = mesh.Sf() / mag(mesh.Sf());
const volSymmTensorField devU = dev(twoSymm(fvc::grad(U)));
const vectorField n_wall = n.boundaryField()[surfaceID];
const symmTensorField devU_wall = devU.boundaryField()[surfaceID];
const vectorField tau = nuEff_wall*(devU_wall & n_wall);
const scalarField skinFric = mag(tau)/(0.5*U_ref*U_ref);

// Calculate pressure coefficient
const scalarField p_wall = p.boundaryField()[surfaceID];
const scalarField pCoeff = (p_wall - p_inf)/(0.5*1.0*U_ref*U_ref);

// Write surface data to file
const vectorField Cf(mesh.Cf().boundaryField()[surfaceID]);

string filename1 = getenv("yp");
string filename2 = "yp_surfaceFields";
//string filename2 = "yp_surfaceFields_iter_";
//string filename3 = std::to_string(count);
string filename4 = ".csv";
//filename1 += filename2 + filename3 + filename4;
filename1 += filename2 + filename4;
string path1 = "yplus_";
string path2 = getenv("yp");
path1 += path2;

OFstream outputFile(path1/filename1);
outputFile.precision(12);
outputFile << "ccx,ccy,nuEff_wall_norm,nuEff_face_norm,nuEff_wall,nuEff_1st_face,wall_slope,face_slope,magUx_face,nut_face,magGradU_wall,magGradU_face,U_tilde,wall_blending,face_blending,Ux_blending,uTau,yPlus1stCent,w,tau_x,tau_y,skinFriction,pressureCoeff,Ux_1st_cell,Uy_1st_cell,Ux_1st_face,Uy_1st_face,p_1st_cell,p_1st_face,scaled_y_face,scaled_avgU,phi_face";

forAll (patchFaceIDs, faceI)
{
    outputFile << "\n";        
    outputFile << Cf[faceI].x() << ", "
                << Cf[faceI].y() << ", "
                << nuEff.boundaryField()[surfaceID][faceI]/nu_ << ", "
                << nuEff[oppFaceIDs[faceI]]/nu_ << ", "
                << nuEff.boundaryField()[surfaceID][faceI] << ", "
                << nuEff[oppFaceIDs[faceI]] << ", "
                << labelAccessor[faceI][0]*U_ref/l_ref << ", "
                << labelAccessor[faceI][1]*U_ref/l_ref << ", "
                << mag(labelAccessor[faceI][2]*(U_ref - U_wall[faceI]) + U_wall[faceI]) << ", "
                << nuEff[oppFaceIDs[faceI]] - nu_ << ", "                
                << magGradU[faceI] << ", "
                << magGradUf[faceI] << ", "
                << U_tilde[faceI] << ", "
                << wall_b[faceI] << ", "
                << face_b[faceI] << ", "
                << Uface_b[faceI] << ", "
                << uTau[faceI] << ", "
                << yPlusCenter[faceI] << ", "
                << w[faceI] << ", "
                << tau[faceI][0] << ", "
                << tau[faceI][1] << ", "
                << skinFric[faceI] << ", "
                << pCoeff[faceI] << ", "
                << U[adjacentCellIDs[faceI]][0] << ", "
                << U[adjacentCellIDs[faceI]][1] << ", "
                << U_face[oppFaceIDs[faceI]][0] << ", "
                << U_face[oppFaceIDs[faceI]][1] << ", "
                << p[adjacentCellIDs[faceI]] << ", "
                << p_face[oppFaceIDs[faceI]] << ", "
                << scaleAccessor[faceI][0] << ", "
                << scaleAccessor[faceI][1] << ", "
                << phi[oppFaceIDs[faceI]];
}

count++;
