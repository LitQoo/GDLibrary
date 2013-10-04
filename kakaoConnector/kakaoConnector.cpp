#include "kakaoConnector.h"

#include "cocos2d.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#import "JSONKit.h"
#import "KALocalUser+LeaderBoard.h"
#import "KALocalUser.h"
#import "KAAuth.h"
#import "KakaoSDKViewController.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "platform/android/jni/JniHelper.h"
#include "jni.h"
#endif
#include <string>
#include <sstream>
using namespace std;
USING_NS_CC;



#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
extern "C"{
	void Java_com_litqoo_lib_kakaoConnector_SendResultNative(JNIEnv *env, jobject thiz,int _key, jstring datas, bool isFinish)
	{
        
        
        CCLOG("sendresultnative1 %d", _key);
        KakaoDelegator::DeleSel delesel = KakaoDelegator::get()->load(_key);
        jboolean isCopy = JNI_FALSE;
		const char* revStr = env->GetStringUTFChars(datas, &isCopy);
		string throwData = revStr;
        
        CCLOG("sendresultnative1");
        KakaoDelegator::get()->buff.append(throwData);
        
        
        CCLOG("sendresultnative3");
        if(delesel.target != 0 && delesel.selector != 0)
        {
            
            CCLOG("sendresultnative4");
            if(isFinish){
                
                CCLOG("sendresultnative5");
                JsonBox::Value resultData;
                resultData.loadFromString(KakaoDelegator::get()->buff.c_str());
                
                
                JsonBox::Object resultObj = resultData.getObject();
//
                CCLOG("sendresultnative6");
                resultObj["param"] = delesel.param;
                resultObj["callback"] = delesel.callbackParam;
                ((delesel.target)->*(delesel.selector))(resultObj);
                
                
                
                KakaoDelegator::get()->buff="";
                
                CCLOG("sendresultnative7");
            }
        }
        
        if(isFinish) KakaoDelegator::get()->remove(_key);
        
        CCLOG("sendresultnative8");
		env->ReleaseStringUTFChars(datas, revStr);
        
        CCLOG("sendresultnative9");
		return;
	}
}
#endif


void kakaoConnector::removeTarget(CCObject *target){
    KakaoDelegator::get()->removeTarget(target);
}

void kakaoConnector::callFuncMainQueue(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector,void*resultDict){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    NSData *_jdata= [NSJSONSerialization dataWithJSONObject:(NSDictionary *)resultDict options:NSJSONWritingPrettyPrinted error:nil];
    NSString *jsonString = [[NSString alloc] initWithData:_jdata encoding:NSUTF8StringEncoding];
    JsonBox::Value _value;
    _value.loadFromString([jsonString cStringUsingEncoding:NSUTF8StringEncoding]);
    JsonBox::Object resultObj = _value.getObject();
    resultObj["param"] = param;
    resultObj["callback"] = callbackParam;
    dispatch_async(dispatch_get_main_queue(),
                    ^{
                        if(target!=NULL && selector!=NULL)(target->*(selector))(resultObj);
                     }
                    );
#endif
}
bool kakaoConnector::isLogin(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    if([KAAuth sharedAuth].authenticated){
        return true;
    }else{
        return false;
    }
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "isLogin", "()Z")) {
        bool result = t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        return result;
    }
#endif

    return false;
}



void kakaoConnector::clearRegistration(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KAAuth sharedAuth] clearRegistration];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "clearRegistration", "()V")) {
        t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::login(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KAAuth sharedAuth] registerWithCompletionHandler:^(NSError *error) {
        NSMutableDictionary *resultDict = [NSMutableDictionary dictionary];
        if ([KAAuth sharedAuth].authenticated) {
            [resultDict setObject:[NSNumber numberWithInt:0] forKey:@"status"];
        } else {
            [resultDict setObject:[NSNumber numberWithInt:-500] forKey:@"status"];
            [resultDict setObject:[error.userInfo objectForKey:@"NSLocalizedDescription"] forKey:@"message"];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
    }];
    
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "login", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target,selector,param,callbackParam);
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::login(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->login(callbackParam,target, selector);
}


