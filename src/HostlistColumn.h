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

#ifndef HostlistColumn_h
#define HostlistColumn_h

#include <glib.h>
#include "config.h"

#include "Column.h"
#include "nagios.h"

class HostlistColumn : public Column
{
    int  _offset;
    bool _show_state;
public:
    HostlistColumn(string name, string description, int offset, int indirect_offset, bool show_state)
        : Column(name, description, indirect_offset), _offset(offset), _show_state(show_state) {}
    int type() { return COLTYPE_LIST; }
    void output(void *, Query *);
    Filter *createFilter(int opid, char *value);
    GTree *getMembers(gpointer data);
};



#endif // HostlistColumn_h

