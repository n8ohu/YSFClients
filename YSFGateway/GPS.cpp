/*
*   Copyright (C) 2016 by Jonathan Naylor G4KLX
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "GPS.h"
#include "YSFPayload.h"
#include "YSFDefines.h"
#include "Utils.h"
#include "Log.h"

#include <cstdio>
#include <cassert>
#include <cstring>

const unsigned char NULL_GPS[] = {0x47U, 0x63U, 0x5FU};
const unsigned char SHRT_GPS[] = {0x22U, 0x62U, 0x5FU};
const unsigned char LONG_GPS[] = {0x47U, 0x64U, 0x5FU};

CGPS::CGPS(const std::string& hostname, unsigned int port, const std::string& password) :
m_hostname(hostname),
m_port(port),
m_password(password),
m_buffer(NULL),
m_dt1(false),
m_dt2(false),
m_sent(false)
{
	assert(port > 0U);

	m_buffer = new unsigned char[20U];
}

CGPS::~CGPS()
{
	delete[] m_buffer;
}

void CGPS::data(const unsigned char* source, const unsigned char* data, unsigned char fi, unsigned char dt, unsigned char fn)
{
	if (m_sent)
		return;

	// In case we had the data but not the source callsign
	if (m_dt1 && m_dt2) {
		transmitGPS(source);
		return;
	}

	CYSFPayload payload;

	if (fi == YSF_FI_COMMUNICATIONS && dt == YSF_DT_VD_MODE1) {
		if (fn == 3U && !m_dt1) {
			bool valid = payload.readVDMode1Data(data, m_buffer);
			if (valid) {
				m_dt1 = true;
				m_dt2 = true;

				// If no GPS data then mark it as complete for this transmission
				if (::memcmp(m_buffer + 1U, NULL_GPS, 3U) == 0) {
					CUtils::dump("Null GPS data received", m_buffer, 20U);
					m_sent = true;
				}

				if (::memcmp(m_buffer + 1U, SHRT_GPS, 3U) == 0) {
					CUtils::dump("Short GPS data received", m_buffer, 20U);
					transmitGPS(source);
				}

				if (::memcmp(m_buffer + 1U, LONG_GPS, 3U) == 0) {
					CUtils::dump("Long GPS data received", m_buffer, 20U);
					transmitGPS(source);
				}
			}
		}
	} else if (fi == YSF_FI_COMMUNICATIONS && dt == YSF_DT_VD_MODE2) {
		if (fn == 6U && !m_dt1) {
			bool valid = payload.readVDMode2Data(data, m_buffer + 0U);
			if (valid) {
				m_dt1 = true;

				// If no GPS data then mark it as complete for this transmission
				if (::memcmp(m_buffer + 1U, NULL_GPS, 3U) == 0) {
					CUtils::dump("Null GPS data received", m_buffer, 20U);
					m_sent = true;
					m_dt2  = true;
				}
			}
		}
		
		if (fn == 7U && !m_dt2) {
			bool valid = payload.readVDMode2Data(data, m_buffer + 10U);
			if (valid) {
				m_dt2  = true;

				if (::memcmp(m_buffer + 1U, SHRT_GPS, 3U) == 0) {
					CUtils::dump("Short GPS data received", m_buffer, 20U);
					transmitGPS(source);
				}

				if (::memcmp(m_buffer + 1U, LONG_GPS, 3U) == 0) {
					CUtils::dump("Long GPS data received", m_buffer, 20U);
					transmitGPS(source);
				}
			}
		}
	}
}

void CGPS::reset()
{
	m_dt1  = false;
	m_dt2  = false;
	m_sent = false;
}

void CGPS::transmitGPS(const unsigned char* source)
{
	// We don't know who its from!
	if (::memcmp(source, "          ", YSF_CALLSIGN_LENGTH) == 0)
		return;


	m_sent = true;
}