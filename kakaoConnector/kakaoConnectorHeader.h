//
//  kakaoConnectoHeader.h
//  kTester
//
//  Created by LitQoo on 13. 6. 21..
//
//

#include "cocos2d.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#import <Foundation/Foundation.h>
#import "Kakao.h"
#import "KALocalUser+LeaderBoard.h"
#import "AppController.h"

static NSString *const kClientID = CLIENT_ID;
static NSString *const kRedirectURL = @"kakao" CLIENT_ID @"://exec";
static NSString *const kClientSecret = @"u8P9b6Le4rUi14q9dfdVhiwJMlyrNO/CElX96drdLPwXWFrUVEbcw4Ke45Ug47krM7yCMIQuogza3sJqvnzlkQ=="; // 발급받은 Client Secret을 입력해 주세요
static NSString *const kAccessTokenKey = @"accessToken";
static NSString *const kRefreshTokenKey = @"refreshToken";



@interface AppController (KakaoConnector)
@property (nonatomic, copy) NSString *accessToken;
@property (nonatomic, copy) NSString *refreshToken;
- (void)setupKakao;
- (void)kakaoAuthenticationDidChangeWithNotification:(NSNotification *)notification;
@end
#endif
