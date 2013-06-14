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

#ifndef PARSER_H_
#define PARSER_H_

#include "Pair.h"
#include "Node.h"

class Parser
{
public:
    Parser(const std::string&);

    ~Parser();

    std::string
    getResult();

private:
    Node*
    Eparser();

    Node*
    E();

    Node*
    T();

    Node*
    F();

    Node*
    P();

    void
    expect(std::string);

    void
    consume();

    std::string
    next();

    TokenType
    nexttoken();

    bool
    isDigit(const char);

    bool
    isOp(const char);

    void
    Scanner(std::string& str);

    PList::iterator _iter;
    PList _list;
    double _result;
};

#endif /* PARSER_H_ */
