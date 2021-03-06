/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     |
    \\  /    A nd           | For copyright notice see file Copyright
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Description
    Class for handling debugging switches.

\*---------------------------------------------------------------------------*/

#include "debug.H"
#include "dictionary.H"
#include "IFstream.H"
#include "OSspecific.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace debug
{

//! @cond ignoreDocumentation - local scope
dictionary* controlDictPtr_(NULL);
dictionary* debugSwitchesPtr_(NULL);
dictionary* infoSwitchesPtr_(NULL);
dictionary* optimisationSwitchesPtr_(NULL);
dictionary* tolerancesPtr_(NULL);

// to ensure controlDictPtr_ is deleted at the end of the run
class deleteControlDictPtr
{
public:

    deleteControlDictPtr()
    {}

    ~deleteControlDictPtr()
    {
        if (controlDictPtr_)
        {
            delete controlDictPtr_;
            controlDictPtr_ = 0;
        }
    }
};

deleteControlDictPtr deleteControlDictPtr_;
//! @endcond ignoreDocumentation


} // End namespace debug
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::dictionary& Foam::debug::controlDict()
{
    if (!controlDictPtr_)
    {
        // Allow users to override the location of the global controlDict
        // dictionary using an environment variable. Using this environment
        // variable, one can assign a different global controlDict for each
        // case, without having to modify the "default" ones.
        fileName globControlDictFileName = getEnv("FOAM_GLOBAL_CONTROLDICT");

        // Fallback to default locations if filename is empty or not valid
        if( ! isFile(globControlDictFileName) )
            globControlDictFileName = findEtcFile("controlDict", true);

        controlDictPtr_ = new dictionary
        (
            IFstream(globControlDictFileName)()
        );
    }

    return *controlDictPtr_;
}


Foam::dictionary& Foam::debug::switchSet
(
    const char* subDictName,
    dictionary*& subDictPtr
)
{
    if (!subDictPtr)
    {
        entry* ePtr = controlDict().lookupEntryPtr
        (
            subDictName, false, false
        );

        if (!ePtr || !ePtr->isDict())
        {
            cerr<< "debug::switchSet(const char*, dictionary*&):\n"
                << "    Cannot find " <<  subDictName << " in dictionary "
                << controlDict().name().c_str()
                << std::endl << std::endl;

            ::exit(1);
        }

        subDictPtr = &ePtr->dict();
    }

    return *subDictPtr;
}


Foam::dictionary& Foam::debug::debugSwitches()
{
    return switchSet("DebugSwitches", debugSwitchesPtr_);
}


Foam::dictionary& Foam::debug::infoSwitches()
{
    return switchSet("InfoSwitches", infoSwitchesPtr_);
}


Foam::dictionary& Foam::debug::optimisationSwitches()
{
    return switchSet("OptimisationSwitches", optimisationSwitchesPtr_);
}


Foam::dictionary& Foam::debug::tolerances()
{
    return switchSet("Tolerances", tolerancesPtr_);
}


int Foam::debug::debugSwitch(const char* name, const int defaultValue)
{
    return debugSwitches().lookupOrAddDefault
    (
        name, defaultValue, false, false
    );
}


int Foam::debug::infoSwitch(const char* name, const int defaultValue)
{
    return infoSwitches().lookupOrAddDefault
    (
        name, defaultValue, false, false
    );
}


int Foam::debug::optimisationSwitch(const char* name, const int defaultValue)
{
    return optimisationSwitches().lookupOrAddDefault
    (
        name, defaultValue, false, false
    );
}


double Foam::debug::tolerances
(
    const char* name,
    const double defaultValue
)
{
    return tolerances().lookupOrAddDefault
    (
        name, defaultValue, false, false
    );
}


// ************************************************************************* //
