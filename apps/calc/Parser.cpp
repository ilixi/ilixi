/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Sonat Deniz <sonat@ilixi.org>.

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

#include "Parser.h"
#include "NumNode.h"
#include "OpNode.h"
#include "UnaryNode.h"
#include "Pair.h"

#include <sstream>
#include <stdexcept>

Parser::Parser(const std::string& str)
{
    std::string txt = str;
    Scanner(txt);
    _iter = _list.begin();
    Node* root = Eparser();
    _result = root->evaluate();
    delete root;
}

Parser::~Parser()
{
    for (PList::iterator it = _list.begin(); it != _list.end(); ++it)
    {
        delete *it;
    }
}

std::string
Parser::getResult()
{
    std::ostringstream o;
//    o << std::fixed;
    if (!(o << _result))
        throw Exception("Malformed Expression");
    return o.str();
}

Node*
Parser::Eparser()
{
    Node* t;
    t = E();
    expect("end");
    return t;
}
Node*
Parser::E()
{
    Node* t;
    t = T();
    while (next() == "+" || next() == "-")
    {
        std::string op = next();
        consume();
        Node* t1 = T();
        t = new OpNode(op, t, t1);
    }
    return t;
}
Node*
Parser::T()
{
    Node* t;
    t = F();
    while (next() == "*" || next() == "/")
    {
        std::string op = next();
        consume();
        Node* t1 = F();
        t = new OpNode(op, t, t1);
    }
    return t;
}
Node*
Parser::F()
{
    Node* t;
    t = P();
    if (next() == "^" || next() == "%")
    {
        std::string op = next();
        consume();
        Node* t1 = P();
        t = new OpNode(op, t, t1);
    }
    return t;
}
Node*
Parser::P()
{
    Node* t;
    if (nexttoken() == NUMBER)
    {
        t = new NumNode(next());
        consume();
        return t;
    } else if (next() == "(")
    {
        consume();
        t = E();
        expect(")");
        return t;
    } else if (next() == "|")
    {
        consume();
        t = E();
        return new UnaryNode("|", t);
    } else if (next() == "-")
    {
        consume();
        t = E();
        return new UnaryNode("-", t);
    } else
    {
        throw Exception("Malformed Expression");
    }
}

void
Parser::expect(std::string c)
{
    if (next() == c)
    {
        consume();
    } else
    {
        throw Exception("Malformed Expression");
    }

}
std::string
Parser::next()
{
    PList::iterator lcl = _iter;
    lcl++;
    if (lcl == _list.end())
    {
        return "end";
    } else
    {
        std::string tmp = (*lcl)->value;
        return tmp;
    }
}
TokenType
Parser::nexttoken()
{
    PList::iterator lcl = _iter;
    lcl++;
    if (lcl == _list.end())
    {
        return NON;
    } else
    {
        TokenType tmp = (*lcl)->tokentype;
        return tmp;
    }
}
void
Parser::consume()
{
    ++_iter;
}

void
Parser::Scanner(std::string& str)
{
    _list.push_back(new Pair(NON, "empty"));
    std::string::iterator iter = str.begin();

    for (iter = str.begin(); iter != str.end(); ++iter)
    {
        std::string last = "";
        if (isDigit(*iter))
        {
            std::string::iterator dest_iter = iter;
            last.append(1, *dest_iter);
            while (isDigit(*(dest_iter + 1)))
            {
                ++dest_iter;
                last.append(1, *dest_iter);
            }
            iter = dest_iter;
            _list.push_back(new Pair(NUMBER, last));
        } else if (isOp(*iter))
        {
            last.append(1, *iter);
            _list.push_back(new Pair(OPERATOR, last));
        } else
        {
            throw Exception("Unknown Variable");
        }
    }
}

bool
Parser::isDigit(const char c)
{
    std::string digit = "0123456789.";
    std::string::iterator iter;
    for (iter = digit.begin(); iter != digit.end(); ++iter)
    {
        if (*iter == c)
        {
            return true;
        }
    }
    return false;
}
bool
Parser::isOp(const char c)
{
    std::string digit = "/*-+^()|%";
    std::string::iterator iter;
    for (iter = digit.begin(); iter != digit.end(); ++iter)
    {
        if (*iter == c)
        {
            return true;
        }
    }
    return false;
}

