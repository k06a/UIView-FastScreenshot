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

- (IBAction)takeShot:(id)sender
{
    self.imageView.image = [UIView screenshot];
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
