1.2 support




****

_If you enjoy my work, please consider donating to my [completely optional tip jar](https://ko-fi.com/robb4)._

Warning: 1.2 introduces the recipe cost multiplier feature, which has not been thorougly tested with ContentLib. Please report any bugs you encounter on the Discord or the mod's github.

## New Stuff

<!-- cspell:ignore Jarno -->
- 1.2 support (Thanks Darth for some help on the upgrade process)
- Support for [Game Feature Data](https://docs.ficsit.app/satisfactory-modding/latest/Development/Satisfactory/GameFeatureDataAsset.html) mods (Thanks Jarno!)
  - Mods consisting of purely ContentLib JSON files should still be placed in the normal Mods folder because they have no assets

## Changed Stuff

- Removed Heat item form, as the base game has removed it in 1.2
- Cleaned up the internal code for JSON file loading

## Fixed Stuff

- Fixed crash when data types in some JSON fields did not match the correct format

## Info for Developers

VSCode now requires trusting JSON schema URLs before they can be used for validation.
Directions on how to do this have been added to the [Setup page of the docs](https://docs.ficsit.app/contentlib/latest/Tutorials/Setup.html).

ContentLib relies on FindObject with the ANY_PACKAGE specifier
to implement the arbitrary class finding from string functionality.
ANY_PACKAGE is currently deprecated and a future update will change it to search the Asset Registry instead.
It is unknown if or how this will affect other mods using ContentLib's features.
The plan is to try and make the switch seamless.

## Known Bugs

See the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib/issues?q=label%3Abug).
