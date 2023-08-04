
==========================================
==============GANERAL SETUP==============
==========================================
You can recreate CPP-derived version of SlidePuzzle from CPP_SlidePuzzle class.
To get a default CPP-child Blueprint actor ready to use, two additional steps are needed:
1. Find its SpringArmComp and set its Y(Pitch) rotation to 270.0
2. Find EditorPlane component and set its mesh to a preferable one ("Plane" from Engine Content by default).

All CPP-derived presets are derived from CPP_PRESET. You’d better not to change them and make all alterations in their custom children.

I_MENUConnect is used by CPP_SlidePuzzle and its blueprint children.

==============OTHER NOTES==============
By default, due to optimization reasons, Tick is disabled in CPP_SlidePuzzle, BPcpp_SlidePuzzle and globally in Project Settings: Allow Blueprint Tick and Can Blueprints Tick by Default.
