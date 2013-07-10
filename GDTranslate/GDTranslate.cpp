//
//  GDTranslate
//  GDTranslate
//
//  Created by LitQoo on 13. 6. 13..
//
//

#include "GDTranslate.h"

namespace GDTranslate{
    string gt(string key){
        return GDTranslateClass::get()->translate(key);
    }
    string gt(int key){
        return GDTranslateClass::get()->translate(key);
    }
    
    GDTranslateClass::GDTranslateClass(){
        //초기화
        isOpenFile = false;
        language = GraphDogLib::getLocalCode();
        defaultLanguage = "en";
    }

    void GDTranslateClass::getLanguageDataFromFile(){
        
        CCFileUtils::sharedFileUtils()->setPopupNotify(false);
        
        
        //설정언어 데이터 읽어오기
        unsigned char* mFileData = NULL;
        {
            string filename = language+".txt";
            
			
			
            std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(filename.c_str()) ;
            unsigned long bufferSize = 0;
            mFileData = CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str(), "r", &bufferSize);
            
            if(mFileData!=NULL){
                string abc = (char *)mFileData;
				abc = "{" + abc + "}";
                languageData = GraphDogLib::StringToJsonObject(abc.c_str());
				delete [] mFileData;
            }
        }
        
        
        //기본언어 데이터 읽어오기
        {
            string filename = defaultLanguage+".txt";
            
            std::string fullPath =CCFileUtils::sharedFileUtils()->fullPathForFilename(filename.c_str());

            unsigned char* mFileData2 = NULL;
            unsigned long bufferSize = 0;
            mFileData2 = CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str(), "r", &bufferSize);
            if(mFileData2!=NULL){
                
                string abc = (char *)mFileData2;
				delete [] mFileData2;
                abc = "{" + abc + "}";
                //설정된 언어가 없으면 기본언어로 대체
                if(mFileData==NULL){
                    languageData = GraphDogLib::StringToJsonObject(abc.c_str());
                }else{
                    defaultlanguageData = GraphDogLib::StringToJsonObject(abc.c_str());
                }
            }
        }
        isOpenFile = true;
        
    }

    void GDTranslateClass::setDefaultLanguage(string defaultLanguageCode){
        //대체언어설정
        defaultLanguage = defaultLanguageCode;
    }

    void GDTranslateClass::setLanguage(string languageCode,string defaultLanguageCode){
        //언어및 대체언어설정
        if(languageCode!="")language = languageCode;
        if(defaultLanguageCode!="")defaultLanguage = defaultLanguageCode;
    }



    string GDTranslateClass::translate(string key){
        //데이터가져오기
        if(isOpenFile==false) getLanguageDataFromFile();

        //설정언어에서 값찾기
        {
            string result = languageData[key.c_str()].getString();
            if(result!="")return result.c_str();
            
        }
        
        //없으면 대체언어에서 값찾기
        {
            string result = defaultlanguageData[key.c_str()].getString();
            if(result!="")return result.c_str();
        }
        
        //그것도 없으면 그냥 key돌려주기
        return key.c_str();
        
    }

    string GDTranslateClass::translate(int key){
        char buffer[100];
        sprintf(buffer, "%d",key);
        return translate(buffer);
    }
    
}