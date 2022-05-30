# Design and implementation of a data-driven wall function for the velocity in RANS simulations

## Introduction

## Dependencies

## Getting started
Compiling the data-driven wall function and running the test cases requires an installation of [OpenFOAM-v2106](https://openfoam.com/). Other releases might work as well but have not been tested.

The singularity container should be located in the top folder of this project (*of2106.sif*).

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

7. In ```(topFolder)/notebooks```, *BestMapping_1DChannel.ipynb* needs to be executed to obtain the trained ML models. In the data loading cell in this notebook, the folder name needs to be changed if a specific folder name is used in the *run* folder. In the case of this document, *channelFlow1D_1.388_69.4* should be changed to *channelFlow1D*.
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

### 2D airfoil (NACA-0012)

## Notebooks

## Solvers

## Utilities

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