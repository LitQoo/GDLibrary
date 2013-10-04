//
//  kakaoConnectorDelegator.h
//  kakaoTest
//
//  Created by LitQoo on 13. 7. 2..
//
//

#ifndef __kakaoTest__kakaoConnectorDelegator__
#define __kakaoTest__kakaoConnectorDelegator__

#include "cocos2d.h"
#include "JsonBox.h"

using namespace cocos2d;
using namespace std;
typedef void (CCObject::*kakaoSelType)(JsonBox::Object);
#define kakao_selector(_SELECTOR) (kakaoSelType)(&_SELECTOR)
class KakaoDelegator
{
    
public:
	
	struct DeleSel
	{
		CCObject* target;
		kakaoSelType selector;
        JsonBox::Object param;
        JsonBox::Object callbackParam;
	};
    
    
public:
	void removeTarget(CCObject* _obj)
	{
        JsonBox::Value value;
		for(auto iter = DeleSelTable.begin(); iter != DeleSelTable.end(); iter++)
		{
			if(iter->second.target == _obj)
			{
				DeleSelTable.erase(iter);
			}
		}
	}
	int add(CCObject* target, kakaoSelType selector,JsonBox::Object param,JsonBox::Object callbackParam)
	{
		key++;
		DeleSel temp;
		temp.target = target;
		temp.selector = selector;
        temp.param = param;
        temp.callbackParam = callbackParam;
		DeleSelTable[key] = temp;
		return key;
	}
	void remove(int _key)
	{
		if(DeleSelTable.find(_key) != DeleSelTable.end())
			DeleSelTable.erase(_key);
	}
	DeleSel load(int _key)
	{
		if(DeleSelTable.find(_key) != DeleSelTable.end())
		{
			return DeleSelTable[_key];
		}
		else
		{
			DeleSel _t;
			_t.target = 0;
			_t.selector = 0;
			return _t;
		}
	}
    
public:
	static KakaoDelegator* get()
	{
		static KakaoDelegator* _ins = 0;
		if(_ins == 0)
			_ins = new KakaoDelegator();
		return _ins;
	}
    
private:
	KakaoDelegator()
	{
		key = 0;
        buff="";
	}
	~KakaoDelegator(){}
private:
	map<int, DeleSel> DeleSelTable;
	int key;
    
public:
    string buff;
};

#endif /* defined(__kakaoTest__kakaoConnectorDelegator__) */

