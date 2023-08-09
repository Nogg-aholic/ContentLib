Update 8 support. Switch to UClass from UDynamicClass internally.




This update hasn't been tested extensively, so please be sure to report any issues you encounter on the discord.

## Changed Stuff

- ContentLib dynamically created classes are now of type UClass instead of UDynamicClass because UDynamicClass was removed
- Now uses SML FClassGenerator::GenerateSimpleClass instead of a copy-pasted version of it
  - I think Nog copied it to include it in ContentLib before it was added to SML

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
  <!-- - TODO use array sort on priority? -->
- Hot reloading of schematics does not seem to work at all
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet
