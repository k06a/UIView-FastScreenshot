//
//  UIView+FastScreenshot.m
//  FastScreenshotDemo
//
//  Created by Anton Bukov on 27.08.13.
//  Copyright (c) 2013 Anton Bukov. All rights reserved.
//

#import "IOSurface.h"
#import "UIView+FastScreenshot.h"

@implementation UIView (FastScreenshot)

+ (UIImage *)screenshot
{
    CGSize screenSize = [UIScreen mainScreen].bounds.size;
    NSInteger width = screenSize.width * [UIScreen mainScreen].scale;
    NSInteger height = screenSize.height * [UIScreen mainScreen].scale;
    NSInteger bytesPerElement = 4;
    NSInteger bytesPerRow = (bytesPerElement * width);
    
    NSDictionary *properties = @{
        (__bridge NSString *)kIOSurfaceIsGlobal:@YES,
        (__bridge NSString *)kIOSurfaceBytesPerElement:@(bytesPerElement),
        (__bridge NSString *)kIOSurfaceBytesPerRow:@(bytesPerRow),
        (__bridge NSString *)kIOSurfaceWidth:@(width),
        (__bridge NSString *)kIOSurfaceHeight:@(height),
        (__bridge NSString *)kIOSurfacePixelFormat:@(0x42475241),//'ARGB'
        (__bridge NSString *)kIOSurfaceAllocSize:@(bytesPerRow * height)
    };
    
    IOSurfaceRef surface = IOSurfaceCreate((__bridge CFDictionaryRef)properties);
    
    IOSurfaceLock(surface, 0, nil);
    // Take currently displayed image from the LCD
    CARenderServerRenderDisplay(0, CFSTR("LCD"), surface, 0, 0);
    // Unlock the surface
    IOSurfaceUnlock(surface, 0, 0);
    
    // Make a raw memory copy of the surface
    void * baseAddr = IOSurfaceGetBaseAddress(surface);
    NSInteger totalBytes = bytesPerRow * height;
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, baseAddr, totalBytes, NULL);
    CGImageRef imageRef = CGImageCreate(width, height, 8, 8 * bytesPerElement, 4 * width,
                                        CGColorSpaceCreateDeviceRGB(),
                                        kCGBitmapByteOrder32Host | kCGImageAlphaFirst,
                                        provider, NULL, false, kCGRenderingIntentDefault);
    
    UIImage * image = [UIImage imageWithCGImage:imageRef];
    
    CGImageRelease(imageRef);
    CFRelease(surface);
    
    return image;
}

@end
