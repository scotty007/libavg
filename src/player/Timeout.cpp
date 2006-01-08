//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2006 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "Timeout.h"

#include "../base/TimeSource.h"
#include "../base/Exception.h"

#include <boost/python.hpp>

#include <iostream>

using namespace boost::python;
using namespace std;

namespace avg {

int Timeout::s_LastID = 0;

Timeout::Timeout(int time, PyObject * pyfunc, bool isInterval)
    : m_Interval(time),
      m_PyFunc(pyfunc),
      m_IsInterval(isInterval)
{
    m_NextTimeout = m_Interval+TimeSource::get()->getCurrentMillisecs();
    s_LastID++;
    m_ID = s_LastID;

    Py_INCREF(m_PyFunc);
}

Timeout::~Timeout()
{
    Py_DECREF(m_PyFunc);
}

bool Timeout::IsReady() const
{
    return m_NextTimeout <= TimeSource::get()->getCurrentMillisecs();
}

bool Timeout::IsInterval() const
{
    return m_IsInterval;
}

void Timeout::Fire()
{
    PyObject * arglist = Py_BuildValue("()");
    PyObject * result = PyEval_CallObject(m_PyFunc, arglist);
    Py_DECREF(arglist);    
    if (!result) {
        throw error_already_set();
    }
    Py_DECREF(result);
    if (m_IsInterval) {
        m_NextTimeout = m_Interval + TimeSource::get()->getCurrentMillisecs();
    }
}

int Timeout::GetID() const
{
    return m_ID;
}

bool Timeout::operator <(const Timeout& other) const
{
    return m_NextTimeout < other.m_NextTimeout;
}

}
