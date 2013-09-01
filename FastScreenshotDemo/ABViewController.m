//
//  ABViewController.m
//  FastScreenshotDemo
//
//  Created by Anton Bukov on 27.08.13.
//  Copyright (c) 2013 Anton Bukov. All rights reserved.
//

#import "ABViewController.h"
#import "UIView+FastScreenshot.h"

@interface ABViewController ()
@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@end

@implementation ABViewController

+ (UIImage *)imageWithView:(UIView *)view
{
    UIGraphicsBeginImageContextWithOptions(view.bounds.size, view.opaque, 0.0);
    [view.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage * img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (IBAction)takeShot:(id)sender
{
    NSDate * date = [NSDate date];
    self.imageView.image = [self.class imageWithView:self.view];
    NSLog(@"%f sec", [[NSDate date] timeIntervalSinceDate:date]);
}

- (IBAction)takeShotNew:(id)sender
{
    NSDate * date = [NSDate date];
    self.imageView.image = [UIView screenshot];
    NSLog(@"%f sec", [[NSDate date] timeIntervalSinceDate:date]);
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