void kakaoConnector::guestLogin(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KAAuth sharedAuth] guestLoginWithCompletionHandler:^(NSError *error) {
     NSMutableDictionary *resultDict = [NSMutableDictionary dictionary];
     if ([KAAuth sharedAuth].isGuestLoggedOn) {
        [resultDict setObject:[NSNumber numberWithInt:0] forKey:@"status"];
     }else{
        [resultDict setObject:[NSNumber numberWithInt:-500] forKey:@"status"];
        [resultDict setObject:[error.userInfo objectForKey:@"NSLocalizedDescription"] forKey:@"message"];
     }
     this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
     }];
    
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCAssert(0, "DO NOT SUPPORT ANDROID");
#endif
}

void kakaoConnector::guestLogin(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->login(callbackParam,target, selector);
}



void kakaoConnector::ageVerification(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    CCLog("dont find method");
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "ageVerification", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::ageVerification(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->ageVerification(param,callbackParam,target, selector);
}

void kakaoConnector::loadLocalUser(JsonBox::Object callbackParam, CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] loadLocalUserWithCompletionHandler:^(NSDictionary *response, NSError *error) {
         NSDictionary *resultDict = response;
         if(error){
             resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
         }
         this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
        
    }];
    
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadLocalUser", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::loadLocalUser(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->loadLocalUser(callbackParam,target,selector);
}


void kakaoConnector::loadFriends(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] loadFriendsWithCompletionHandler:^(NSDictionary *response, NSError *error) { 
        
        NSDictionary *resultDict = response;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadFriends", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::loadFriends(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->loadFriends(callbackParam,target,selector);
}

