************* SIMROBOT TOOLBOX for MATLAB 5 readme file *************
****************** (c) 2000-2001, Jakub Hrabec **********************

This is the first non-BETA version of the toolbox. 


CHECK FOR AN UPDATE AT:
http://www.fee.vutbr.cz/robotics/simulations/amrt

------------------------------
Version 0.95beta
+ First release of the toolbox
------------------------------
Version 0.96beta
+ Changes in Simulator
	+ Added 'Edit ..' into robots context menu
	+ 'Run' menu now has two lines - Simulation (Ctrl+A) and Replay (Ctrl+R)
	+ It is possible to edit simulation parameters
- Saving bug fixed (appeared in special cases)
+ DLLs for Windows NT included
------------------------------
Version 0.98beta
- Some minor bugs fixed
------------------------------
Version 0.99beta
Changes in Simulator:
- Saving-replay-bug fixed (replay was not saved properly)
- After opening a new file in Simulator, the step counter now displays "Step" instead of the old counter state
+ 'Draw tracks' (Ctrl+D) option added (in Run and context menus) - draws tracks of the robots (available after simulation/replay)
- In context menu, Display Parameters option changed to Parameters > (the same as in window toolbar menu) + bug fixes related to this
- Bugs in enabling/disabling menus after load/save commands fixed
------------------------------
Version 1.0beta (Unreleased, but changes are valid for version 1.1beta)
+ !!Header of the algorithm files changed - added one parameter called 'step' (actual simulation step). See changes.txt for more information.
+ Added "memory management" menu into Simulator (right-click on a robot ...)
- 'readlaser' and 'readusonic' function changed (optimized for speed)
- Removed unnecessary commented code from m-files
------------------------------
Version 1.1beta
+ STOP MENU ADDED (CTRL+T) into Simulator - this is the main difference between older versions and v1.1beta. See changes.txt for more information.
+ Editor 
	- it is not possible to save/run an empty simulation (empty environment without robots)
	- in window toolbar menu, 'Run' option has now 'Run  Ctrl+R' submenu (to prevent
	unwanted activation)
+ Added input checking for Steps limit and Step time
+ Some minor changes (warning messages, etc.)
------------------------------
Version 1.11beta
- Incorrect version of invmodel.m replaced (returned wheel frequencies instead of angular velocities)
- Simulator Window title did not change when the simulation/replay ended after steps limit achievement
- Simulation always saved as replay in Simulator
- Known bug - it is necessary to press Ctrl+T shortcut for Stop command repeatedly to make it work on some OS/Matlab version combinations
-----------------------------
Version 1.2
+ Units of length defined in the toolbox - please refer to User's manual and changes.txt
+ File structure checking added (when opening the simulation/replay file)
- Bug fix in mmodel.m function
+ 'Angle of scanning' renamed to 'Beamwidth' in Sensorial subsystem editor (in degrees)