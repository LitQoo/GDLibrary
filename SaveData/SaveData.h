//
//  SaveData.h
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#ifndef __BasketWorldCup2__SaveData__
#define __BasketWorldCup2__SaveData__

#include <map>
#include <string>
#include "cocos2d.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	#include "SaveData_ios.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	#include "SaveData_android.h"
#endif


#include "JsonBox.h"

using namespace std;
class SaveData
{
private:
	SaveData(){
		isInit = false;
	}
public:
	static SaveData* sharedObject();
//	string loadDatas()
//	{
//		string t = readF();
//		return t;
//	}
	void setKeyValue(string key, string value);
	void setKeyValue(string key, int value);
	string getValue(string key, string defaultValue);
	int getValue(string key, int defaultValue);
	void createJSON();
private:
	bool isInit;
private:
	JsonBox::Value jsonData;
};
extern SaveData* saveData;
#endif /* defined(__BasketWorldCup2__SaveData__) */
