Fix for stack size of new items bug caused by the Update 6 `mCachedStackSize` field.




## Bugfixes

- Fixed that newly created items would be unusable because the game would treat them as having a stack size of 0
  - This was caused by the `mCachedStackSize` field introduced by Update 6, which defaults to `0` (a valid but problematic max stack size), but must be `-1` for the game to replace it with the real stack size.
  - Thank you for reporting this McGalleon!

## Known Bugs

Nothing new in this update, yet.

(Old bugs below)

- Schematics are processed in alphabetical order, so if you want a ContentLib defined schematic to depend on another one, you must have the *dependee* schematic file name come alphabetically before than the *depender*. An easy way to do this is with numbers at the start of the file name. This will be fixed eventually, but for now, this workaround should help. Thanks McGalleon!
- Hot reloading of schematics does not seem to work at all
- Custom-defined nuclear fuels might not work?
- Probably more I haven't found yet