void kakaoConnector::sendMessage(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    NSString* receiverID =  [NSString stringWithUTF8String:param["receiver_id"].getString().c_str()];
    NSString* message =  [NSString stringWithUTF8String:param["message"].getString().c_str()];
    NSString* executeURLString = [NSString stringWithUTF8String:param["executeurl"].getString().c_str()];
    
    KALinkMessageRequest *request = nil;
    
    request = [KALinkMessageRequest requestWithReceiverID:receiverID
                                                      message:message
                                             executeURLString:executeURLString];
    
    [[KALocalUser localUser] sendLinkMessageWithRequest:request completionHandler:^(BOOL success, NSError *error) {
     
        
        NSDictionary *resultDict;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }else{
            resultDict = [NSDictionary dictionaryWithObject:[NSNumber numberWithInt:0] forKey:@"status"];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
     }];
    
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "sendMessage", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::sendMessage(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->sendMessage(param,callbackParam,target,selector);
}

void kakaoConnector::logout(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] logoutWithCompletionHandler:^(BOOL success, NSError *error) {
        
        NSMutableDictionary *resultDict = [NSMutableDictionary dictionary];
        if (success==true) {
            [resultDict setObject:[NSNumber numberWithInt:0] forKey:@"status"];
        } else {
            [resultDict setObject:[NSNumber numberWithInt:-500] forKey:@"status"];
            [resultDict setObject:[error.userInfo objectForKey:@"NSLocalizedDescription"] forKey:@"message"];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
    }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "logout", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif

}

void kakaoConnector::logout(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->logout(callbackParam,target,selector);
}


void kakaoConnector::unregister(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] unregisterWithCompletionHandler:^(BOOL success, NSError *error) {
        NSMutableDictionary *resultDict = [NSMutableDictionary dictionary];
        if (success==true) {
            [resultDict setObject:[NSNumber numberWithInt:0] forKey:@"status"];
        } else {
            [resultDict setObject:[NSNumber numberWithInt:-500] forKey:@"status"];
            [resultDict setObject:[error.userInfo objectForKey:@"NSLocalizedDescription"] forKey:@"message"];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

    }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "unregister", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::unregister(CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
    this->unregister(param,target,selector);
}

void kakaoConnector::showStoryView(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    #if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    CCLog("show story view");
    NSDictionary *androidMetaInfo = [NSDictionary dictionaryWithObjectsAndKeys:@"android",@"os",
                                     @"custum_var=1",@"executeurl", nil];
    NSDictionary *iOSMetaInfo = [NSDictionary dictionaryWithObjectsAndKeys:@"ios",@"os",
                                 @"custum_var=2",@"executeurl", nil];
    [[KakaoSDKViewController controller] showStoryViewWithImage:[UIImage imageNamed:@"background.png"]
                                                     postString:@"내 친구들과 나누는 이야기!\nhttp://www.kakao.com/story"
                                                  metaInfoArray:[NSArray arrayWithObjects:androidMetaInfo,iOSMetaInfo, nil]
                                              completionHandler:^(BOOL success, NSError *error) {
          NSMutableDictionary *resultDict = [NSMutableDictionary dictionary];
          if (success==true) {
              [resultDict setObject:[NSNumber numberWithInt:0] forKey:@"status"];
          } else {
              [resultDict setObject:[NSNumber numberWithInt:-500] forKey:@"status"];
              [resultDict setObject:[error.userInfo objectForKey:@"NSLocalizedDescription"] forKey:@"message"];
          }
          this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);
                                              }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    
#endif
}

void kakaoConnector::showStoryView(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->showStoryView(param,callbackParam,target,selector);
    
}
void kakaoConnector::sendPaymentInfo(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];

    
    [[KALocalUser localUser] sendPaymentInfo:parameters withCompletionHandler:^(NSDictionary *response, NSError *error) {
        NSDictionary *resultDict = response;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

    }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "sendPaymentInfo", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
    
}

void kakaoConnector::sendPaymentInfo(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->sendPaymentInfo(param, callbackParam,target, selector);
    
}

void kakaoConnector::loadGameInfo(JsonBox::Object callbackParam,cocos2d::CCObject *target, kakaoSelType selector){
    JsonBox::Object param;
    #if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser]  loadGameInfoWithCompletionHandler:^(NSDictionary *response, NSError *error) {
        NSDictionary *resultDict = response;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

    }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadGameInfo", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        CCLog("loadgameinfo %d",_key);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::loadGameInfo(cocos2d::CCObject *target, kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->loadGameInfo(callbackParam,target,selector);
}
void kakaoConnector::loadGameMe(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] loadGameMeWithCompletionHandler:^(NSDictionary *response, NSError *error) {
        NSMutableDictionary *resultDict = [NSMutableDictionary dictionaryWithDictionary:response];
        
        NSData* pudata = [resultDict objectForKey:@"public_data"];
        if(pudata!=nil && pudata.length>0){
            [resultDict setObject:[NSString stringWithUTF8String:(const char *)[pudata bytes]] forKey:@"public_data"];
        }
        NSData* prdata = [resultDict objectForKey:@"private_data"];
        if(prdata!=nil && prdata.length>0){
        [resultDict setObject:[NSString stringWithUTF8String:(const char *)[prdata bytes]] forKey:@"private_data"];
        }
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadGameMe", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}
void kakaoConnector::loadGameMe(CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
    this->loadGameMe(param,target,selector);
}


void kakaoConnector::loadGameFriends(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] loadGameFriendsWithCompletionHandler:^(NSDictionary *response, NSError *error) {
        // data to string 으로 교체
        NSMutableDictionary *newdict = [NSMutableDictionary dictionaryWithDictionary:response];
        if (!error) {
            NSMutableArray *newarray = [NSMutableArray array];
            for(NSDictionary* messageInfo in [response objectForKey:@"app_friends"])
            {
                NSMutableDictionary *msg = [NSMutableDictionary dictionaryWithDictionary:messageInfo];
                
                NSData* data = [messageInfo objectForKey:@"public_data"];
                if( data!=nil && data.length>0 ) {
                    
                    NSString* newStr = [NSString stringWithUTF8String:(const char *)[data bytes]];
                    if( newStr!=nil ){
                        [msg setObject:newStr forKey:@"public_data"];
                    }else [msg setObject:@"" forKey:@"public_data"];
                }else [msg setObject:@"" forKey:@"public_data"];
                
                [newarray addObject:msg];
                
            }
            [newdict setObject:newarray forKey:@"app_friends"];
            
        }
        
        NSDictionary *resultDict = newdict;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadGameFriends", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::loadGameFriends(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->loadGameFriends(callbackParam,target,selector);
}

void kakaoConnector::loadLeaderBoard(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];

    [[KALocalUser localUser] loadLeaderBoardWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error) {
        
        
        ///////// 언젠가 삭제할꺼다 이딴코드를 짜야한다니 user_id... #########################################################
         NSMutableDictionary *newdict = [NSMutableDictionary dictionary];
        if(!error){
        //{
           
            NSMutableArray *newAppFriends = [NSMutableArray array];
            
            NSArray* appFriends = [response objectForKey:@"app_friends"];
            for (NSDictionary* appFriendInfo in appFriends)
            {
                NSMutableDictionary *newAppFrinedInfo = [NSMutableDictionary dictionaryWithDictionary:appFriendInfo];
                [newAppFrinedInfo setObject:[[appFriendInfo objectForKey:@"user_id"] stringValue] forKey:@"user_id"];
                [newAppFriends addObject:newAppFrinedInfo];
            }
            [newdict setObject:newAppFriends forKey:@"app_friends"];
        //}
        }
        
        //######################################################################
        
        NSDictionary *resultDict = newdict;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }

        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

    }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadLeaderBoard", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::loadLeaderBoard(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->loadLeaderBoard(param,callbackParam, target, selector);
}

