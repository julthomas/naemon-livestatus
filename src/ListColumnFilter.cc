// +------------------------------------------------------------------+
// |             ____ _               _        __  __ _  __           |
// |            / ___| |__   ___  ___| | __   |  \/  | |/ /           |
// |           | |   | '_ \ / _ \/ __| |/ /   | |\/| | ' /            |
// |           | |___| | | |  __/ (__|   <    | |  | | . \            |
// |            \____|_| |_|\___|\___|_|\_\___|_|  |_|_|\_\           |
// |                                                                  |
// | Copyright Mathias Kettner 2012             mk@mathias-kettner.de |
// +------------------------------------------------------------------+
//
// This file is part of Check_MK.
// The official homepage is at http://mathias-kettner.de/check_mk.
//
// check_mk is free software;  you can redistribute it and/or modify it
// under the  terms of the  GNU General Public License  as published by
// the Free Software Foundation in version 2.  check_mk is  distributed
// in the hope that it will be useful, but WITHOUT ANY WARRANTY;  with-
// out even the implied warranty of  MERCHANTABILITY  or  FITNESS FOR A
// PARTICULAR PURPOSE. See the  GNU General Public License for more de-
// ails.  You should have  received  a copy of the  GNU  General Public
// License along with GNU Make; see the file  COPYING.  If  not,  write
// to the Free Software Foundation, Inc., 51 Franklin St,  Fifth Floor,
// Boston, MA 02110-1301 USA.

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "logger.h"
#include "opids.h"
#include "ListColumnFilter.h"
#include "ListColumn.h"
#include "OutputBuffer.h"

ListColumnFilter::ListColumnFilter(ListColumn *column, int opid, char *value)
    : _column(column)
    , _opid(opid)
    , _empty_ref(!value[0])
    , _regex(0)
{
    if (_opid == OP_REGEX || _opid == OP_REGEX_ICASE) {
        if (strchr(value, '{') || strchr(value, '}')) {
            setError(RESPONSE_CODE_INVALID_HEADER, "disallowed regular expression '%s': must not contain { or }", value);
        }
        else {
            _regex = new regex_t();
            if (0 != regcomp(_regex, value, REG_EXTENDED | REG_NOSUB | (_opid == OP_REGEX_ICASE ? REG_ICASE : 0)))
            {
                setError(RESPONSE_CODE_INVALID_HEADER, "invalid regular expression '%s'", value);
                delete _regex;
                _regex = 0;
            }
        }
    }
    else {
        _ref_member = _column->getNagiosObject(value);
    }
}

ListColumnFilter::~ListColumnFilter()
{
    if (_regex) {
        regfree(_regex);
        delete _regex;
    }
}

bool ListColumnFilter::accepts(void *data)
{
    data = _column->shiftPointer(data);
    if (!data)
        return false;
    switch (_opid) {
        case -OP_LESS: /* !< means >= means 'contains' */
            return _column->isNagiosMember(data, _ref_member);
        case OP_LESS:
            return !_column->isNagiosMember(data, _ref_member);
        case OP_EQUAL:
        case -OP_EQUAL:
            if (_empty_ref)
                return _column->isEmpty(data) == (_opid == OP_EQUAL);
            logger(LG_INFO, "Sorry, equality for lists implemented only for emptyness");
            return false;
        case OP_REGEX:
        case OP_REGEX_ICASE:
            return _regex != 0 && _column->isNagiosMatch(data, _regex);

        default:
            logger(LG_INFO, "Sorry, Operator %s for lists not implemented.", op_names_plus_8[_opid]);
            return true;
    }
}

void *ListColumnFilter::indexFilter(const char *columnname)
{
    if (_opid == -OP_LESS && !strcmp(columnname, _column->name()))
        return _ref_member;
    else
        return 0;
}
