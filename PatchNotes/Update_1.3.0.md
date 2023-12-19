New reflection utility for mod developers. Avoid crashing when another mod registers an invalid subclass of a production machine.




## New Stuff

- New C++/Blueprint Library function: `CL_ExecuteArbitraryFunction(FName functionName, UObject* onObject)`
  - Allows calling (zero-argument) arbitrary functions on arbitrary objects at runtime without having their owning mods installed at editor time
  - Pass the name of a function and an object and it will call that function
  - Return values not supported
  - Be very careful when using this
- New Blueprint Library function: `CL_GetExecutingPackage()`
  - Returns an FString with the mod reference of the mod currently executing the blueprint code
- Additional logging in cases where ContentLib tries to process a null schematic/producer/recipe referenced by another mod
  - Hopefully helps that other mod catch their mistake
- ContentLib No longer requires ContentInspector source code present at editor time to package the mod
  - Uses CL_ExecuteArbitraryFunction to accomplish this

## Bugfixes

- When processing production machines, don't crash when a null subclass is detected, just produce an error message for that mod developer instead.
  - Thanks Th3Fanbus for fixing this!

## Info for Developers

ContentLib relies on FindObject with the ANY_PACKAGE specifier
to implement the arbitrary class finding from string functionality.
ANY_PACKAGE is currently deprecated and a future update will change it to search the Asset Registry instead.
It is unknown if or how this will affect other mods using ContentLib's features.
The plan is to try and make the switch seamless.

## Known Bugs

No new known bugs in this update, yet.

(Old bugs below)

- Schematics are processed in alphabetical order, so if you want a ContentLib defined schematic to depend on another one, you must have the *dependee* schematic file name come alphabetically before than the *depender*. An easy way to do this is with numbers at the start of the file name. This will be fixed eventually, but for now, this workaround should help. Thanks McGalleon!
<!-- TODO use array sort for patch loading priority? -->
- Hot reloading of schematics does not seem to work at all
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet
