/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Andreas Shimokawa <andi@directfb.org>.

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
#include <core/Logger.h>
#include <directfb.h>
#include <iconv.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_INPUTHELPERJP, "ilixi/lib/InputHelperJP", "InputHelperJP");

#if ILIXI_HAVE_LIBWNN

extern "C"
{

struct wnn_buf *
jl_open_lang(char *, char *, char *, char *, int
(*)(char *), int
(*)(char *), int);

void
jl_close(struct wnn_buf*);

int
jl_ren_conv(struct wnn_buf*, w_char*, int, int, int);

int
jl_zenkouho_dai(struct wnn_buf*, int, int, int, int);

int
wnn_get_area(struct wnn_buf*, int, int, w_char*, int);

void
jl_get_zenkouho_kanji(struct wnn_buf*, int, w_char *);

int
jl_nobi_conv(struct wnn_buf*, int, int, int, int, int);

int
jl_yomi_len(struct wnn_buf*, int, int);

//int
//jl_zenkouho_suu(struct wnn_buf*);
} // extern C

int
wnnMessage(char* str)
{
    ILOG_INFO(ILX_INPUTHELPERJP, "WNN: %s\n", str);
    return 1;
}

int
wnnError(char* str)
{
    ILOG_ERROR(ILX_INPUTHELPERJP, "WNN: %s\n", str);
    return 1;
}

void
wstrtoeucstr(w_char* wstr, unsigned char* eucstr)
{
    w_char tmp;
    while (*wstr)
    {
        tmp = *wstr++;
        if (tmp & 0x8000)
        {
            *eucstr++ = tmp >> 8;
            *eucstr++ = tmp;
        } else if (tmp & 0x0080)
        {
            *eucstr++ = 0x8e;
            *eucstr++ = tmp & 0xff;
        } else
            *eucstr++ = tmp;
    }
    *eucstr++ = 0;
}

void
eucstrtowstr(unsigned char* eucstr, w_char* wstr)
{
    w_char tmp;
    while (*eucstr)
    {
        tmp = *eucstr++;
        if (tmp & 0x0080)
            tmp = ((tmp << 8) & 0xff00) | *eucstr++;
        *wstr++ = tmp;
    }
    *wstr = 0;
}

int
eucwstoutf8(w_char* eucwstr, char* utfstr)
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    size_t inbytesleft;
    size_t outbytesleft = 1023;
    char *tmp = (char*) calloc(1024, 1); // must be zeroed for iconv
    char *inptr = tmp;
    char *outptr = utfstr;

    wstrtoeucstr(eucwstr, (unsigned char*) tmp);

    inbytesleft = strlen(tmp) + 1;

    iconv_t cd = iconv_open("UTF-8", "EUC-JP");
    if (cd == (void*) -1)
    {
        ILOG_ERROR(ILX_INPUTHELPERJP, "Error: iconv_open EUC-JP to UTF8\n");
        return -1;
    }

    int res = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
    if (res)
    {
        ILOG_ERROR(ILX_INPUTHELPERJP, "iconv could not convert EUC-JP to UTF8!\n");
        if (errno == E2BIG)
            ILOG_ERROR(ILX_INPUTHELPERJP, " -> Output buffer not large enough.\n");
        if (errno == EILSEQ)
            ILOG_ERROR(ILX_INPUTHELPERJP, " -> Invalid byte sequence in input.\n");
        if (errno == EINVAL)
            ILOG_ERROR(ILX_INPUTHELPERJP, " -> Incomplete byte sequence at the end of input.\n");

        free(tmp);
        iconv_close(cd);
        return res;
    }
    iconv_close(cd);

    free(tmp);
    return res;
}

int
utf8toeucws(char* utfstr, w_char* eucwstr)
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    size_t inbytesleft = strlen(utfstr);
    size_t outbytesleft = 1023;
    char *tmp = (char*) calloc(1024, 1); // must be zeroed for iconv
    char *inptr = utfstr;
    char *outptr = tmp;

    iconv_t cd = iconv_open("EUC-JP", "UTF-8");
    if (cd == (void*) -1)
    {
        ILOG_ERROR(ILX_INPUTHELPERJP, "Error: iconv_open UTF8 to EUC-JP \n");
        return -1;
    }

    int res = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
    if (res)
    {
        ILOG_ERROR(ILX_INPUTHELPERJP, "iconv could not convert UTF8 to EUC-JP!\n");
        if (errno == E2BIG)
            ILOG_ERROR(ILX_INPUTHELPERJP, " -> Output buffer not large enough.\n");
        if (errno == EILSEQ)
            ILOG_ERROR(ILX_INPUTHELPERJP, " -> Invalid byte sequence in input.\n");
        if (errno == EINVAL)
            ILOG_ERROR(ILX_INPUTHELPERJP, " -> Incomplete byte sequence at the end of input.\n");

        free(tmp);
        iconv_close(cd);
        return res;
    }
    iconv_close(cd);

    eucstrtowstr((unsigned char*) tmp, eucwstr);

    free(tmp);
    return res;
}

#endif

//*************************************************************************************************************

InputHelperJP::InputHelperJP()
        : InputHelper()
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    initHiraganaMap();

#if ILIXI_HAVE_LIBWNN
    ILOG_DEBUG(ILX_INPUTHELPERJP, " -> Connecting to wnn for user: %s\n", getlogin());
    _wnn = NULL;
    _wnn = jl_open(getlogin(), "localhost", "/usr/share/wnn/ja_JP/wnnenvrc", wnnMessage, wnnError, 10);
    if (_wnn == NULL)
        ILOG_THROW(ILX_INPUTHELPERJP, "Could not connect to wnn server!\n");
