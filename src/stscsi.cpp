/*
 * File: stscsi.cpp
 *
 * Copyright (c) 2010-2011 Freescale Semiconductor, Inc. All rights reserved.
 * See included license file for license details.
*/
#include "stheader.h"
#include "stglobals.h"
#include "stversioninfo.h"
#include "ddildl_defs.h"
#include "stbytearray.h"
#include "stddiapi.h"
#include "stscsi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStScsi::CStScsi(string name):CStBase(name)
{
	m_last_error = STERR_NONE;
//	m_drive_letter = L'\0';
//	m_handle = INVALID_HANDLE_VALUE;
	m_p_arr_sense_data = NULL;
}

CStScsi::~CStScsi()
{
}

SCSI_STATE CStScsi::GetState()
{
	return m_state;
}

ST_ERROR CStScsi::SendDdiApiCommand(CStDdiApi* pApi)
{
	ST_ERROR result = STERR_NONE;
	size_t size;
	pApi->PrepareCommand();
	
	result=pApi->GetResponseSize(size);

	CStByteArray byte_array(size);
	
	result = SendCommand( pApi->GetCommandArray(), (UCHAR)pApi->GetCommandSize(), pApi->IsCommandTypeWrite(), byte_array);
	if(result != STERR_NONE)
		return result;

//	CStByteArray arr = byte_array;
//	arr = *pApi->GetCommandArray();
	if( ! pApi->IsCommandTypeWrite() )
	{
		return pApi->ProcessResponse(byte_array);
	}

	return STERR_NONE;
}


ST_ERROR CStScsi::IsSystemMedia( ST_BOOLEAN& _media_system )
{
	//ST_ERROR result=STERR_NONE;
	//CStGetLogicalMediaInfo api(kMediaInfoIsSystemMedia);
	//if(api.GetLastError() != STERR_NONE)
	//	return api.GetLastError();

	//result = SendDdiApiCommand(&api);
	//if(result != STERR_NONE)
	//	return result;

	//return api.IsSystemMedia(_media_system);
	
	_media_system = FALSE;
	return STERR_NONE;
}

ST_ERROR CStScsi::WriteSector(CStByteArray* _p_sector, ULONG _num_sectors, ULONG _start_sector_number, ULONG _sector_size)
{
	ST_ERROR result=STERR_NONE;
	CStWrite api(_start_sector_number, _num_sectors, _sector_size);
	if(api.GetLastError() != STERR_NONE)
		return (ST_ERROR)api.GetLastError();

	result = (ST_ERROR)api.PutData(*_p_sector);
	if(result != STERR_NONE)
		return result;

	return (ST_ERROR)SendDdiApiCommand(&api);
}

ST_ERROR CStScsi::ReadSector(CStByteArray* _p_sector, ULONG _num_sectors, ULONG _start_sector_number, ULONG _sector_size)
{
	ST_ERROR result=STERR_NONE;
	CStRead api(_start_sector_number, _num_sectors, _sector_size);
	if(api.GetLastError() != STERR_NONE)
		return (ST_ERROR)api.GetLastError();

	result = (ST_ERROR)SendDdiApiCommand(&api);
	if(result != STERR_NONE)
		return result;

	return (ST_ERROR)api.GetData(*_p_sector);
}


void CStScsi::SaveSenseData(UCHAR* _sense_data, UCHAR _sense_len)
{
	if( m_p_arr_sense_data )
	{
		delete m_p_arr_sense_data;
		m_p_arr_sense_data = NULL;
	}

	m_p_arr_sense_data = new CStByteArray( (size_t)_sense_len );
	
	for(size_t index = 0; index < m_p_arr_sense_data->GetCount(); index ++ )
	{
		m_p_arr_sense_data->SetAt( index, _sense_data[index] );
	}
}

wstring CStScsi::GetSenseData()
{
	if( !m_p_arr_sense_data )
		return L"";
	
	UCHAR			sense_key;
	USHORT			sense_code;

	m_p_arr_sense_data->GetAt( ST_SENSE_DATA_SENSE_KEY_OFFSET, sense_key );

	if( sense_key != SCSI_SENSE_UNIQUE )
	{
		return m_p_arr_sense_data->GetAsString();
	}

	m_p_arr_sense_data->Read( sense_code, ST_SENSE_DATA_ASC_OFFSET );

	wstring msg;

//	if( GetUpdater()->GetResource()->GetErrorMessage( sense_code, msg ) == STERR_UNKNOWN_VENDOR_SPECIFIC_SENSE_CODE )
//	{
//		msg += L"\n" + m_p_arr_sense_data->GetAsString();
//	}

	return msg;
}

ST_ERROR CStScsi::FormatPartition( PDISK_GEOMETRY _p_dg, ULONG _hidden_sectors )
{
	return STERR_NONE;
	UNREFERENCED_PARAMETER(_p_dg);
	UNREFERENCED_PARAMETER(_hidden_sectors);
}

ST_ERROR CStScsi::DriveLayout( PDRIVE_LAYOUT_INFORMATION _p_dl )
{
	return STERR_NONE;
	UNREFERENCED_PARAMETER(_p_dl);
}


ST_ERROR CStScsi::OpenPhysicalDrive()
{
	return STERR_NONE;
}
