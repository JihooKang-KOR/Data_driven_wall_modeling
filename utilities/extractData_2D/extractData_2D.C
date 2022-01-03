/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    extractData_2D

Description
    Extract data in 2D simulation

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Extract data in 2D flat plate."
    );

    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"    
    
    #include "createFields.H"
    #include "findCellFaceLabels.H"
    #include "saveDataInit.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    
    instantList timeDirs = timeSelector::select0(runTime, args);

    for (int timei = 1; timei < timeDirs.size(); timei++)
    {
        runTime.setTime(timeDirs[timei], timei);
        Info << "Time = " << runTime.timeName() << endl;

        #include "createFields.H"
        #include "saveDataFields.H"
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
