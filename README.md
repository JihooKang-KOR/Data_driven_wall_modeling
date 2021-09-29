# Development of data-driven wall functions for turbulent flows

The repository is structured as follows:
- *test_cases* : OpenFOAM simulation setups
- *notebooks* : Jupyter notebooks for post-processing of results and other visualizations

## Getting started

Compiling the data-driven wall function and running the test cases requires an installation of [OpenFOAM-v2012](https://openfoam.com/). Other releases might work as well but have not been tested.

To run a test case, create a *run* folder (ignored by version control), copy the case from *test_cases* to *run*, and execute the *Allrun* script.

```
mkdir -p run
cp -r test_cases/turbulentFlatPlate run/
cd run/turbulentFlatPlate
./Allrun
```

## Singularity and SLURM

[Singularity]() is a container tool that allows making results reproducible and performing simulations, to a large extent, platform independent. The only remaining dependencies are Singularity itself and Open-MPI (see next section for further comments). To build the image, run:

```
sudo singularity build of_v2012.sif docker://andreweiner/of_pytorch:of2012-py1.7.1-cpu
```
To run a simulation with Singularity, use the dedicated *Allrun.singularity* scripts. TU Braunschweig's HPC uses the SLURM scheduler. The repository contains an annotated example *jobscript*. The script expects the Singularity image in the top level directory of this repository and the simulation folder in *run*. To submit a job, run:

```
sbatch jobscript name_of_simulation
```
To show all running jobs of a user, use `squeue -u $USER`. Another helpful command is `quota -s` to check the available disk space.

## References

The starting point of the literature review is as follows:
1. This paper is about a wall-modeling in Large Eddy Simulation (LES). In LES, the whole grid is divided into two parts. On the one hand, a wall-modelled inner layer which is normally not resolved. On the other hand, an outer layer where the typical LES method is applied. In this paper, the inner layer is also resolved which leads to the reduced error rate.
```
@article{doi:10.1063/1.3678331,
author = {Kawai,Soshi  and Larsson,Johan },
title = {Wall-modeling in large eddy simulation: Length scales, grid resolution, and accuracy},
journal = {Physics of Fluids},
volume = {24},
number = {1},
pages = {015105},
year = {2012},
doi = {10.1063/1.3678331}
}
```

2. This is the paper used the LES as well, but related to neural networks (NNs). This work focuses on the extrapolation ability of NNs for higher Reynolds numbers and y+ values. When the inputs (velocity and height) are well normalized, the extrapolation performance of NNs is much better than the case of unnormalized one.
```
@article{PhysRevFluids.4.034602,
  title = {Predictive large-eddy-simulation wall modeling via physics-informed neural networks},
  author = {Yang, X. I. A. and Zafar, S. and Wang, J.-X. and Xiao, H.},
  journal = {Phys. Rev. Fluids},
  volume = {4},
  issue = {3},
  pages = {034602},
  numpages = {22},
  year = {2019},
  month = {Mar},
  publisher = {American Physical Society},
  doi = {10.1103/PhysRevFluids.4.034602},
  url = {https://link.aps.org/doi/10.1103/PhysRevFluids.4.034602}
}
```

3. The work is, however, related to Reynolds-Averaged Navier–Stokes Simulation (RANS). Therefore, papers that are relevant to RANS should be reviewed. This paper used the RANS equation with the Spalart-Allmaras (SA) model via OpenFOAM to simulate the various shapes of airfoils. It focuses on finding the velocity field via Convolutional Neural Networks (CNNs), but the principle of the CNNs is not considered here.
```
@article{doi:10.2514/1.J058291,
author = {Thuerey, Nils and Weißenow, Konstantin and Prantl, Lukas and Hu, Xiangyu},
title = {Deep Learning Methods for Reynolds-Averaged Navier–Stokes Simulations of Airfoil Flows},
journal = {AIAA Journal},
volume = {58},
number = {1},
pages = {25-36},
year = {2020},
doi = {10.2514/1.J058291},
URL = {https://doi.org/10.2514/1.J058291}
}
```

4. This paper is also about NNs for RANS, but focuses on the Reynolds' stress term. Particularly, the deviatoric part of the Reynolds' stress is considered here. When the stress term is reformed as a rotational invariance term, the NN performs well for finding the stress term itself and the velocity field as well.
```
@article{ling_kurzawski_templeton_2016, 
title={Reynolds averaged turbulence modelling using deep neural networks with embedded invariance}, 
volume={807}, 
DOI={10.1017/jfm.2016.615}, 
journal={Journal of Fluid Mechanics}, 
publisher={Cambridge University Press}, 
author={Ling, Julia and Kurzawski, Andrew and Templeton, Jeremy}, 
year={2016}, 
pages={155–166}
}
```
