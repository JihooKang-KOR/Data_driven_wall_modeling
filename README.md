# Design and implementation of a data-driven wall function for the velocity in RANS simulations

## Introduction
A wall function is used to reduce computational cost by employing an analytical profile. However, this approach still yields mesh-dependent results for coarser meshes. The mesh-dependency comes from the difference between the analytical profile and the numerical profile from a simulation as shown in the video.

The red-dotted lines at the wall and at the first cell face are the actual velocity gradient (slope), and the orange line is the numerical velocity gradient. It is obvious that the slopes are different for coarser meshes. In addition, the actual velocity value (blue graph) and the numerical velocity value (orange graph) at the first cell face are also different for coarser meshes. The wall function approach somehow corrects this discrepancy at the wall, but it does not deal with the difference at the first cell face. Therefore, one of the objectives of this project is to mitigate the discrepancies at the wall as well as the first cell face by correcting the diffusive and the convective fluxes in the momentum equation. Another objective is to create ML models instead of the analytical profile to predict actual values.

## Dependencies
- [SingularityCE](https://sylabs.io/guides/3.8/user-guide/index.html) 3.8.4
- OpenFOAM-v2106
- PyTorch 1.9.0 (only CPU)
- Jupyter-lab 3.2.1
- Numpy 1.21.3
- Pandas 1.3.4
- Matplotlib 3.1.2
- [Flowtorch](https://github.com/FlowModelingControl/flowtorch) 1.0

## Getting started
Compiling the data-driven wall function and running the test cases requires an installation of [OpenFOAM-v2106](https://openfoam.com/). Other releases might work as well but have not been tested.

To build the image at a local system, a docker container should be downloaded as follows.
```
git clone https://github.com/AndreWeiner/of_pytorch_docker.git
cd of_pytorch_docker
docker build -t user_name/of_pytorch:of2106-py1.9.0-cpu -f Dockerfile .
```
Afterward, this docker image can be converted to a singularity image as follows.
```
sudo singularity build of2106-py1.9.0-cpu.sif Singularity.def
```
After the creation of the singularity image, it should be moved to the top folder of this project. If the name of the image is not *of2106.sif*, please revise the name to *of2106.sif*, and then the below instructions can be executed.

The more detailed instruction for this version is available in [this link](https://github.com/AndreWeiner/of_pytorch_docker/tree/6c0f9b9f2077015240bb236c426e4884eb8c500e) written by Dr.-Ing. Andre Weiner. If the latest version is needed, please refer to [this](https://github.com/AndreWeiner/of_pytorch_docker) repository.

## Data generation phase
### 1D channel
1. The case is found in ```./test_cases/channelFlow1D```.
2. To run the test case, create a *run* folder (ignored by version control) only for the first time, copy the case from *test_cases* to *run*, and execute the *Allrun_1D* script.

```
mkdir -p run
cp -r test_cases/channelFlow1D run/
cd run/channelFlow1D
./Allrun_1D
```
3. The simulation consists of two parts, 50 time steps with 0.0002 write interval and 90 time steps with 0.001 write interval, but they are automatically executed by the *Allrun_1D* script.

4. After the simulations are finished, the 1D data extraction utility is to be used, which is found in ```(topFolder)/utilities/extractData```.

5. In the folder, type ```wmake``` to compile the extraction utility.

6. Then, move to ```../../run/channelFlow1D``` folder, and type ```extractData```. This makes a csv file that contains all the features and the labels.

7. In ```(topFolder)/notebooks```, [BestMapping_1DChannel.ipynb](https://github.com/JihooKang-KOR/Data_driven_wall_modeling/blob/main/notebooks/BestMapping_1DChannel.ipynb) needs to be executed to obtain the trained ML models. In the data loading cell in this notebook, the folder name needs to be changed if a specific folder name is used in the *run* folder. In the case of this document, *channelFlow1D_1.388_69.4* should be changed to *channelFlow1D*.
```
field1d_path = run + 'channelFlow1D_1.388_69.4/extractData.csv'
field1d_data = pd.read_csv(field1d_path, delim_whitespace=False)
```

8. After the notebook is executed, the two folders are created to use in OpenFOAM. These models are already in each 2D case if needed.

## Application of trained ML models
### 2D flat plate (No wall function / Standard wall function cases)
1. The case is found in ```./test_cases/turbulentFlatPlate```.
2. To run the test case, copy the case from *test_cases* to *run*, and execute the *Allrun.singularity* script.

```
cp -r test_cases/turbulentFlatPlate run/
cd run/turbulentFlatPlate
./Allrun.singularity
```
3. To change the type of wall function, the *nut* file in ```(topFolder)/0.SpalartAllmaras``` folder needs to be modified. For no wall function, keep the script as it is. For the standard wall function *nutUSpaldingWallFunction*, comment out the line 45 and 46, and comment the line 48 and 49.
```
42    bottomWall
43    {
44        // No wall function
45        type            fixedValue;
46        value           uniform 0.0;
47        // Standard wall function
48        //type            nutUSpaldingWallFunction;
49        //value           $internalField;
50    }
```

4. This simulation is executed as parallel, and therefore MPI is used. To change the setting of the number of CPU cores, edit *decomposeParDict* in ```(topFolder)/system``` folder.

### 2D flat plate (Data-driven wall function)
1. The case is found in ```./test_cases/turbulentFlatPlate_datadriven```.
2. To run the test case, copy the case from *test_cases* to *run*, and execute the *Allrun.singularity_ddwmSimpleFoam_serial* script.

```
cp -r test_cases/turbulentFlatPlate_datadriven run/
cd run/turbulentFlatPlate_datadriven
./Allrun.singularity_ddwmSimpleFoam_serial
```

3. To change the correction method (either the wall correction or the wall/face correction), move to ```(topFolder)/solvers/datadriven_wmSimpleFoam``` and edit *nuEffCorrection.H*. For the wall correction, the line 108 and 109 should be commented out, then no face correction occurs. For the wall/face correction, keep it as it is.
```
105 // Blended 1st face correction with yPlus blending
106 forAll (oppFaceIDs, faceI)
107 {
108     scalar blendFace = nuEff[oppFaceIDs[faceI]]*(labelAccessor[faceI][1]*U_ref/l_ref)/(face_b[faceI] + ROOTVSMALL);
109     nuEff[oppFaceIDs[faceI]] = w_face[faceI]*nuEff[oppFaceIDs[faceI]] + (1 - w_face[faceI])*blendFace;
110 }
```
4. ```wmake``` needs not to be executed because it is already included in the *Allrun.singularity_ddwmSimpleFoam_serial* script.

5. In ```(topFolder)/notebooks```, [PlotCf_2DflatPlate.ipynb](https://github.com/JihooKang-KOR/Data_driven_wall_modeling/blob/main/notebooks/PlotCf_2DflatPlate.ipynb) shows the mesh-dependency of four scenarios that correspond to no wall function, standard wall function, data-driven wall function with wall correction, and data-driven wall function with wall/face correction by plotting skin friction, which was investigated in the thesis. In order to execute the notebook, change the folder name in the notebook accordingly for one's cases.

6. The folders *normalized1DModel* and *scaleModule* made from the ML training phase are already included in the test case, and thus one can run the data-driven wall function cases without executing the notebook [BestMapping_1DChannel.ipynb](https://github.com/JihooKang-KOR/Data_driven_wall_modeling/blob/main/notebooks/BestMapping_1DChannel.ipynb) in the training phase.

7. This simulation is executed as serial due to an indexing issue along the plate in the modified solver.

### 2D NACA-0012 airfoil (No wall function / Standard wall function cases)
1. The case is found in ```./test_cases/airFoil2D_Re3e6_alpha0```. The case is with Re = 3e6 and the angle of attack 0 degree.
2. To run the test case, copy the case from *test_cases* to *run*, and execute the *Allrun_serial.singularity* script.

```
cp -r test_cases/airFoil2D_Re3e6_alpha0 run/
cd run/airFoil2D_Re3e6_alpha0
./Allrun_serial.singularity
```
3. To change the type of wall function, the *nut* file in ```(topFolder)/0.orig``` folder needs to be modified. For no wall function, keep the script as it is. For the standard wall function *nutUSpaldingWallFunction*, comment out the line 38 and 39, and comment the line 40 and 41.
```
36    airfoil
37    {
38        type            fixedValue; // without wall functions
39        value           uniform 0;
40        //type            nutUSpaldingWallFunction; // with the wall function
41        //value           $internalField;
42    }
```
4. The NACA profile for *blockMesh* is from the repository https://github.com/AndreWeiner/naca0012_shock_buffet.

### 2D NACA-0012 airfoil (Data-driven wall function)
1. The case is found in ```./test_cases/airFoil2D_Re3e6_alpha0_datadriven```.
2. To run the test case, copy the case from *test_cases* to *run*, and execute the *Allrun_ddwmSFairfoil_serial.singularity* script.

```
cp -r test_cases/airFoil2D_Re3e6_alpha0_datadriven run/
cd run/airFoil2D_Re3e6_alpha0_datadriven
./Allrun_ddwmSFairfoil_serial.singularity
```

3. To change the correction method (either the wall correction or the wall/face correction), move to ```(topFolder)/solvers/ddwmSimpleFoam_airfoil``` and edit *nuEffCorrection.H* (for diffusive flux) and *fluxCorrection.H* (for convective flux). For the wall correction, the line 138 and 139 for *nuEffCorrection.H* as well as the line 14 and 15 for *fluxCorrection.H* should be commented out, then no face correction occurs. For the wall/face correction, keep it as it is.

*nuEffCorrection.H*
```
133 // Blended 1st face correction with yPlus blending
134 forAll (oppFaceIDs, faceI)
135 {
136     if (mesh.Cf().boundaryField()[surfaceID][faceI].x() < 0.998)
137     {
138         scalar blendFace = nuEff[oppFaceIDs[faceI]]*(labelAccessor[faceI][1]*U_ref/l_ref)/(face_b[faceI] + ROOTVSMALL);
139         nuEff[oppFaceIDs[faceI]] = w_face[faceI]*nuEff[oppFaceIDs[faceI]] + (1 - w_face[faceI])*blendFace;
140     }
141 }
```
*fluxCorrection.H*
```
 9 // Convective flux correction at the 1st face
10 forAll (oppFaceIDs, faceI)
11 {
12     if (mesh.Cf().boundaryField()[surfaceID][faceI].x() < 0.998)
13     {
14         scalar blendPhi = phi[oppFaceIDs[faceI]]*(Uface_b[faceI])/(mag(U_face[oppFaceIDs[faceI]]) + ROOTVSMALL);
15         phi[oppFaceIDs[faceI]] = w_face[faceI]*phi[oppFaceIDs[faceI]] + (1 - w_face[faceI])*blendPhi;
16     }
17 }
```

4. In ```(topFolder)/notebooks```, [PlotCf_Cp_2Dairfoil.ipynb](https://github.com/JihooKang-KOR/Data_driven_wall_modeling/blob/main/notebooks/PlotCf_Cp_2Dairfoil.ipynb) shows the mesh-dependency of four scenarios by plotting skin friction and pressure coefficient, which was investigated in the thesis. In order to execute the notebook, change the folder name in the notebook accordingly for one's cases.

5. The folders *normalized1DModel* and *scaleModule* made from the ML training phase are already included in the test case as well.

## Notebooks
### Used in the thesis
- **BestMapping_1DChannel.ipynb** : Creation of the ML models that used in the thesis.
- **Mapping_1DChannel.ipynb** : Checking uncertainties of each hyper-parameter to find the best setting of hyper-parameters.
- **MeshEffect_Spalding.ipynb** : Showing the effect of coarser meshes based on Spalding's function (the notebook for explaining mesh-dependency).
- **PlotCf_2DflatPlate.ipynb** : Plot skin friction for the flat plate case at Re = 1e7.
- **PlotCf_2DflatPlate_Re3e6.ipynb** : Plot skin friction for the flat plate case at Re = 3e6.
- **PlotCf_2DflatPlate_Re6e6.ipynb** : Plot skin friction for the flat plate case at Re = 6e6.
- **PlotCf_Cp_2Dairfoil.ipynb** : Plot skin friction and pressure coefficient for the airfoil case at Re = 3e6.
- **PlotCf_bestBlendingExponent_2DflatPlate.ipynb** : Finding proper blending exponents *gammaWall* and *gammaFace* for diffusive fluxes.
- **FieldAnalysis_avgNuEff.ipynb** : Field analysis for average effective viscosity over iteration steps and along the plate for y+ = 0.05, 10, and 100.

### Others
- **BestMapping_distanceBasedModel_1D.ipynb** : Creation of the ML models for distance based wall model that is similar to the standard wall function.
- **FieldAnalysis_1stFace.ipynb** : Field analysis over iteration steps such as nuEff and face slopes to find the reason of divergence before the reverse blending is employed for the 1st cell face correction.
- **PlotCf_distanceBasedModel_2DflatPlate.ipynb** : Plot skin friction for the flat plate case at Re = 1e7 with respect to the distance based wall model.
- **PlotResiduals_2Dairfoil.ipynb** : Plot residuals to initially check convergence status for the airfoil case at Re = 3e6.
- **PlotResiduals_2DflatPlate.ipynb** : Plot residuals to initially check convergence status for the flat plate case at Re = 1e7.
- **Plots_monotonicVelLimit.ipynb** : Plot for monotonic velocity limitation setting before the reverse blending is employed for the 1st cell face correction.

## Solvers
### Used in the thesis
- **datadriven_wmSimpleFoam** : The modified solver from *simpleFoam* for the flat plate cases by employing the trained ML models.
- **ddwmSimpleFoam_airfoil** : The modified solver from *simpleFoam* for the airfoil case by employing the trained ML models.

### Other
- **ddwmSimpleFoam_wallCorr** : The distance-based modified solver for the flat plate cases.

## Utilities
### Used in the thesis
- **extractData** : Data extraction utility for the 1D channel flow.

### Others
- **extractData_2D** : Data extraction utility for the 2D flat plate case to compare 1st face slopes between the 1D channel and the 2D flat plate with resolved mesh.
- **extractData_cellCenterVel** : Data extraction utility for the 1D channel flow with respect to the distance-based model.

## Thesis
The thesis for the project : 
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.6590747.svg)](https://doi.org/10.5281/zenodo.6590747)

BibTeX :
```
@misc{kang_jihoo_2022_6590747,
  author       = {Kang, Jihoo},
  title        = {{Design and implementation of a data-driven wall 
                   function for the velocity in RANS simulations}},
  month        = may,
  year         = 2022,
  publisher    = {Zenodo},
  doi          = {10.5281/zenodo.6590747},
  url          = {https://doi.org/10.5281/zenodo.6590747}
}
```