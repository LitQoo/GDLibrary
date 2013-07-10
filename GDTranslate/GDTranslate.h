//
//  GDTranslate
//  GDTranslate
//
//  Created by LitQoo on 13. 6. 13..
//
//

#ifndef __GDUtility__GDTranslate__
#define __GDUtility__GDTranslate__

#include <iostream>
#include "cocos2d.h"
#include <string>
#include "JsonBox.h"
#include "GraphDog.h"
#include <stdio.h>

/*
 
사용법 
리소스폴더에 언어코드.txt파일을 만들고 안에 json dict 형태의 번역문을 놓는다.
 {"키명":"번역문","키명2":"번역문2",...}
              또는
 {"숫자":"번역문","1":"번역문",....}
 
번역텍스트가 필요한곳에
 using namespace GDTranslate; 해놓고
              
                gt("키명")   or   gt(숫자)

 을 호출하면 const char * 형의 번역문을 받을수있다.
 @tip : enum으로 번역키워드를 정해놓고 쓰면 편리하게 쓸수있을것이다.
 
언어는 기기설정에 의해 자동으로 선택되며 기본언어(대체언어)는 en으로 설정된다.
(고로 en.txt파일은 꼭 추가해놓는다.)
 
이설정을 바꾸고 싶으면 GDTranslateClass::get()->setLanguage(언어코드,기본언어코드);
를 가장 먼저 호출해주면된다.
*/
#include <string>

using namespace std;



namespace GDTranslate{
    class GDTranslateClass{
    private:
        string language;
        string defaultLanguage;
        JsonBox::Object languageData;
        JsonBox::Object defaultlanguageData;
        void getLanguageDataFromFile();
        bool isOpenFile;
    
    public:
        static GDTranslateClass* get()
        {
            static GDTranslateClass* _ins = 0;
            if(_ins == 0)
                _ins = new GDTranslateClass();
            return _ins;
        }
        
        GDTranslateClass();
        void setDefaultLanguage(string defaultLanguageCode);
        void setLanguage(string languageCode,string defaultLanguageCode);
        string translate(string key);
        string translate(int key);
    };

    string gt(string key);
    string gt(int key);
}

#endif /* defined(__GDUtility__GDTranslate__) */
