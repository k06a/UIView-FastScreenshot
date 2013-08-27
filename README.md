UIView-FastScreenshot
=====================

The fastest way to get iOS screenshot
Usage
=====================

####1. Include all headers from project subdir `headers/` 
####2. Add Other Linker Flags:

```
-framework IOSurface -framework CoreSurface -framework IOMobileFramebuffer -framework IOKit
```

If you are using iOS 7, you may have a problem with IOKit: [http://stackoverflow.com/a/17624748/440168]()

####3. Build and Run your project only for Device configuration, not for Simulator
Need to be implemented
=====================

####1. Add some methods to get screenshot of views by cropping whole screenshot
####2. Fix problem iPhone-only apps on iPad take screenshot out of bounds :)
