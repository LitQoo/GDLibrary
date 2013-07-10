//
//  StringCodec.h
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#ifndef __BasketWorldCup2__StringCodec__
#define __BasketWorldCup2__StringCodec__
#include <iostream>
using namespace std;
std::basic_string<char> stringEnc(string plainText);
string stringDecode(std::basic_string<char> encodedText);
#endif /* defined(__BasketWorldCup2__StringCodec__) */
