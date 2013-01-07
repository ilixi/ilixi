/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lib/InputHelperJP.h>
#include <lib/utf8.h>
#include <directfb.h>

namespace ilixi
{

InputHelperJP::InputHelperJP()
        : InputHelper()
{
    initHiraganaMap();
}

InputHelperJP::~InputHelperJP()
{
}

bool
InputHelperJP::convert(uint32_t symbol)
{
    if (symbol == DIKS_PERIOD)
    {
//        std::vector<uint32_t> ucs32;
//        decode((uint8_t*) "。", ucs32);
//        forwardKeyData(ucs32);

        wchar_t* out = (wchar_t*) calloc(6, sizeof(wchar_t));
        size_t bytes = utf8_to_wchar("。", sizeof("。"), out, 6, UTF8_IGNORE_ERROR);
        _conversion += out;
        free(out);

        return true;
    } else if (symbol == DIKS_COMMA)
    {
        wchar_t* out = (wchar_t*) calloc(6, sizeof(wchar_t));
        size_t bytes = utf8_to_wchar("、", sizeof("、"), out, 6, UTF8_IGNORE_ERROR);
        _conversion += out;
        free(out);

        return true;
    } else if (symbol == DIKS_MINUS_SIGN)
    {
        wchar_t* out = (wchar_t*) calloc(6, sizeof(wchar_t));
        size_t bytes = utf8_to_wchar("ー", sizeof("ー"), out, 6, UTF8_IGNORE_ERROR);
        _conversion.append(out);
        free(out);

        return true;
    }

    if (_buffer.length() > 3) // 4-letter
    {
        std::string prefix = _buffer.substr(_buffer.length() - 4, 4);
        if (_hiraganaMap.find(prefix) != _hiraganaMap.end())
        {
//            std::vector<uint32_t> ucs32;
//            decode((uint8_t*) _hiraganaMap.find(prefix)->second.c_str(), ucs32);
//            forwardKeyData(ucs32);
            wchar_t* out = (wchar_t*) calloc(_hiraganaMap.find(prefix)->second.size() + 1, sizeof(wchar_t));
            size_t bytes = utf8_to_wchar(_hiraganaMap.find(prefix)->second.c_str(), _hiraganaMap.find(prefix)->second.size(), out, _hiraganaMap.find(prefix)->second.size() + 1, UTF8_SKIP_BOM);
            _processed.push_back(4);
            _steps.push_back(bytes);
            _conversion.append(out);
            free(out);

            printf(" 4 - %s = %s\n", prefix.c_str(), _hiraganaMap.find(prefix)->second.c_str());
            return true;
        }
    }

    if (_buffer.length() > 2) // 3-letter
    {
        std::string prefix = _buffer.substr(_buffer.length() - 3, 3);
        if (_hiraganaMap.find(prefix) != _hiraganaMap.end())
        {
            wchar_t* out = (wchar_t*) calloc(_hiraganaMap.find(prefix)->second.size() + 1, sizeof(wchar_t));
            size_t bytes = utf8_to_wchar(_hiraganaMap.find(prefix)->second.c_str(), _hiraganaMap.find(prefix)->second.size(), out, _hiraganaMap.find(prefix)->second.size() + 1, UTF8_SKIP_BOM);
            _processed.push_back(3);
            _steps.push_back(bytes);
            _conversion.append(out);
            free(out);

            printf(" 3 - %s = %s\n", prefix.c_str(), _hiraganaMap.find(prefix)->second.c_str());
            return true;
        }
    }

    if (_buffer.length() > 1) // 2-letter
    {
        std::string prefix = _buffer.substr(_buffer.length() - 2, 2);
        if (_hiraganaMap.find(prefix) != _hiraganaMap.end())
        {
            wchar_t* out = (wchar_t*) calloc(_hiraganaMap.find(prefix)->second.size() + 1, sizeof(wchar_t));
            size_t bytes = utf8_to_wchar(_hiraganaMap.find(prefix)->second.c_str(), _hiraganaMap.find(prefix)->second.size(), out, _hiraganaMap.find(prefix)->second.size() + 1, UTF8_SKIP_BOM);
            _processed.push_back(2);
            _steps.push_back(bytes);
            _conversion.append(out);
            free(out);

            printf(" 2 - %s = %s\n", prefix.c_str(), _hiraganaMap.find(prefix)->second.c_str());
            return true;
        }

    }

    if (_buffer.length() > 0)
    {
        // 1-letter
        std::string prefix = _buffer.substr(_buffer.length() - 1, 1);
        if (_hiraganaMap.find(prefix) != _hiraganaMap.end())
        {
            wchar_t* out = (wchar_t*) calloc(_hiraganaMap.find(prefix)->second.size() + 1, sizeof(wchar_t));
            size_t bytes = utf8_to_wchar(_hiraganaMap.find(prefix)->second.c_str(), _hiraganaMap.find(prefix)->second.size(), out, _hiraganaMap.find(prefix)->second.size() + 1, UTF8_SKIP_BOM);
            _processed.push_back(1);
            _steps.push_back(bytes);
            _conversion.append(out);
            free(out);

            printf(" 1 - %s = %s\n", prefix.c_str(), _hiraganaMap.find(prefix)->second.c_str());
            return true;
        }
    }
    return false;
}

void
InputHelperJP::initHiraganaMap()
{
    //// 4-letter romaji
    _hiraganaMap["xtsu"] = "っ";

    //// 3-letter romaji
    _hiraganaMap["kya"] = "きゃ";
    _hiraganaMap["kyi"] = "きぃ";
    _hiraganaMap["kyu"] = "きゅ";
    _hiraganaMap["kye"] = "きぇ";
    _hiraganaMap["kyo"] = "きょ";
    _hiraganaMap["sya"] = "しゃ";
    _hiraganaMap["syi"] = "し";
    _hiraganaMap["syu"] = "しゅ";
    _hiraganaMap["sye"] = "しぇ";
    _hiraganaMap["syo"] = "しょ";
    _hiraganaMap["sha"] = "しゃ";
    _hiraganaMap["shi"] = "し";
    _hiraganaMap["shu"] = "しゅ";
    _hiraganaMap["she"] = "しぇ";
    _hiraganaMap["sho"] = "しょ";
    _hiraganaMap["tya"] = "ちゃ";
    _hiraganaMap["tyi"] = "ちぃ";
    _hiraganaMap["tyu"] = "ちゅ";
    _hiraganaMap["tye"] = "ちぇ";
    _hiraganaMap["tyo"] = "ちょ";
    _hiraganaMap["cya"] = "ちゃ";
    _hiraganaMap["cyi"] = "ちぃ";
    _hiraganaMap["cyu"] = "ちゅ";
    _hiraganaMap["cye"] = "ちぇ";
    _hiraganaMap["cyo"] = "ちょ";
    _hiraganaMap["cha"] = "ちゃ";
    _hiraganaMap["chi"] = "ち";
    _hiraganaMap["chu"] = "ちゅ";
    _hiraganaMap["che"] = "ちぇ";
    _hiraganaMap["cho"] = "ちょ";
    _hiraganaMap["nya"] = "にゃ";
    _hiraganaMap["nyi"] = "にぃ";
    _hiraganaMap["nyu"] = "にゅ";
    _hiraganaMap["nye"] = "にぇ";
    _hiraganaMap["nyo"] = "にょ";
    _hiraganaMap["hya"] = "ひゃ";
    _hiraganaMap["hyi"] = "ひぃ";
    _hiraganaMap["hyu"] = "ひゅ";
    _hiraganaMap["hye"] = "ひぇ";
    _hiraganaMap["hyo"] = "ひょ";
    _hiraganaMap["mya"] = "みゃ";
    _hiraganaMap["myi"] = "みぃ";
    _hiraganaMap["myu"] = "みゅ";
    _hiraganaMap["mye"] = "みぇ";
    _hiraganaMap["myo"] = "みょ";
    _hiraganaMap["rya"] = "りゃ";
    _hiraganaMap["ryi"] = "りぃ";
    _hiraganaMap["ryu"] = "りゅ";
    _hiraganaMap["rye"] = "りぇ";
    _hiraganaMap["ryo"] = "りょ";

    _hiraganaMap["gya"] = "ぎゃ";
    _hiraganaMap["gyi"] = "ぎぃ";
    _hiraganaMap["gyu"] = "ぎゅ";
    _hiraganaMap["gye"] = "ぎぇ";
    _hiraganaMap["gyo"] = "ぎょ";
    _hiraganaMap["zya"] = "じゃ";
    _hiraganaMap["zyi"] = "じぃ";
    _hiraganaMap["zyu"] = "じゅ";
    _hiraganaMap["zye"] = "じぇ";
    _hiraganaMap["zyo"] = "じょ";
    _hiraganaMap["jya"] = "じゃ";
    _hiraganaMap["jyi"] = "じぃ";
    _hiraganaMap["jyu"] = "じゅ";
    _hiraganaMap["jye"] = "じぇ";
    _hiraganaMap["jyo"] = "じょ";
    _hiraganaMap["dya"] = "ぢゃ";
    _hiraganaMap["dyi"] = "ぢぃ";
    _hiraganaMap["dyu"] = "ぢゅ";
    _hiraganaMap["dye"] = "ぢぇ";
    _hiraganaMap["dyo"] = "ぢょ";
    _hiraganaMap["bya"] = "びゃ";
    _hiraganaMap["byi"] = "びぃ";
    _hiraganaMap["byu"] = "びゅ";
    _hiraganaMap["bye"] = "びぇ";
    _hiraganaMap["byo"] = "びょ";
    _hiraganaMap["pya"] = "ぴゃ";
    _hiraganaMap["pyi"] = "ぴぃ";
    _hiraganaMap["pyu"] = "ぴゅ";
    _hiraganaMap["pye"] = "ぴぇ";
    _hiraganaMap["pyo"] = "ぴょ";

    _hiraganaMap["xya"] = "ゃ";
    _hiraganaMap["xyi"] = "ぃ";
    _hiraganaMap["xyu"] = "ゅ";
    _hiraganaMap["xye"] = "ぇ";
    _hiraganaMap["xyo"] = "ょ";
    _hiraganaMap["lya"] = "ゃ";
    _hiraganaMap["lyi"] = "ぃ";
    _hiraganaMap["lyu"] = "ゅ";
    _hiraganaMap["lye"] = "ぇ";
    _hiraganaMap["lyo"] = "ょ";

    _hiraganaMap["qya"] = "くゃ";
    _hiraganaMap["qyi"] = "くぃ";
    _hiraganaMap["qyu"] = "くゅ";
    _hiraganaMap["qye"] = "くぇ";
    _hiraganaMap["qyo"] = "くょ";
    _hiraganaMap["fya"] = "ふゃ";
    _hiraganaMap["fyi"] = "ふぃ";
    _hiraganaMap["fyu"] = "ふゅ";
    _hiraganaMap["fye"] = "ふぇ";
    _hiraganaMap["fyo"] = "ふょ";
    _hiraganaMap["tsa"] = "つぁ";
    _hiraganaMap["tsi"] = "つぃ";
    _hiraganaMap["tsu"] = "つ";
    _hiraganaMap["tse"] = "つぇ";
    _hiraganaMap["tso"] = "つぉ";

    ////2-letter romaji
    _hiraganaMap["nn"] = "ん";

    _hiraganaMap["ka"] = "か";
    _hiraganaMap["ki"] = "き";
    _hiraganaMap["ku"] = "く";
    _hiraganaMap["ke"] = "け";
    _hiraganaMap["ko"] = "こ";
    _hiraganaMap["sa"] = "さ";
    _hiraganaMap["si"] = "し";
    _hiraganaMap["su"] = "す";
    _hiraganaMap["se"] = "せ";
    _hiraganaMap["so"] = "そ";
    _hiraganaMap["ta"] = "た";
    _hiraganaMap["ti"] = "ち";
    _hiraganaMap["tu"] = "つ";
    _hiraganaMap["te"] = "て";
    _hiraganaMap["to"] = "と";
    _hiraganaMap["na"] = "な";
    _hiraganaMap["ni"] = "に";
    _hiraganaMap["nu"] = "ぬ";
    _hiraganaMap["ne"] = "ね";
    _hiraganaMap["no"] = "の";
    _hiraganaMap["ha"] = "は";
    _hiraganaMap["hi"] = "ひ";
    _hiraganaMap["hu"] = "ふ";
    _hiraganaMap["he"] = "へ";
    _hiraganaMap["ho"] = "ほ";
    _hiraganaMap["ma"] = "ま";
    _hiraganaMap["mi"] = "み";
    _hiraganaMap["mu"] = "む";
    _hiraganaMap["me"] = "め";
    _hiraganaMap["mo"] = "も";
    _hiraganaMap["ya"] = "や";
    _hiraganaMap["yi"] = "い";
    _hiraganaMap["yu"] = "ゆ";
    _hiraganaMap["ye"] = "いぇ";
    _hiraganaMap["yo"] = "よ";
    _hiraganaMap["ra"] = "ら";
    _hiraganaMap["ri"] = "り";
    _hiraganaMap["ru"] = "る";
    _hiraganaMap["re"] = "れ";
    _hiraganaMap["ro"] = "ろ";
    _hiraganaMap["wa"] = "わ";
    _hiraganaMap["wi"] = "うぃ";
    _hiraganaMap["wu"] = "う";
    _hiraganaMap["we"] = "うぇ";
    _hiraganaMap["wo"] = "を";

    _hiraganaMap["ga"] = "が";
    _hiraganaMap["gi"] = "ぎ";
    _hiraganaMap["gu"] = "ぐ";
    _hiraganaMap["ge"] = "げ";
    _hiraganaMap["go"] = "ご";
    _hiraganaMap["za"] = "ざ";
    _hiraganaMap["zi"] = "じ";
    _hiraganaMap["zu"] = "ず";
    _hiraganaMap["ze"] = "ぜ";
    _hiraganaMap["zo"] = "ぞ";
    _hiraganaMap["da"] = "だ";
    _hiraganaMap["di"] = "ぢ";
    _hiraganaMap["du"] = "づ";
    _hiraganaMap["de"] = "で";
    _hiraganaMap["do"] = "ど";
    _hiraganaMap["ba"] = "ば";
    _hiraganaMap["bi"] = "び";
    _hiraganaMap["bu"] = "ぶ";
    _hiraganaMap["be"] = "べ";
    _hiraganaMap["bo"] = "ぼ";
    _hiraganaMap["pa"] = "ぱ";
    _hiraganaMap["pi"] = "ぴ";
    _hiraganaMap["pu"] = "ぷ";
    _hiraganaMap["pe"] = "ぺ";
    _hiraganaMap["po"] = "ぽ";

    _hiraganaMap["ja"] = "じゃ";
    _hiraganaMap["ji"] = "じ";
    _hiraganaMap["ju"] = "じゅ";
    _hiraganaMap["je"] = "じぇ";
    _hiraganaMap["jo"] = "じょ";
    _hiraganaMap["xa"] = "ぁ";
    _hiraganaMap["xi"] = "ぃ";
    _hiraganaMap["xu"] = "ぅ";
    _hiraganaMap["xe"] = "ぇ";
    _hiraganaMap["xo"] = "ぉ";
    _hiraganaMap["la"] = "ぁ";
    _hiraganaMap["li"] = "ぃ";
    _hiraganaMap["lu"] = "ぅ";
    _hiraganaMap["le"] = "ぇ";
    _hiraganaMap["lo"] = "ぉ";
    _hiraganaMap["qa"] = "くぁ";
    _hiraganaMap["qi"] = "くぃ";
    _hiraganaMap["qu"] = "く";
    _hiraganaMap["qe"] = "くぇ";
    _hiraganaMap["qo"] = "くぉ";
    _hiraganaMap["fa"] = "ふぁ";
    _hiraganaMap["fi"] = "ふぃ";
    _hiraganaMap["fu"] = "ふ";
    _hiraganaMap["fe"] = "ふぇ";
    _hiraganaMap["fo"] = "ふぉ";
    _hiraganaMap["ca"] = "か";
    _hiraganaMap["ci"] = "し";
    _hiraganaMap["cu"] = "く";
    _hiraganaMap["ce"] = "せ";
    _hiraganaMap["co"] = "こ";

    ////1-letter romaji
    _hiraganaMap["a"] = "あ";
    _hiraganaMap["i"] = "い";
    _hiraganaMap["u"] = "う";
    _hiraganaMap["e"] = "え";
    _hiraganaMap["o"] = "お";
    _hiraganaMap["n"] = "ん";
}

} /* namespace ilixi */