#endif
}

InputHelperJP::~InputHelperJP()
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);

#if ILIXI_HAVE_LIBWNN
    jl_close(_wnn);
    _wnn = NULL;
#endif
}

void
InputHelperJP::process()
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
#if ILIXI_HAVE_LIBWNN
    // Convert hiragana (UTF-8) to _ws (EUC-JP).
    int res = utf8toeucws(const_cast<char*>(_pdata.c_str()), _ws);
    ILOG_DEBUG(ILX_INPUTHELPERJP, " -> utf8toeucws: %d\n", res);

    // Get number of segments.
    int segmentCount = jl_ren_conv(_wnn, _ws, 0, -1, WNN_USE_ZENGO);
    ILOG_DEBUG(ILX_INPUTHELPERJP, " -> segmentCount: %d\n", segmentCount);
    if (segmentCount < 0)
    {
        ILOG_WARNING(ILX_INPUTHELPERJP, "No bunsetsu!\n");
        return;
    }
    generateSegments(segmentCount);
#endif // ILIXI_HAVE_LIBWNN
}

void
InputHelperJP::resizeSegment(int direction)
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);

#if ILIXI_HAVE_LIBWNN

    if ((_currentSegment >= _segments.size() - 1) && (direction > 0))
        return;

    int length = jl_yomi_len(_wnn, _currentSegment, _currentSegment + 1);

    if ((direction < 1) && (length < 2))
        return;

    if (length > 0)
    {
        int mode = WNN_NO_USE;
        int ret;

        if (_currentSegment > 0)
            mode |= WNN_USE_MAE;
        if (_currentSegment < _segments.size() - 1)
            mode |= WNN_USE_ATO;

        int segmentCount = 0;
        ret = jl_nobi_conv(_wnn, _currentSegment, length + direction, -1, mode, WNN_SHO);
        if (ret == -1)
            ILOG_WARNING(ILX_INPUTHELPERJP, "Error: jl_nobi_conv()!\n");
        else
            segmentCount = ret;

        ILOG_DEBUG(ILX_INPUTHELPERJP, " -> segmentCount: %d\n", segmentCount);
        generateSegments(segmentCount);
        generateCandidates();
    }
#endif // ILIXI_HAVE_LIBWNN
}

void
InputHelperJP::generateSegments(int segmentCount)
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    _segments.clear();

#if ILIXI_HAVE_LIBWNN
    for (int i = 0; i < segmentCount; i++)
    {
        jl_get_kanji(_wnn, i, i + 1, _ws);
        char segmentBuffer[1024];
        eucwstoutf8(_ws, segmentBuffer);
        _segments.push_back(std::string(segmentBuffer));
        ILOG_DEBUG(ILX_INPUTHELPERJP, " -> part[%d]: %s\n", i, segmentBuffer);
    }
#endif // ILIXI_HAVE_LIBWNN
}

void
InputHelperJP::generateCandidates()
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    _candidates.clear();

#if ILIXI_HAVE_LIBWNN
    jl_zenkouho_dai(_wnn, getCurrentSegment(), getCurrentSegment() + 1, WNN_USE_ZENGO, WNN_UNIQ);
    int candidateCount = jl_zenkouho_suu(_wnn);

    for (int i = 0; i < candidateCount; i++)
    {
        jl_get_zenkouho_kanji(_wnn, i, _ws);
        char candidateBuffer[1024];
        eucwstoutf8(_ws, candidateBuffer);
        _candidates.push_back(std::string(candidateBuffer));
        ILOG_DEBUG(ILX_INPUTHELPERJP, " -> canditate[%d]: %s\n", i, candidateBuffer);
    }
#endif // ILIXI_HAVE_LIBWNN
}

void
InputHelperJP::preProcessInputData()
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    ILOG_DEBUG(ILX_INPUTHELPERJP, " -> romaji: %s\n", _data.c_str());
    int pos = 0;
    int len = 0;
    bool found;
    _pdata.clear();

    while (pos < _data.length())
    {
        len = 4; // 4-letter max
        if (len > _data.length() - pos)
            len = _data.length() - pos;

        found = false;

        while (len > 0 && !found)
        {
            std::string prefix = _data.substr(pos, len);
            if (_hiraganaMap.find(prefix) != _hiraganaMap.end())
            {
                _pdata.append(_hiraganaMap.find(prefix)->second);
                pos += len;
                found = true;
            }
            len--;
        }

        if (!found)
            pos++;
    }
    ILOG_DEBUG(ILX_INPUTHELPERJP, " -> hiragana: %s\n", _pdata.c_str());
}

void
InputHelperJP::initHiraganaMap()
{
    ILOG_TRACE_F(ILX_INPUTHELPERJP);
    // 4-letter romaji
    _hiraganaMap["xtsu"] = "っ";

    // 3-letter romaji
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

    // 2-letter romaji
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

    // 1-letter romaji
    _hiraganaMap["a"] = "あ";
    _hiraganaMap["i"] = "い";
    _hiraganaMap["u"] = "う";
    _hiraganaMap["e"] = "え";
    _hiraganaMap["o"] = "お";
    _hiraganaMap["n"] = "ん";

    // punctuation
    _hiraganaMap["."] = "。";
    _hiraganaMap[","] = "、";
    _hiraganaMap["-"] = "ー";
}

} /* namespace ilixi */
