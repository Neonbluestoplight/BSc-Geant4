# Silicon Detector simulation using Geant4

The work carried out encompasses the project condcted for my BSc in Physics at the University of York. The dissertation produced was titled "Designing a Silicon PIN diode array using Geant4 Monte-Carlo simulations" submitted in May 2023

The detector setup is implemented from the B4c example in the Geant4 basic example set. It simulates a simple sampling Calorimeter setup and data scoring is done via hits and sensitive detectors. 

The primary generator was modified from a single particle beam to an isotropic source of alpha particles at a pre-specified distance from the detector geometry. The README is also a modified version of the [example B4 README](https://gitlab.cern.ch/geant4/geant4/-/blob/master/examples/basic/B4/README).

Prior commits contained an isotropy directory alongside a detector directory. The code specific to both has now been incorporated into a common directory. 

## Installation
The application requires Geant4 to be installed prior. You can find the installation instructions [here](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/)

This application can be built using CMAKE from a separate build folder via
```bash
cmake <PATH-TO-FOLDER>
```
where `<PATH-TO-FOLDER>` is the path of this folder in the local system. Then the application can be 
made using

```shell
  make -j<Number-of-cores> # eg: make -j4 for 4 cores
```

an example of a build folder is also provided.

## Geometry Definition

The geometry is constructed in B4c::DetectorConstruction class. The calorimeter is a box made of a given number of layers. A layer consists of an absorber plate and of a detection gap. The layer is replicated.

Four parameters define the geometry of the calorimeter :
* the thickness of an absorber plate,
* the thickness of a gap,
* the number of layers
* the transverse size of the calorimeter (the entrance face is a square).

In addition, a global, uniform, and transverse magnetic field can be applied using `G4GlobalMagFieldMessenger`, instantiated in
```C++
B4c::DetectorConstruction::ConstructSDandField()
```
with a non zero field value or via interactive commands, such as:
```bash
/globalField/setValue 0.2 0 0 tesla
```

```
        |<-------layer 0------->|             |<-------layer N------->|
        |                       |             |                       |
        ===============================================================
        ||              |       ||           ||              |       ||
        ||              |       || N layers  ||              |       ||
 beam   ||   absorber   |  gap  ||.........->||   absorber   |  gap  ||
======> ||              |       ||           ||              |       ||
        ||              |       ||           ||              |       ||
        ===============================================================
```

In this case, only one gap and absorber layer (i.e. Layer 0) is utilised to simulate a detector and a backing plate.

## Physics List

The particle's type and the physic processes which will be available in this example are set in the FTFP_BERT physics list. This physics list requires data files for electromagnetic and hadronic processes.

See more on installation of the datasets in Geant4 Installation Guide v11.1:
* [Postinstall Setup](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/postinstall.html)
* [Environment Variables for Datasets](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/postinstall.html#environment-variables-for-datasets)

The datasets: 
* `G4LEDATA`
* `G4LEVELGAMMADATA`
* `G4SAIDXSDATA`
* `G4ENSDFSTATEDATA`

Are mandatory for this example.

In addition the build-in interactive command:
```
/process/(in)activate processName
```
allows to activate/inactivate the processes one by one.

## Action Initialization

The `B4c::ActionInitialization` class instantiates and registers to Geant4 kernel all user action classes.

While in sequential mode the action classes are instatiated just once, via invoking the method:
```c++
B4c::ActionInitialization::Build()
```
in multi-threading mode the same method is invoked for each thread worker and so all user action classes are defined thread-local.

A run action class is instantiated both thread-local and global that's why its instance is created also in the method:
```c++
  B4c::ActionInitialization::BuildForMaster()
```
which is invoked only in multi-threading mode.

## Primary generator

The primary particle source is an isotropic source of alpha particles of 50 MeV energy by default. This is implemented using a procedure found in the `TestEm4::PrimaryGeneratorAction::PrimaryGeneratorAction()` in [Example TestEm4](https://gitlab.cern.ch/geant4/geant4/-/tree/master/examples/extended/electromagnetic/TestEm4)

The type of the particle and its energy are set in the `B4::PrimaryGeneratorAction` class, and can be changed via the G4 built-in commands of the `G4ParticleGun` class (see the macros provided with this example).

## Runs and Events

A run is a set of events.
The user can choose the frequency of printing via the Geant4 interactive
command, for example:
```
/run/printProgress 100
```

## Detector response

The energy deposit, track lengths and (X, Y, Z) coordinates of the charged particles are recorded on an event by event basis in the Absober and Gap layers. 

The method of data scoring is implemented in 4 ways in B4c. The method relevant to the project is given below:

### Variant c: Hits and Sensitive detectors

The physics quantities are accounted using the hits and sensitive detectors framework defined in the Geant4 kernel. The physics quantities are stored in B4c::CalorHit via two B4c::CalorimeterSD objects, one associated with the Absorber volume and another one with Gap in
```c++
  B4c::DetectorConstruction::ConstructSDandField()
```
Contrary to the B2 example (Tracker) where a new hit is created with each track passing the sensitive volume (in the calorimeter), only one hit is created for each calorimeter layer and one more hit to account for the total quantities in all layers. In addition to the variants B4a and B4b, the quantities per each layer are also available in addition to the total quantities.

## Histograms

The analysis tools are used to accumulate statistics and compute the dispersion of the energy deposit and track lengths of the charged particles. H1D histograms are created in B4c::RunAction::RunAction() for the following quantities:

* Energy deposit in absorber
* Energy deposit in gap
* Track length in absorber
* Track length in gap
* X position in absorber
* Y position in absorber
* Z position in absorber
* X position in gap
* Y position in gap
* Z position in gap

The same values are also saved in an ntuple.

The histograms and the ntuple are saved in the output file in a format according to a specified file extension, the default in this example is ROOT.

The accumulated statistic and computed dispersion is printed at the end of run, in `B4::RunAction::EndOfRunAction()`. When running in multi-threading mode, the histograms and the ntuple accumulated on threads are merged in a single output file. While merging of histograms is performed by default, merging of ntuples is explicitly activated in the `B4::RunAction` constructor.

The ROOT histograms and ntuple can be plotted with ROOT using the plotHisto.C and plotNtuple.C macros.

## How to run

This example handles the program arguments in a new way. It can be run with the following optional arguments:
```
% exampleB4c [-m macro ] [-u UIsession] [-t nThreads] [-vDefault]
```

The `-vDefault` option will activate using the default Geant4 stepping verbose class (`G4SteppingVerbose`) instead of the enhanced stepping verbose with best units (`G4SteppingVerboseWithUnits`) used in the example by default.

The `-t` option is available only in multi-threading mode and allows the user to override the Geant4 default number of threads. The number of threads can be also set via G4FORCENUMBEROFTHREADS environment variable which has the top priority.

* Execute exampleB4c in the 'interactive mode' **with visualization**
  ```bash
    % exampleB4c
  ```
  and type in the commands from run1.mac line by line:
  ```bash
    Idle> /tracking/verbose 1
    Idle> /run/beamOn 1
    Idle> ...
    Idle> exit
  ```
  or
  ```bash
    Idle> /control/execute run1.mac
    ....
    Idle> exit
  ```

* Execute exampleB4c in the 'batch' mode from macro files **without visualization**
  ```
    % exampleB4c -m run2.mac
    % exampleB4c -m exampleB4.in > exampleB4.out
  ```

* Execute exampleB4c in the 'interactive mode' with a selected UI session, e.g. tcsh
  ```
    % exampleB4c -u tcsh
  ```

The following paragraphs are common to all basic examples

### Visualisation

The visualization manager is set via the G4VisExecutive class in the main() function in exampleB4c.cc. The initialisation of the drawing is done via a set of /vis/ commands in the macro vis.mac. This macro is automatically read from the main function when the example is used in interactive running mode.

By default, vis.mac opens an OpenGL viewer (`/vis/open OGL`). The user can change the initial viewer by commenting out this line and instead uncommenting one of the other `/vis/open` statements, such as `HepRepFile` or `DAWNFILE` (which produce files that can be viewed with the HepRApp and DAWN viewers, respectively). 


> ### Note
> One can always open new viewers at any time from the command line. For example, if you already have a view in, say, an OpenGL window with a name "viewer-0", then
> ```
> /vis/open DAWNFILE
> ```
> then to get the same view
> ```
>   /vis/viewer/copyView viewer-0
> ```
> or to get the same view *plus* scene-modifications
> ```
>   /vis/viewer/set/all viewer-0
> ```
> then to see the result
> ```
>   /vis/viewer/flush
> ```
>
> The `DAWNFILE`, `HepRepFile` drivers are always available (since they require no external libraries), but the OGL driver requires that the Geant4 libraries have been built with the OpenGL option.

For more information on visualization, including information on how to install and run DAWN, OpenGL and HepRApp, see the visualization tutorials, for example, `https://conferences.fnal.gov/g4tutorial/g4cd/Documentation/Visualization/G4[VIS]Tutorial/G4[VIS]Tutorial.html`

(Where [VIS] can be replaced by DAWN, OpenGL. HepRApp not currently available on above site or geant4 SLAC)

The tracks are automatically drawn at the end of each event, accumulated
for all events and erased at the beginning of the next run.

### USER INTERFACES

The user command interface is set via the G4UIExecutive class in the main() function in `exampleB4c.cc`

The selection of the user command interface is then done automatically according to the Geant4 configuration or it can be done explicitly via the third argument of the G4UIExecutive constructor (see `exampleB4c.cc`).
