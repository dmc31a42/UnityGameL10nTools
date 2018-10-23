// RegExpULTTextPlugin.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
using namespace std;
#include <string>
#include <algorithm>
#include <regex>

#include "IULTPluginCommonInterface.h"
#include "IULTTextPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"

TextPluginInfo* TextPluginInfoGlobal;

AssetMapOption ModifyKeyValueRegExpRecursive(AssetMapOption assetMapOptionFrom, int i, wstring keyValue);
wstring RegExpReplaceRecursive(wstring str, AssetMapOption assetMapOptionFrom);
LanguagePairDics _cdecl GetOriginalMapFromText(wstring OriginalText, LanguagePairDics languagePairDics) {
	if (languagePairDics.size() == 0) {
		languagePairDics.insert(pair<wstring,LanguagePairDic>(L"0", LanguagePairDic()));
	}
	for (map<wstring, LanguagePairDic>::iterator lpdItr = languagePairDics.begin();
		lpdItr != languagePairDics.end(); lpdItr++) {
		if (lpdItr->second.InteractWithAssetOptions.size() != 5) {
			lpdItr->second.InteractWithAssetOptions.clear();
			AssetMapOption firstRegExp = AssetMapOption(
				L"RegExp Match expression",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption keySelectOption = AssetMapOption(
				L"Select Key Group",
				L"Select which group is key.",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_INT,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption valueSelectOption = AssetMapOption(
				L"Select Value Group",
				L"Select which group is value.",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_INT,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption keyRexExpReplaceFrom = AssetMapOption(
				L"Key Replace [0] From",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption keyRexExpReplaceTo = AssetMapOption(
				L"Key Replace [0] To",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			keyRexExpReplaceFrom.nestedOptions.push_back(keyRexExpReplaceTo);

			AssetMapOption valueRexExpReplaceFrom = AssetMapOption(
				L"Value Replace [0] From",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption valueRexExpReplaceTo = AssetMapOption(
				L"Value Replace [0] To",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			valueRexExpReplaceFrom.nestedOptions.push_back(valueRexExpReplaceTo);

			lpdItr->second.InteractWithAssetOptions.push_back(firstRegExp);
			lpdItr->second.InteractWithAssetOptions.push_back(keySelectOption);
			lpdItr->second.InteractWithAssetOptions.push_back(valueSelectOption);
			lpdItr->second.InteractWithAssetOptions.push_back(keyRexExpReplaceFrom);
			lpdItr->second.InteractWithAssetOptions.push_back(valueRexExpReplaceFrom);
		} else {
			AssetMapOption firstRegExp = lpdItr->second.InteractWithAssetOptions[0];
			
			if (firstRegExp.Value && *(wstring*)firstRegExp.Value != L"") {
				wregex firstRegex(*(wstring*)firstRegExp.Value);
				wsmatch firstTest;
				regex_match(OriginalText, firstTest, firstRegex);
				if (firstTest.size() >= 3) {
					if (firstTest.size() - 1 != lpdItr->second.InteractWithAssetOptions[1].nestedOptions.size()) {
						lpdItr->second.InteractWithAssetOptions[1].nestedOptions.clear();
						for (int i = 1; i < firstTest.size(); i++) {
							AssetMapOption keySelectOptionChild = AssetMapOption(
								L"",
								L"",
								new int(i),
								NULL,
								AssetMapOption::OPTION_TYPE_NONE,
								AssetMapOption::OPTION_TYPE_INT,
								vector<AssetMapOption>()
							);
							lpdItr->second.InteractWithAssetOptions[1].nestedOptions.push_back(keySelectOptionChild);
						}
						lpdItr->second.InteractWithAssetOptions[2].nestedOptions.clear();
						for (int i = 1; i < firstTest.size(); i++) {
							AssetMapOption valueSelectOptionChild = AssetMapOption(
								L"",
								L"",
								new int(i),
								NULL,
								AssetMapOption::OPTION_TYPE_NONE,
								AssetMapOption::OPTION_TYPE_INT,
								vector<AssetMapOption>()
							);
							lpdItr->second.InteractWithAssetOptions[2].nestedOptions.push_back(valueSelectOptionChild);
						}
					}
					lpdItr->second.InteractWithAssetOptions[3] = ModifyKeyValueRegExpRecursive(lpdItr->second.InteractWithAssetOptions[3], 0, L"Key");
					lpdItr->second.InteractWithAssetOptions[4] = ModifyKeyValueRegExpRecursive(lpdItr->second.InteractWithAssetOptions[4], 0, L"Value");
					AssetMapOption keyRexExpReplaceFrom = lpdItr->second.InteractWithAssetOptions[3];
					AssetMapOption keyRexExpReplaceTo = keyRexExpReplaceFrom.nestedOptions[0];
					AssetMapOption valueRexExpReplaceFrom = lpdItr->second.InteractWithAssetOptions[4];
					AssetMapOption valueRexExpReplaceTo = keyRexExpReplaceFrom.nestedOptions[0];
					// 
					AssetMapOption keySelectOption = lpdItr->second.InteractWithAssetOptions[1];
					AssetMapOption valueSelectOption = lpdItr->second.InteractWithAssetOptions[2];
					if (keySelectOption.Value && valueSelectOption.Value &&
						*(int*)keySelectOption.Value != *(int*)valueSelectOption.Value) {
						for (wsregex_iterator it(OriginalText.begin(), OriginalText.end(), firstRegex);
							it != wsregex_iterator(); ++it) {
							wstring key = (*it)[*(int*)keySelectOption.Value].str();
							wstring value = (*it)[*(int*)valueSelectOption.Value].str();
							key = RegExpReplaceRecursive(key, keyRexExpReplaceFrom);
							value = RegExpReplaceRecursive(value, valueRexExpReplaceFrom);
							LanguagePair lp;
							lp.Original = value;
							lpdItr->second.Dic.insert(pair<wstring, LanguagePair>(key, lp));
						}
					}
				}
			}
		}
	}
	return languagePairDics;
}

wstring RegExpReplaceRecursive(wstring str, AssetMapOption assetMapOptionFrom) {
	AssetMapOption assetMapOptionTo = assetMapOptionFrom.nestedOptions[0];
	if (assetMapOptionFrom.Value && assetMapOptionTo.Value && *(wstring*)assetMapOptionFrom.Value != L"") {
		wregex wrgx(*(wstring*)assetMapOptionFrom.Value);
		str = regex_replace(str, wrgx, *(wstring*)assetMapOptionTo.Value);
		if (assetMapOptionTo.nestedOptions.size() == 1) {
			str = RegExpReplaceRecursive(str, assetMapOptionTo.nestedOptions[0]);
		}
	}
	return str;
}

AssetMapOption ModifyKeyValueRegExpRecursive(AssetMapOption assetMapOptionFrom, int i, wstring keyValue) {
	AssetMapOption assetMapOptionTo = assetMapOptionFrom.nestedOptions[0];
	if (assetMapOptionFrom.Value && *(wstring*)assetMapOptionFrom.Value != L"") {
		if (assetMapOptionTo.nestedOptions.size() == 0) {
			AssetMapOption childFrom = AssetMapOption(
				keyValue + L" Replace [" + to_wstring((long long)++i) + L"] From",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption childTo = AssetMapOption(
				keyValue + L" Replace [" + to_wstring((long long)++i) + L"] To",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			childFrom.nestedOptions.push_back(childTo);
			assetMapOptionTo.nestedOptions.push_back(childFrom);
		}
		else {
			assetMapOptionTo.nestedOptions[0] = ModifyKeyValueRegExpRecursive(assetMapOptionTo.nestedOptions[0], ++i, keyValue);
		}

	}
	assetMapOptionFrom.nestedOptions[0] = assetMapOptionTo;
	return assetMapOptionFrom;
}

wstring RegExpReplaceReverseRecursive(wstring str, AssetMapOption assetMapOptionFrom);
wstring _cdecl GetTranslatedTextFromMap(wstring OriginalText, LanguagePairDics TranslatedMap) {
	for (LanguagePairDics::iterator lpdItr = TranslatedMap.begin();
		lpdItr != TranslatedMap.end(); lpdItr++) {
		AssetMapOption firstRegExp = lpdItr->second.InteractWithAssetOptions[0];
		AssetMapOption keySelectOption = lpdItr->second.InteractWithAssetOptions[1];
		AssetMapOption valueSelectOption = lpdItr->second.InteractWithAssetOptions[2];
		AssetMapOption keyRegExpReplaceFrom = lpdItr->second.InteractWithAssetOptions[3];
		AssetMapOption valueRegExpReplaceFrom = lpdItr->second.InteractWithAssetOptions[4];
		if (firstRegExp.Value && keySelectOption.Value && keyRegExpReplaceFrom.Value && valueRegExpReplaceFrom.Value) {
			wstring regStr = *(wstring*)firstRegExp.Value;
			int keySelect = *(int*)keySelectOption.Value;
			int valueSelect = *(int*)valueSelectOption.Value;
			if (regStr != L"" && keySelect != valueSelect) {
				wregex wrgx(regStr);
				for (wsregex_iterator it(OriginalText.begin(), OriginalText.end(), wrgx);
					it != wsregex_iterator(); it++) {
					wstring originalBlock = (*it)[0];
					wstring key = (*it)[keySelect];
					wstring value = (*it)[valueSelect];
					wstring keyParsered = RegExpReplaceRecursive(key, keyRegExpReplaceFrom);
					map<wstring, LanguagePair>::iterator lpItr = lpdItr->second.Dic.find(keyParsered);
					if (lpItr != lpdItr->second.Dic.end()) {
						if (lpItr->second.Translated != L"") {
							//여기서 OriginalText를 변경
							wstring TranslatedReversedParsed = RegExpReplaceReverseRecursive(lpItr->second.Translated, valueRegExpReplaceFrom);
							wstring ModifiedBlock = ReplaceAll(originalBlock, value, TranslatedReversedParsed);
							OriginalText = ReplaceAll(OriginalText, wstring((*it)[0]), ModifiedBlock);
						}
					}
				}
			}
		}
	}
	return OriginalText;
}

wstring RegExpReplaceReverseRecursive(wstring str, AssetMapOption assetMapOptionFrom) {
	AssetMapOption assetMapOptionTo = assetMapOptionFrom.nestedOptions[0];
	if (assetMapOptionFrom.Value && assetMapOptionTo.Value && *(wstring*)assetMapOptionFrom.Value != L"" && *(wstring*)assetMapOptionTo.Value != L"") {
		str = RegExpReplaceReverseRecursive(str, assetMapOptionTo.nestedOptions[0]);
		wregex wrgx(*(wstring*)assetMapOptionTo.Value);
		return str = regex_replace(str, wrgx, *(wstring*)assetMapOptionFrom.Value);
	}
	else {
		return str;
	}
}

//
//LanguagePairDics _cdecl GetUpdateFileTextFromMap(LanguagePairDics UpdateMap) {
//
//}
//
//LanguagePairDics _cdecl GetTranslatedMapFromFileText(LanguagePairDics translatedMap) {
//
//}

bool _cdecl GetTextPluginInfo(TextPluginInfo* textPluginInfo) {
	TextPluginInfoGlobal = textPluginInfo;

	wcsncpy_s(textPluginInfo->TextPluginName, L"RegExp", 6);
	textPluginInfo->GetOriginalMapFromText = GetOriginalMapFromText;
	textPluginInfo->GetTranslatedTextFromMap = GetTranslatedTextFromMap;

	//textPluginInfo->GetUpdateFileTextFromMap = GetUpdateFileTextFromMap;
	//textPluginInfo->GetTranslatedMapFromFileText = GetTranslatedMapFromFileText;
	return true;
}