void kakaoConnector::updateResult(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSUInteger readingOptions = NSJSONReadingAllowFragments | NSJSONReadingMutableContainers;
    NSMutableDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:readingOptions error:nil];
    if([[parameters objectForKey:@"public_data"] isKindOfClass:[NSString class]]){
    NSData* bData = [[parameters objectForKey:@"public_data"] dataUsingEncoding: NSUTF8StringEncoding];
    [parameters setObject:bData forKey:@"public_data"];
    }
    if([[parameters objectForKey:@"private_data"] isKindOfClass:[NSString class]]){
    NSData* bData = [[parameters objectForKey:@"private_data"] dataUsingEncoding: NSUTF8StringEncoding];
    [parameters setObject:bData forKey:@"private_data"];
    }
    
    [[KALocalUser localUser] updateResult:parameters withCompletionHandler:^(NSDictionary *response, NSError *error) {
     

        NSDictionary *resultDict = response;
        if(error){
            resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

    }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "updateResult", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::updateResult(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->updateResult(param, callbackParam,target, selector);
}

void kakaoConnector::updateMe(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSUInteger readingOptions = NSJSONReadingAllowFragments | NSJSONReadingMutableContainers;
    NSMutableDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:readingOptions error:nil];
    if([[parameters objectForKey:@"public_data"] isKindOfClass:[NSString class]]){
        NSData* bData = [[parameters objectForKey:@"public_data"] dataUsingEncoding: NSUTF8StringEncoding];
        [parameters setObject:bData forKey:@"public_data"];
    }
    if([[parameters objectForKey:@"private_data"] isKindOfClass:[NSString class]]){
        NSData* bData = [[parameters objectForKey:@"private_data"] dataUsingEncoding: NSUTF8StringEncoding];
        [parameters setObject:bData forKey:@"private_data"];
    }
    
    [[KALocalUser localUser] updateMeWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error) {


        NSDictionary *resultDict = response;
         if(error){
             resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "updateMe", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::updateMe(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->updateMe(param, callbackParam, target,selector);
}

void kakaoConnector::sendGameMessage(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSUInteger readingOptions = NSJSONReadingAllowFragments | NSJSONReadingMutableContainers;
    NSMutableDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:readingOptions error:nil];
    if([parameters objectForKey:@"data"]){
    NSData* bData = [[parameters objectForKey:@"data"] dataUsingEncoding: NSUTF8StringEncoding];
    [parameters setObject:bData forKey:@"data"];
    }

    [[KALocalUser localUser] sendGameMessageWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error){
    
       
        NSDictionary *resultDict = response;
         if(error){
             resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "sendGameMessage", "(ILjava/lang/String;)V")) {
        CCLog("find method");
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::sendGameMessage(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->sendGameMessage(param, callbackParam,target, selector);
}

void kakaoConnector::loadGameMessages(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
    [[KALocalUser localUser] loadGameMessagesWithCompletionHandler:^(NSDictionary *response, NSError *error) {

        
        // data to string 으로 교체
        NSMutableDictionary *newdict = [NSMutableDictionary dictionary];
        if (!error) {
            NSMutableArray *newarray = [NSMutableArray array];
            for(NSDictionary* messageInfo in [response objectForKey:@"messages"])
            {
                NSMutableDictionary *msg = [NSMutableDictionary dictionaryWithDictionary:messageInfo];
                
                NSData* data = [messageInfo objectForKey:@"data"];
                if( data!=nil && data.length>0 ) {
                    
                    NSString* newStr = [NSString stringWithUTF8String:(const char *)[data bytes]];
                    if( newStr!=nil ){
                        [msg setObject:newStr forKey:@"data"];
                    }else [msg setObject:@"" forKey:@"data"];
                }else [msg setObject:@"" forKey:@"data"];
                
                [newarray addObject:msg];
               
            }
            [newdict setObject:newarray forKey:@"messages"];
            
        }else{
            newdict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        
        
        this->callFuncMainQueue(param,callbackParam,target,selector,newdict);

     }];

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "loadGameMessages", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::loadGameMessages(CCObject *target,kakaoSelType selector)
{
    JsonBox::Object callbackParam;
    this->loadGameMessages(callbackParam,target,selector);
}

void kakaoConnector::acceptMessage(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    
    [[KALocalUser localUser] acceptMessageWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error) {

            NSDictionary *resultDict = response;
            if(error){
                resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
            }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "acceptMessage", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::acceptMessage(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->acceptMessage(param, callbackParam, target,selector);
}

void kakaoConnector::acceptAllMessages(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] acceptAllMessagesWithCompletionHandler:^(NSDictionary *response, NSError *error)
     {
          NSDictionary *resultDict = response;
     if(error){
        resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
     }
         this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "acceptAllMessages", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}

void kakaoConnector::acceptAllMessages(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->acceptAllMessages(callbackParam,target,selector);
}


void kakaoConnector::useHeart(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    
    [[KALocalUser localUser] useHeartWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error) {
     
     NSDictionary *resultDict = response;
     if(error){
                resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
     }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     
     }];

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "useHeart", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::useHeart(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->useHeart(param,callbackParam, target, selector);
    
}

void kakaoConnector::messageBlock(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    
    [[KALocalUser localUser] messageBlockWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error){

        NSDictionary *resultDict = response;
        if(error){
                resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "messageBlock", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::messageBlock(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->messageBlock(param,callbackParam, target, selector);
}

void kakaoConnector::sendInviteMessage(JsonBox::Object param,JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    ostringstream oss;
    oss<<param;
    NSString *jsonString = [NSString stringWithUTF8String:oss.str().c_str()];
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *parameters = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    
    [[KALocalUser localUser] sendInviteMessageWithParameters:parameters withCompletionHandler:^(NSDictionary *response, NSError *error){
       
        NSDictionary *resultDict = response;
        if(error){
                resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "sendInviteMessage", "(ILjava/lang/String;)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        ostringstream oss;
        oss << param;
    	t.env->CallStaticObjectMethod(t.classID, t.methodID,_key, t.env->NewStringUTF(oss.str().c_str()));
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::sendInviteMessage(JsonBox::Object param,CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->sendInviteMessage(param,callbackParam, target, selector);
}

void kakaoConnector::deleteMe(JsonBox::Object callbackParam,CCObject *target,kakaoSelType selector){
    JsonBox::Object param;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [[KALocalUser localUser] deleteMeWithCompletionHandler:^(NSDictionary *response, NSError *error) {
        
        NSDictionary *resultDict = response;
        if(error){
                resultDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:error.code],@"status",[error.userInfo objectForKey:@"NSLocalizedDescription"],@"message",nil];
        }
        this->callFuncMainQueue(param,callbackParam,target,selector,resultDict);

     }];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/litqoo/lib/kakaoConnector", "deleteMe", "(I)V")) {
        int _key =  KakaoDelegator::get()->add(target, selector,param,callbackParam);
        t.env->CallStaticObjectMethod(t.classID, t.methodID,_key);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}


void kakaoConnector::deleteMe(CCObject *target,kakaoSelType selector){
    JsonBox::Object callbackParam;
    this->deleteMe(callbackParam,target,selector);
}
