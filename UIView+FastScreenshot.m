//
//  UIView+FastScreenshot.m
//  FastScreenshotDemo
//
//  Created by Anton Bukov on 27.08.13.
//  Copyright (c) 2013 Anton Bukov. All rights reserved.
//
//  Fix for iPad compatibility found on:
//  https://github.com/coolstar/RecordMyScreen/blob/master/RecordMyScreen/CSScreenRecorder.m

#import "IOSurface.h"
#import "UIView+FastScreenshot.h"

@implementation UIView (FastScreenshot)

+ (UIImage *)screenshot
{
    CGSize screenSize = [UIScreen mainScreen].bounds.size;
    float scale = [UIScreen mainScreen].scale;
    
    NSInteger width, height;
    // setup the width and height of the framebuffer for the device
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        // iPhone frame buffer is Portrait
        width = screenSize.width * scale;
        height = screenSize.height * scale;
    } else {
        // iPad frame buffer is Landscape
        width = screenSize.height * scale;
        height = screenSize.width * scale;
    }
    
    NSInteger bytesPerElement = 4;
    NSInteger bytesPerRow = bytesPerElement * width;
    NSInteger totalBytes = bytesPerRow * height;

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
    CARenderServerRenderDisplay(0, CFSTR("LCD"), surface, 0, 0);
    void * baseAddr = IOSurfaceGetBaseAddress(surface);
    NSData * data = [NSData dataWithBytes:baseAddr length:totalBytes];
    IOSurfaceUnlock(surface, 0, 0);
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data.bytes, data.length, NULL);
    CGImageRef imageRef = CGImageCreate(width, height, 8, 8*bytesPerElement, bytesPerRow,
                                        CGColorSpaceCreateDeviceRGB(),
                                        kCGBitmapByteOrder32Host | kCGImageAlphaFirst,
                                        provider, NULL, false, kCGRenderingIntentDefault);
    
    UIImage * image = [UIImage imageWithCGImage:imageRef];

    CGImageRelease(imageRef);
    CFRelease(surface);
    
    return image;
}

@end
