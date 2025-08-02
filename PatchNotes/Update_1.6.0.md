Satisfactory 1.1 release support. Tested by Jarno.




This update brought to you by Jarno, Robb's computer is broken.

## Info

This update is literally just 1.1 compatibility and nothing else - ContentLib's json system has not been extended to support new 1.0 features like the Gas resource form Energy sub-type. Let me know on the [GitHub](https://github.com/Nogg-aholic/ContentLib/issues) or discord if there's a specific feature you'd like.

## Info for Developers

ContentLib relies on FindObject with the ANY_PACKAGE specifier
to implement the arbitrary class finding from string functionality.
ANY_PACKAGE is currently deprecated and a future update will change it to search the Asset Registry instead.
It is unknown if or how this will affect other mods using ContentLib's features.
The plan is to try and make the switch seamless.

## Known Bugs

See the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib/issues?q=label%3Abug).