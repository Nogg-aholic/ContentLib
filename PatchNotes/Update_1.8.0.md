Added the ability to unlock other schematics from a schematic (thanks BlueBeka!)




****

_If you enjoy my work, please consider donating to my [completely optional tip jar](https://ko-fi.com/robb4)._

## New Stuff

<!-- cspell:ignore BlueBeka -->
- Added the ability to unlock other schematics from a schematic (FGUnlockSchematic) (thanks BlueBeka! [PR #22](https://github.com/Nogg-aholic/ContentLib/pull/22))
  - The [schematic JSON Schema](https://github.com/budak7273/ContentLib_Documentation/blob/main/JsonSchemas/CL_Schematic.json) has been updated to show how this is done.
  - `ClearSchematics` field can clear existing schematic unlocks from a milestone
- Visual Kits for all vanilla and modded schematics are now loaded on startup, meaning you can reuse them in your own schematics
  - See [the documentation](https://docs.ficsit.app/contentlib/latest/Features/VisualKits.html) for more information on Visual Kits.
  - Use the `/CLDumpVisualKits` chat command to see them
  - Note that although schematic and item visual kits are represented by the same JSON schema, schematics do not have meshes nor fluid colors.

## Changed Stuff

- Improved performance of `ClearRecipes`
- Improved logging of recipe and schematic unlocks
- Dumped visual kits are now placed in `ContentLib/Output/VisualKits/DumpOutput/` instead of `ContentLib/Configs/ItemVisualKits/DumpOutput/` to avoid confusion

## Info for Developers

ContentLib relies on FindObject with the ANY_PACKAGE specifier
to implement the arbitrary class finding from string functionality.
ANY_PACKAGE is currently deprecated and a future update will change it to search the Asset Registry instead.
It is unknown if or how this will affect other mods using ContentLib's features.
The plan is to try and make the switch seamless.

## Known Bugs

See the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib/issues?q=label%3Abug).
