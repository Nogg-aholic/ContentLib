Fix rare crashes caused by trying to parse invalid or absent contents of other mods.




## New Stuff

- New UE logging category LogContentLibAssetParsing, you'll have to [turn it on in Engine.ini if you want to see it](https://forums.unrealengine.com/t/how-do-i-show-log-entries-with-log-verbosity/284695)
- Warning message if you try to load a class path that doesn't end in `_C` (for example, in patch targets)

## Bugfixes

- Fixed that ContentLib could hard crash your game if a mod was loaded that had assets with broken parent classes
  - Now reported as a log message: `Somehow the parent of asset %s is None, report this to that mod's author`
  - Thank you Borketh for helping me track this down with Satisfactory Explosions
- Fix crashes caused by parsing a mod referencing content from a mod that is not installed.
  - Thank you Aquilla for helping me track this down with Colony 2 and FICSIT Farming
- Fixed that the game would hard crash if you tried to use a double slash in a patch target
  - Now logs a message in the format: `Detected a double slash in a class path; valid Unreal Engine paths can't have double slashes. Path is: PATH`
  - Thank you Vas for finding this crash

## Known Bugs

Nothing new in this update, yet.

(Old bugs below)

- Schematics are processed in alphabetical order, so if you want a ContentLib defined schematic to depend on another one, you must have the *dependee* schematic file name come alphabetically before than the *depender*. An easy way to do this is with numbers at the start of the file name. This will be fixed eventually, but for now, this workaround should help. Thanks McGalleon!
- Hot reloading of schematics does not seem to work at all
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet
