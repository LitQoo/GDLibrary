//
//  GDWebSprite.cpp
//  sportsworldcup
//
//  Created by LitQoo on 13. 7. 10..
//
//

#include "GDWebSprite.h"

void GDWebSprite::removeAllSprite(){
    GDWebSpriteManager::get()->webImages->removeAllObjects();
}

void GDWebSprite::startDownload(GDWebSprite *newSprite){
    
    GDWebSpriteManager::get()->webImages->addObject(newSprite);
    //start thread
    
    if(GDWebSpriteManager::get()->isDownloding==false){
        GDWebSpriteManager::get()->isDownloding=true;
        pthread_t p_thread;
        int thr_id;
        // 쓰레드 생성 아규먼트로 1 을 넘긴다.
        thr_id = pthread_create(&p_thread, NULL, t_function, newSprite);
        if (thr_id < 0)
        {
            perror("thread create error : ");
            exit(0);
        }
    }
    
}

size_t GDWebSprite::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    
    size_t realsize = size * nmemb;
    struct GDWebSpriteMemoryStruct *mem = (struct GDWebSpriteMemoryStruct *)userp;
    
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    
    if (mem->memory == NULL) {
        /* out of memory! */
        CCLog("not enough memory (realloc returned NULL)");
        exit(EXIT_FAILURE);
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void* GDWebSprite::t_function(void * _caller)
{
    //FBConnector* caller = (FBConnector*)_caller;
    
    CURL *curl_handle;
    CURLcode retCode = curl_global_init(CURL_GLOBAL_ALL);
    if(retCode != CURLE_OK){
        GDWebSpriteManager::get()->isDownloding=false;
        return NULL;
    }
    curl_handle = curl_easy_init();
    int reDownCnt=0;
    while (1) {
        string pURL;
        if(GDWebSpriteManager::get()->webImages->count()<=GDWebSpriteManager::get()->downloadIndex){
            GDWebSpriteManager::get()->isDownloding=false;
            break;
        }else{
            pURL=((GDWebSprite *)GDWebSpriteManager::get()->webImages->objectAtIndex(GDWebSpriteManager::get()->downloadIndex))->imageUrl;
        }
        
        //CCLog("start downloadIndex : %d", GDWebSpriteManager::get()->downloadIndex);
        
        GDWebSpriteMemoryStruct chunk = {(char*)malloc(1), 0};
        int downI = GDWebSpriteManager::get()->downloadIndex;
        curl_easy_setopt(curl_handle, CURLOPT_URL, pURL.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        if(curl_easy_perform(curl_handle) == CURLE_OK){
            //CCLog("finish downloadIndex : %d", GDWebSpriteManager::get()->downloadIndex);
            GDWebSpriteManager::get()->chunks.push(pair<GDWebSpriteMemoryStruct, int>(chunk, downI));
            CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GDWebSprite::finishDownload), (GDWebSprite *)(GDWebSpriteManager::get()->webImages->objectAtIndex(GDWebSpriteManager::get()->downloadIndex)), 0, false, 0, 0);
            GDWebSpriteManager::get()->downloadIndex++;
        }else{
             free(chunk.memory);
            if(reDownCnt>3){
                reDownCnt=0;
                GDWebSpriteManager::get()->downloadIndex++;
            }else{
                reDownCnt++;
            }
        }
    }
    GDWebSpriteManager::get()->isDownloding=false;
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    return NULL;
}

CCSprite* GDWebSprite::create(string imgUrl, CCNode *defaultNode, string imageName){
    
    if(imgUrl==""){
        return (CCSprite *)defaultNode;
    }
    
    //1. webImages 검사해서 값있으면 그냥 리턴
    for(int i=0;i<GDWebSpriteManager::get()->webImages->count();i++){
        GDWebSprite* image = (GDWebSprite*)GDWebSpriteManager::get()->webImages->objectAtIndex(i);
        if(image->imageUrl==imgUrl || (image->imageName==imageName && imageName!="")){
            if(image->webSprite!=NULL){
                CCNode *newimg = CCNode::create();
                CCSprite* ret = CCSprite::createWithTexture(image->webSprite->getTexture(),image->webSprite->getTextureRect());
                
                ret->setScaleX(image->xScale);
                ret->setScaleY(image->yScale);
                
                newimg->addChild(ret);
                ret->setAnchorPoint(CCPointZero);
                ret->setPosition(CCPointZero);
                ret->setContentSize(CCSizeMake(ret->getContentSize().width*image->xScale,ret->getContentSize().height*image->yScale));
                return (CCSprite *)newimg;
            }
        }
    }
    
    
    //2. 새로운 이미지이면 webImages에 값넣고 스프라잇
    GDWebSprite* _ws = new GDWebSprite;
    _ws->init(imgUrl, defaultNode, imageName);
    _ws->autorelease();
    
    GDWebSprite::startDownload(_ws);
    
    return (CCSprite*)_ws;
}

CCSprite* GDWebSprite::create(string imgUrl, string defaultImg){
    CCNode *defalutNode = CCSprite::create(defaultImg.c_str());
    return GDWebSprite::create(imgUrl,defalutNode,"");
}


CCSprite* GDWebSprite::create(string imgUrl, CCNode* defaultNode){
    return GDWebSprite::create(imgUrl,defaultNode,"");
}

CCSprite* GDWebSprite::create(string imgUrl, string defaultImg, string imageName){
    CCNode *defalutNode = CCSprite::create(defaultImg.c_str());
    return GDWebSprite::create(imgUrl,defalutNode,imageName);
}


bool GDWebSprite::init(string imgUrl, string defaultImg){
    return this->init(imgUrl,defaultSprite,"");
}

bool GDWebSprite::init(string imgUrl, CCNode* defaultNode){
    return this->init(imgUrl,defaultNode,"");
}

bool GDWebSprite::init(string imgUrl, string defaultImg, string imageName){
    defaultSprite = CCSprite::create(defaultImg.c_str());
    return this->init(imgUrl,defaultSprite,imageName);
}

bool GDWebSprite::init(string imgUrl, CCNode *defaultNode, string imageName){
    
    if(!CCNode::init()){
        return false;
    }
    
    defaultSprite = defaultNode;
    this->addChild(defaultSprite,1);
    this->setContentSize(CCSize(defaultSprite->getContentSize().width,defaultSprite->getContentSize().height));
    CCSize s = getContentSize();
    defaultSprite->setAnchorPoint(ccp(0.5,0.5));
    defaultSprite->setPosition(ccp(s.width / 2, s.height / 2));
    this->imageName=imageName;
    this->imageUrl=imgUrl;
    webSprite=NULL;
    isDown=false;
    xScale=1;
    yScale=1;
    return true;
}



void GDWebSprite::finishDownload(){
    if(!GDWebSpriteManager::get()->chunks.empty())
    {
        
        CCImage* img = new CCImage;
        
        CCTexture2D* texture = new CCTexture2D();
        auto chunk_index = GDWebSpriteManager::get()->chunks.front();
        GDWebSpriteManager::get()->chunks.pop();
        
        
        
        
        try {
            
            if(img->initWithImageData(chunk_index.first.memory, (long)chunk_index.first.size, CCImage::kFmtUnKnown) == false)
                throw "..";
            
            if(texture->initWithImage(img) == false)
                throw "..";
            
            //CCLog("showing downimage %d",chunk_index.second);
            
            GDWebSprite *_sprite = (GDWebSprite *)GDWebSpriteManager::get()->webImages->objectAtIndex(chunk_index.second);
            
            _sprite->changeWebSprite(texture);
            
            if(chunk_index.first.memory)
                free(chunk_index.first.memory);
            
            
            texture->release();
            delete img;// in cocos2d-x 1.x
        } catch (...) {
            if(chunk_index.first.memory)
                free(chunk_index.first.memory);
            
            texture->release();
            delete img;// in cocos2d-x 1.x
        }
    }
    
}

void GDWebSprite::changeWebSprite(CCTexture2D *pTexture){
    if(this->isDown)return;
    this->webSprite=CCSprite::createWithTexture(pTexture);
    CCSize s = getContentSize();
    this->xScale = this->defaultSprite->getContentSize().width/this->webSprite->getContentSize().width*this->defaultSprite->getScaleX();
    this->yScale = this->defaultSprite->getContentSize().height/this->webSprite->getContentSize().height*this->defaultSprite->getScaleY();
    this->webSprite->setAnchorPoint(ccp(0.5,0.5));
    this->webSprite->setPosition(ccp(s.width / 2, s.height / 2));
    this->webSprite->setScaleX(xScale);
    this->webSprite->setScaleY(yScale);
    this->addChild(this->webSprite,2);
    this->isDown=true;
    this->removeChild(this->defaultSprite);
    this->defaultSprite=NULL;
}
