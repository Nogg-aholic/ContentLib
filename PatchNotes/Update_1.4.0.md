SML3.7/server support. Further testing is still required to be sure it's working properly.




This update brought to you by Robb and Th3Fanbus.
If you enjoy my work, please consider my [completely optional tip jar](https://ko-fi.com/robb4). (Robb)

## New Stuff

- Initial revision of Linux + Windows server support
  - Remember that host and all clients must have the same ContentLib scripts present
    Also, another mod not properly ensuring that generated content is created on both clients and hosts could cause desyncs!
  - Thanks Th3Fanbus for fixing clang compiler related issues to help make this possible
- Log a warning when a null producer is encountered when analyzing recipes from other mods

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
