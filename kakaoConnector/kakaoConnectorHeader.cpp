//
//  kakaoConnectoHeader.m
//  kTester
//
//  Created by LitQoo on 13. 6. 21..
//
//

#import "kakaoConnectorHeader.h"


#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
@implementation AppController (KakaoConnector)

- (void)kakaoAuthenticationDidChangeWithNotification:(NSNotification *)notification
{
    self.accessToken = [KAAuth sharedAuth].accessToken;
    self.refreshToken = [KAAuth sharedAuth].refreshToken;
    
    NSLog(@"kakaoAuthenticationDidChangeWithNotification");
    // [self showViewController];
}


- (void)setupKakao
{

    
    KAAuth *kakao = [[KAAuth alloc] initWithClientID:kClientID
                                        clientSecret:kClientSecret
                                         redirectURL:kRedirectURL
                                         accessToken:self.accessToken
                                        refreshToken:self.refreshToken];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(kakaoAuthenticationDidChangeWithNotification:)
                                                 name:KAAuthenticationDidChangeNotification
                                               object:kakao];
    [KAAuth setSharedAuth:kakao];
    [kakao release];
}

- (NSString *)accessToken
{
    return [[NSUserDefaults standardUserDefaults] stringForKey:kAccessTokenKey];
}

- (void)setAccessToken:(NSString *)accessToken
{
    NSLog(@"setAccessToken");
    [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:kAccessTokenKey];
}

- (NSString *)refreshToken
{
    return [[NSUserDefaults standardUserDefaults] stringForKey:kRefreshTokenKey];
}

- (void)setRefreshToken:(NSString *)refreshToken
{
    NSLog(@"refreshToken");
    [[NSUserDefaults standardUserDefaults] setObject:refreshToken forKey:kRefreshTokenKey];
}




- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url {
    NSLog(@"application handleOpenURL:(%@)", url);
    return [[KAAuth sharedAuth] handleOpenURL:url];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    NSLog(@"application openURL:(%@) sourceApplication:(%@) annotation:(%@)", url, sourceApplication, annotation);
    return [[KAAuth sharedAuth] handleOpenURL:url];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [KAAuth sharedAuth].deviceToken = [deviceToken description];
    if ( [KAAuth sharedAuth].authenticated)
    {
        [[KALocalUser localUser] registerDeviceTokenWithCompletionHandler:^(NSDictionary *response, NSError *error) {
            
        }];
    }
}

@end
#endif