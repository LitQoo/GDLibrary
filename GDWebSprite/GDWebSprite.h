//
//  GDWebSprite.h
//  sportsworldcup
//
//  Created by LitQoo on 13. 7. 10..
//
//

#ifndef __sportsworldcup__GDWebSprite__
#define __sportsworldcup__GDWebSprite__

#include <iostream>
#include "cocos2d.h"
#include "curl/curl.h"
#include <pthread.h>
#include <queue>

using namespace cocos2d;
using namespace std;





class GDWebSprite: public cocos2d::CCNode{
public:
    CCSprite *webSprite;
    CCNode *defaultSprite;
    string imageUrl;
    string imageName;
    bool isDown;
    float xScale;
    float yScale;

    static CCSprite* create(string imgUrl, string defaultImg);
    static CCSprite* create(string imgUrl, CCNode *defaultNode);
    static CCSprite* create(string imgUrl, string defaultImg, string imageName);
    static CCSprite* create(string imgUrl, CCNode *defaultNode, string imageName);
    
    
    bool init(string imgUrl, string defaultImg);
    bool init(string imgUrl, CCNode *defaultNode);
    bool init(string imgUrl, string defaultImg, string imageName);
    bool init(string imgUrl, CCNode *defaultNode, string imageName);

    static void removeAllSprite(); //remove all sprite
    
    
    
    static void startDownload(GDWebSprite *newSprite);
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static void* t_function(void * _caller);
    void finishDownload();
    void changeWebSprite(CCTexture2D *pTexture);
    
};

struct GDWebSpriteMemoryStruct {
    char *memory;
    size_t size;
};

class GDWebSpriteManager {
public:
    static GDWebSpriteManager* get()
    {
        
        static GDWebSpriteManager* _ins = 0;
        if(_ins == 0){
            _ins = new GDWebSpriteManager();
        }
        return _ins;
    }
    
    
    GDWebSpriteManager(){
        webImages = new CCArray;
        webImages->init();
        isDownloding=false;
        downloadIndex=0;
        
    }
    
    CCArray* webImages;
    bool isDownloding;
    int downloadIndex;
    std::queue<pair<GDWebSpriteMemoryStruct, int> > chunks;
};

#endif /* defined(__sportsworldcup__GDWebSprite__) */
