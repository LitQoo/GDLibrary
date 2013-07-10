//
//  SaveData.cpp
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#include "SaveData.h"
#include "JsonBox.h"
#include <sstream>
#include "StringCodec.h"

SaveData* saveData = SaveData::sharedObject();
SaveData* SaveData::sharedObject()
{
	static SaveData* _ins = 0;
	
	if(_ins == 0)
	{
		_ins = new SaveData();
		
	}
	return _ins;
}
void SaveData::createJSON()
{
	string rawData = readF();
	if(rawData == "")
		jsonData.loadFromString("{}");
	else
		jsonData.loadFromString(rawData);
	isInit = true;
}
void SaveData::setKeyValue(string _key, string _value)
{
	CCAssert(isInit == true, "call createJSON");
	string key = stringEnc(_key);
	string value = stringEnc(_value);
	jsonData[key] = value;
	ostringstream oss;
	jsonData.writeToStream(oss);
	testF(oss.str());
}
void SaveData::setKeyValue(string _key, int _value)
{
	CCAssert(isInit == true, "call createJSON");
	string key = stringEnc(_key);
	ostringstream valueoss;
	valueoss << _value;
	string value = stringEnc(valueoss.str());
	jsonData[key] = value;
	ostringstream oss;
	jsonData.writeToStream(oss);
	testF(oss.str());
}

string SaveData::getValue(string _key, string defaultValue)
{
	CCAssert(isInit == true, "call createJSON");
	string key = stringEnc(_key);
	string v = jsonData[key].getString();
	string v2 = stringDecode(v);
	if(v2 == "")
	{
		return defaultValue;
	}
	else
	{
		return v2;
	}
}

int SaveData::getValue(string _key, int defaultValue)
{
	CCAssert(isInit == true, "call createJSON");
	string key = stringEnc(_key);
	string v = jsonData[key].getString();
	string v2 = stringDecode(v);
	int _v2 = atoi(v2.c_str());
	if(v2 == "")
	{
		return defaultValue;
	}
	else
	{
		return _v2;
	}
}