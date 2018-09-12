/*
 * SecuwallMgmtFile.cpp - hosts(5) file format parser implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */



#include "fwbuilder/SecuwallMgmtFile.h"
#include "fwbuilder/Tools.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace libfwbuilder;

void SecuwallMgmtFile::parse(const string &filename)
{
    ifstream f(filename.c_str(), ios::in);
    if (!f)
    {
        throw FWException("Can't open file '" + filename + "'");
    }
    else
    {
        parse(f);
    }
}

/**
 * Does the actual parsing.
 */
void SecuwallMgmtFile::parse(istream &from)
{
    enum
    {
        s_begin,
        s_key,
        s_space,
        s_value,
        s_rest
    } state;

    state = s_begin;

    string key;
    string temp;

    char c;
    int  ln = 1;

    while (from.get(c))
    {
        switch (state)
        {
        case s_begin:
            if (c == '#')
            {
                /* Ignore commented lines */
                state = s_rest;
                break;
            }
            else if (c != '\n' && c != ' ' && c != '\t' && c != '\"')
            {
                /* Found key entry */
                state = s_key;
            }
            else
                break;
        case s_key:
            if (c == '=' || c == ' ' || c == '\t' || c == '\"')
            {
                /* End of key */
                if (temp.empty())
                {
                    ostringstream err;
                    cerr << "Found line without key: " << ln;
                    throw FWException(err.str());
                }
                key = temp;
                temp.clear();
                state = s_space;
            }
            else
            {
                /* Processing key entry */
                temp += c;
            }
            break;
        case s_space:
            if (c == '#' || c == '\n')
            {
                /* Key entry without value */
                ostringstream err;
                cerr << "Found key without value: " << ln;
                throw FWException(err.str());
            }
            else if (c == ' ' || c == '\t')
            {
                /* Ignore whitespaces */
                break;
            }
            else
            {
                /* Process value */
                state = s_value;
            }
            /* FALLTHROUGH */
        case s_value:
            if (c == ' ' || c == '\t' || c == '#' || c == '\n')
            {
                /* Store data */
                data.insert(make_pair(key, temp));
                /* Reset processing variables */
                key.clear();
                temp.clear();
                if (c == '\n')
                {
                    /* Process next line */
                    ln++;
                    state = s_begin;
                }
                else
                {
                    /* Process rest of line */
                    state = s_rest;
                }
                break;
            }
            else if (c == '\"')
            {
                /* Ignore Quotation marks */
                break;
            }
            else
            {
                temp += c;
                break;
            }
        case s_rest:
            if (c == '\n')
            {
                ln++;
                state = s_begin;
            }
            break;
        }
    }
}

