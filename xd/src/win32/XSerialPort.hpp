/*******************************************************************************
** 
** Copyright (C) 2016 The Xd Company Ltd.
**
** Windows串口类
**
** Date: 2016-06-14
** Author: XD
** Contact: QQ 978028760
**
*******************************************************************************/

#ifndef _X_SERIAL_PORT_HPP
#define _X_SERIAL_PORT_HPP

#include <Windows.h>
#include "../XString.hpp"
#include "../XMutexLocker.hpp"

class XSerialPort {
public:
	// ref struct DCB
	enum BaudRate {
		Baud1200 = 1200,
		Baud2400 = 2400,
		Baud4800 = 4800,
		Baud9600 = 9600,
		Baud19200 = 19200,
		Baud38400 = 38400,
		Baud57600 = 57600,
		Baud115200 = 115200,
		UnknownBaud = -1
	}; 
	enum DataBits {
		Data5 = 5,
		Data6 = 6,
		Data7 = 7,
		Data8 = 8,
		UnknownDataBits = -1
	};
	enum Parity {
		NoParity = 0,
		OddParity = 1,
		EvenParity = 2,
		MarkParity = 3,
		SpaceParity = 4,
		UnknownParity = -1
	};
	enum StopBits {
		OneStop = 0,
		OneAndHalfStop = 1,
		TwoStop = 2,
		UnknownStopBits = -1
	};
	enum FlowControl {
		NoFlowControl,
		HardwareControl,
		SoftwareControl,
		UnknownFlowControl = -1
	};

	XSerialPort()
		: _baudRate(UnknownBaud)
		, _dataBits(UnknownDataBits)
		, _parity(UnknownParity)
		, _stopBits(UnknownStopBits)
		, _flowControl(UnknownFlowControl)
		, _handle(nullptr)
		, _isOpen(false){}
	~XSerialPort(){
		if(isOpen()){
			close();
		}
	}

	void setPortName(const XString& portName){ _portName = portName; }
	const XString& portName()const{ return _portName; }
	void setBaudRate(BaudRate baudRate){ _baudRate = baudRate; }
	BaudRate baudRate()const{ return _baudRate; }
	void setDataBits(DataBits dataBits){ _dataBits = dataBits; }
	DataBits dataBits()const{ return _dataBits; }
	void setParity(Parity parity){ _parity = parity; }
	Parity parity()const{ return _parity; }
	void setStopBits(StopBits stopBits){ _stopBits = stopBits; }
	StopBits stopBits()const{ return _stopBits; }
	void setFlowControl(FlowControl flowControl){ _flowControl = flowControl; }
	FlowControl flowControl()const{ return _flowControl; }

	bool open(){
		if (isOpen()){
			close();
		}
		_handle = CreateFile(_portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (_handle == INVALID_HANDLE_VALUE){
			return false;
		}

		if (::SetupComm(_handle, 1024, 1024)==FALSE){
			CloseHandle(_handle);
			return false;
		}

		COMMTIMEOUTS timeOuts;
		GetCommTimeouts(_handle, &timeOuts);
		timeOuts.ReadIntervalTimeout = MAXDWORD;
		timeOuts.ReadTotalTimeoutConstant = 0;
		timeOuts.ReadTotalTimeoutMultiplier = 0;
		timeOuts.WriteTotalTimeoutConstant = 1000;
		timeOuts.WriteTotalTimeoutMultiplier = 50;
		SetCommTimeouts(_handle, &timeOuts);

		DCB dcb;
		if (!GetCommState(_handle, &dcb)){
			CloseHandle(_handle);
			return false;
		}
		dcb.BaudRate = _baudRate;
		dcb.ByteSize = _dataBits;
		dcb.StopBits = _stopBits;
		dcb.Parity = _parity;
		if (!SetCommState(_handle, &dcb)){
			CloseHandle(_handle);
			return false;
		}

		::ZeroMemory(&_overlapped, sizeof(_overlapped));
		if (_overlapped.hEvent){
			ResetEvent(_overlapped.hEvent);
			_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		}

		_isOpen = true;
		return true;
	}
	bool close(){
		bool retval = (::CloseHandle(_handle) == TRUE);
        _handle = nullptr;
		_isOpen = false;
		return retval;
	}

	int read(char buf[], const int maxSize){
		XMutexLocker locker(&_commMutex);

		int retval = 0;
		DWORD err = 0;
		DWORD recvCount = 0;
		if (ClearCommError(_handle, &err, NULL)){
			PurgeComm(_handle, PURGE_TXABORT | PURGE_TXCLEAR);
		}
		if (!ReadFile(_handle, buf, maxSize, &recvCount, &_overlapped)){
			if (GetLastError() == ERROR_IO_PENDING){
				while (!GetOverlappedResult(_handle, &_overlapped, &recvCount, FALSE)){
					if (GetLastError() == ERROR_IO_INCOMPLETE){
						continue;
					}
					else{
						ClearCommError(_handle, &err, NULL);
						break;
					}
				}
			}
		}
		retval = recvCount;
		return retval;
	}
	bool write(const char* buf, int size){
		XMutexLocker locker(&_commMutex);

		DWORD err = 0;
		DWORD sendCount = 0;
		if (ClearCommError(_handle, &err, NULL)){
			PurgeComm(_handle, PURGE_TXABORT | PURGE_TXCLEAR);
		}
		if (!WriteFile(_handle, buf, size, &sendCount, &_overlapped)){
			err = GetLastError();
			if (err == ERROR_IO_PENDING){
				while (!GetOverlappedResult(_handle, &_overlapped, &sendCount, FALSE)){
					err = GetLastError();
					if ((err == ERROR_IO_INCOMPLETE)){
						continue;
					}
					else{
						ClearCommError(_handle, &err, NULL);
						return false;
					}
				}
				return true;
			}
			return false;
		}
		return true;
	}

	bool isOpen()const{ return _isOpen; }

	static XList<XString> availablePorts(){
		XList<XString> ports;
		HKEY hk = nullptr;
		HRESULT hr = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			L"HARDWARE\\DEVICEMAP\\SERIALCOMM", NULL, KEY_READ | KEY_QUERY_VALUE, &hk);
		if (hr == S_OK&&hk){
			TCHAR key[MAX_PATH];
			TCHAR val[MAX_PATH];
			DWORD type = 0;
			for (int i = 0;; i++){
				DWORD szKey = 255;
				DWORD szVal = 255;
				hr = ::RegEnumValue(hk, i, key, &szKey, NULL, &type, (LPBYTE)val, &szVal);
				if (hr == S_OK){
					ports.append(val);
				}
				else
					break;
			}
			::RegCloseKey(hk);
		}
		return ports;
	}

protected:
private:
	XString		_portName;
	BaudRate	_baudRate;
	DataBits	_dataBits;
	Parity		_parity;
	StopBits	_stopBits;
	FlowControl	_flowControl;

	HANDLE		_handle;
	OVERLAPPED	_overlapped;
	bool		_isOpen;
	std::mutex	_commMutex;
};


#endif // _X_SERIAL_PORT_HPP


