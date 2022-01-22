#pragma once
#pragma comment(lib, "wbemuuid.lib")

#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <ntddscsi.h>

#define MB 1048576

/// <summary>
///		Computer statistics class
/// </summary>
class ComputerStatistics 
{

public:

	/// <summary>
	///		Return wstring if input is null
	/// </summary>
	/// 
	/// <param name="pString">Input string</param>
	/// 
	/// <returns>std::wstring</returns>
	static std::wstring SafeString(const wchar_t* pString) 
	{
		return std::wstring((pString == nullptr ? L"(null)" : pString));
	}

	/// <summary>
	///		Remove whitespaces
	/// </summary>
	/// 
	/// <param name="String">Input string</param>
	static void RemoveWhitespaces(std::wstring& String) 
	{
		String.erase(std::remove(String.begin(), String.end(), L' '), String.end());
	}

	/// <summary>
	///		Get cpu load in %
	/// </summary>
	/// 
	/// <returns>float</returns>
	float GetCPULoad()
	{
		FILETIME idleTime, kernelTime, userTime;

		return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
	}

	/// <summary>
	///		Get memory load in %
	/// </summary>
	/// 
	/// <returns>float</returns>
	float GetMemoryLoad()
	{
		MEMORYSTATUSEX memStat;
		memStat.dwLength = sizeof(memStat);
		GlobalMemoryStatusEx(&memStat);

		return memStat.dwMemoryLoad;
	}

private:

	/// <summary>
	///		Calculate cpu load
	/// </summary>
	/// 
	/// <param name="idleTicks">IDLE ticks count</param>
	/// <param name="totalTicks">Total ticks count</param>
	/// 
	/// <returns>float</returns>
	static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
	{
		static unsigned long long _previousTotalTicks = 0;
		static unsigned long long _previousIdleTicks = 0;

		unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
		unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

		float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

		_previousTotalTicks = totalTicks;
		_previousIdleTicks = idleTicks;

		return ret;
	}

	/// <summary>
	///		Convert to integer type
	/// </summary>
	/// 
	/// <param name="ft">File time type</param>
	/// 
	/// <returns>unsigned long long</returns>
	static unsigned long long FileTimeToInt64(const FILETIME& ft)
	{
		return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
	}

	/// <summary>
	///		Get computer hardware id
	/// </summary>
	/// 
	/// <param name="SubKey">Sub key</param>
	/// <param name="Value">Value</param>
	/// 
	/// <returns>std::wstring</returns>
	std::wstring GetHKLM(std::wstring SubKey, std::wstring Value) 
	{
		DWORD Size {};
		std::wstring Ret {};

		// Get string size
		RegGetValueW(
			HKEY_LOCAL_MACHINE,
			SubKey.c_str(),
			Value.c_str(),
			RRF_RT_REG_SZ,
			nullptr,
			nullptr,
			&Size
		);

		Ret.resize(Size);

		// Get hardware id
		RegGetValueW(
			HKEY_LOCAL_MACHINE,
			SubKey.c_str(),
			Value.c_str(),
			RRF_RT_REG_SZ,
			nullptr,
			&Ret[0],
			&Size
		);

		return Ret.c_str();
	}

	/// <summary>
	///		General query execute
	/// </summary>
	/// 
	/// <typeparam name="T">Type</typeparam>
	/// <param name="WMIClass">Class name</param>
	/// <param name="Field">Filed name</param>
	/// <param name="Value">Value</param>
	/// <param name="ServerName">Server name</param>
	template <typename T = const wchar_t*>
	void QueryWMI(std::wstring WMIClass, std::wstring Field, std::vector <T>& Value, const wchar_t* ServerName = L"ROOT\\CIMV2") 
	{
		// Build query
		std::wstring Query(L"SELECT ");
		Query.append(Field.c_str()).append(L" FROM ").append(WMIClass.c_str());

		// Initialization
		IWbemLocator* Locator {};
		IWbemServices* Services {};
		IEnumWbemClassObject* Enumerator {};
		IWbemClassObject* ClassObject {};
		VARIANT Variant {};
		DWORD Returned {};

		// Initialize COM library
		HRESULT hResult { CoInitializeEx(nullptr, COINIT_MULTITHREADED) };

		if (FAILED(hResult)) 
		{
			return;
		}

		// Set security properties
		hResult = CoInitializeSecurity(
			nullptr,
			-1,
			nullptr,
			nullptr,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			nullptr,
			EOAC_NONE,
			nullptr
		);

		if (FAILED(hResult))
		{
			CoUninitialize();
			return;
		}

		// Create COM instance
		hResult = CoCreateInstance(
			CLSID_WbemLocator,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			reinterpret_cast<PVOID*>(&Locator)
		);

		if (FAILED(hResult)) 
		{
			CoUninitialize();
			return;
		}

		// Connect to the WMI server
		hResult = Locator->ConnectServer(
			_bstr_t(ServerName),
			nullptr,
			nullptr,
			nullptr,
			NULL,
			nullptr,
			nullptr,
			&Services
		);

		if (FAILED(hResult)) 
		{
			Locator->Release();
			CoUninitialize();
			return;
		}

		// Set proxy
		hResult = CoSetProxyBlanket(
			Services,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			nullptr,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			nullptr,
			EOAC_NONE
		);

		if (FAILED(hResult))
		{
			Services->Release();
			Locator->Release();
			CoUninitialize();
			return;
		}

		// Execute custom query
		hResult = Services->ExecQuery(
			bstr_t(L"WQL"),
			bstr_t(Query.c_str()),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			nullptr,
			&Enumerator
		);

		if (FAILED(hResult)) 
		{
			Services->Release();
			Locator->Release();
			CoUninitialize();
			return;
		}

		// Process result
		while (Enumerator) 
		{
			HRESULT Res { Enumerator->Next(
				WBEM_INFINITE,
				1,
				&ClassObject,
				&Returned
			) };

			if (!Returned) 
			{
				break;
			}

			Res = ClassObject->Get(Field.c_str(), 0, &Variant, nullptr, nullptr);

			if (typeid(T) == typeid(long) || typeid(T) == typeid(int)) 
			{
				Value.push_back((T)Variant.intVal);
			}
			else if (typeid(T) == typeid(bool)) 
			{
				Value.push_back((T)Variant.boolVal);
			}
			else if (typeid(T) == typeid(unsigned int)) 
			{
				Value.push_back((T)Variant.uintVal);
			}
			else if (typeid(T) == typeid(unsigned short))
			{
				Value.push_back((T)Variant.uiVal);
			}
			else if (typeid(T) == typeid(long long)) 
			{
				Value.push_back((T)Variant.llVal);
			}
			else 
			{
				Value.push_back((T)((bstr_t)Variant.bstrVal).copy());
			}

			VariantClear(&Variant);
			ClassObject->Release();
		}

		if (!Value.size()) 
		{
			Value.resize(1);
		}

		// Free objects
		Services->Release();
		Locator->Release();
		Enumerator->Release();

		CoUninitialize();
	}

	/// <summary>
	///		Get disks information
	/// </summary>
	void QueryDisk() 
	{
		// Initialization
		std::wstring DrivePath { L"\\\\.\\PhysicalDrive" };
		std::wstring VolumePath { L"\\\\.\\" };
		HANDLE hVolume { nullptr };
		VOLUME_DISK_EXTENTS DiskExtents { NULL };
		DWORD IoBytes { NULL };
		ULARGE_INTEGER FreeBytesAvailable {};
		ULARGE_INTEGER TotalBytes {};
		int DriveCount { 0 };

		std::vector <const wchar_t*> SerialNumber {};
		std::vector <const wchar_t*> Model {};
		std::vector <const wchar_t*> Interface {};
		std::vector <const wchar_t*> Name {};
		std::vector <const wchar_t*> DeviceId {};
		std::vector <const wchar_t*> SortedDeviceId {};
		std::vector <const wchar_t*> FriendlyName {};
		std::vector <unsigned int> MediaType {};
		std::vector <bool> IsBoot {};

		// Get available physical disks
		HANDLE Handle { nullptr };
		for (;; DriveCount++) {
			Handle = CreateFileW(
				(DrivePath + std::to_wstring(DriveCount)).c_str(),
				NULL,
				NULL,
				nullptr,
				OPEN_EXISTING,
				NULL,
				nullptr
			);
			if (Handle == INVALID_HANDLE_VALUE) 
			{ 
				break; 
			}

			CloseHandle(Handle);
		}

		// Resize device container
		this->Disk.resize(DriveCount);
		SortedDeviceId.resize(DriveCount);

		// To get most of the data we want, we make several queries to the Windows Management Instrumentation (WMI) service
		// Queries to MSFT_PhysicalDisk and MSFT_Disk require a connection to the ROOT\\microsoft\\windows\\storage namespace
		QueryWMI(L"Win32_DiskDrive", L"SerialNumber", SerialNumber);
		QueryWMI(L"Win32_DiskDrive", L"Model", Model);
		QueryWMI(L"Win32_DiskDrive", L"InterfaceType", Interface);
		QueryWMI(L"Win32_DiskDrive", L"Name", Name);
		QueryWMI(L"Win32_LogicalDisk", L"DeviceId", DeviceId);
		QueryWMI(L"MSFT_PhysicalDisk", L"MediaType", MediaType, L"ROOT\\microsoft\\windows\\storage");
		QueryWMI(L"MSFT_PhysicalDisk", L"FriendlyName", FriendlyName, L"ROOT\\microsoft\\windows\\storage");
		QueryWMI(L"MSFT_Disk", L"IsBoot", IsBoot, L"ROOT\\microsoft\\windows\\storage");

		for (int i = 0; i < DriveCount; i++) 
		{
			for (int j = 0; j < DriveCount; j++) 
			{
				// To get necessary letter name we need to find it using DeviceIoControl
				hVolume = CreateFileW(
					(VolumePath + DeviceId.at(j)).c_str(),
					NULL,
					NULL,
					nullptr,
					OPEN_EXISTING,
					NULL,
					nullptr
				);

				// IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS will fill our buffer with a VOLUME_DISK_EXTENTS structure
				// First, we must get the number of disk extents
				DeviceIoControl(
					hVolume,
					IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
					nullptr,
					NULL,
					&DiskExtents,
					sizeof(DiskExtents),
					&IoBytes,
					nullptr
				);

				// VOLUME_DISK_EXTENTS contains an array of DISK_EXTENT structures. DISK_EXTENT contains a DWORD member, DiskNumber
				// DiskNumber will be the same number used to construct the name of the disk, which is PhysicalDriveX, where X is the DiskNumber
				DeviceIoControl(
					hVolume,
					IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
					nullptr,
					NULL,
					&DiskExtents,
					offsetof(VOLUME_DISK_EXTENTS, Extents[DiskExtents.NumberOfDiskExtents]),
					&IoBytes,
					nullptr
				);

				CloseHandle(hVolume);

				// To map the drive letter from Win32_LogicalDisk to the data returned by Win32_DiskDrive
				// We compare the drive letter's DiskNumber to the number at the end of the "Name" we recieve from Win32_DiskDrive
				// We then reorder the drive letters accordingly
				if (DiskExtents.Extents->DiskNumber == std::stoi(&SafeString(Name.at(i)).back())) 
				{
					SortedDeviceId.at(i) = DeviceId.at(j);
					break;
				}
			}
		}

		this->Disk.resize(DriveCount);

		for (int i = 0; i < DriveCount; i++) 
		{
			// Give the size and free space available corresponding to the drive letters we have
			GetDiskFreeSpaceEx(
				SafeString(SortedDeviceId.at(i)).c_str(),
				&FreeBytesAvailable,
				&TotalBytes,
				nullptr
			);

			// Save characteristics
			RemoveWhitespaces(this->Disk.at(i).SerialNumber = SafeString(SerialNumber.at(i)));
			this->Disk.at(i).Model = SafeString(Model.at(i));
			this->Disk.at(i).Interface = SafeString(Interface.at(i));
			this->Disk.at(i).DriveLetter = SafeString(SortedDeviceId.at(i));
			this->Disk.at(i).Size = TotalBytes.QuadPart / pow(1024, 3);
			this->Disk.at(i).FreeSpace = FreeBytesAvailable.QuadPart / pow(1024, 3);
			this->Disk.at(i).IsBootDrive = IsBoot.at(i);

			// Data from MSFT_PhysicalDisk will not be in the same order as Win32_DiskDrive
			// So we compare the "FriendlyName" from MSFT_PhysicalDisk with the "Model" from Win32_DiskDrive
			// We then reorder the data accordingly
			for (int j = 0; j < DriveCount; j++) 
			{
				if (!this->Disk.at(i).Model.compare(FriendlyName.at(j))) 
				{
					this->Disk.at(i).MediaType = MediaType.at(j);
				}
			}
		}
	}

	/// <summary>
	///		Get SMBIOS information
	/// </summary>
	void QuerySMBIOS() 
	{
		// Initialization
		std::vector <const wchar_t*> Manufacturer {};
		std::vector <const wchar_t*> Product {};
		std::vector <const wchar_t*> Version {};
		std::vector <const wchar_t*> SerialNumber {};

		// Get information
		QueryWMI(L"Win32_BaseBoard", L"Manufacturer", Manufacturer);
		QueryWMI(L"Win32_BaseBoard", L"Product", Product);
		QueryWMI(L"Win32_BaseBoard", L"Version", Version);
		QueryWMI(L"Win32_BaseBoard", L"SerialNumber", SerialNumber);

		// Save characteristics
		this->SMBIOS.Manufacturer = SafeString(Manufacturer.at(0));
		this->SMBIOS.Product = SafeString(Product.at(0));
		this->SMBIOS.Version = SafeString(Version.at(0));
		this->SMBIOS.SerialNumber = SafeString(SerialNumber.at(0));
	}

	/// <summary>
	///		Get CPU information
	/// </summary>
	void QueryProcessor() 
	{
		// Initialization
		std::vector <const wchar_t*> ProcessorId {};
		std::vector <const wchar_t*> Manufacturer {};
		std::vector <const wchar_t*> Name {};
		std::vector <int> Cores {};
		std::vector <int> Threads {};

		// Get information
		QueryWMI(L"Win32_Processor", L"ProcessorId", ProcessorId);
		QueryWMI(L"Win32_Processor", L"Manufacturer", Manufacturer);
		QueryWMI(L"Win32_Processor", L"Name", Name);
		QueryWMI<int>(L"Win32_Processor", L"NumberOfCores", Cores);
		QueryWMI<int>(L"Win32_Processor", L"NumberOfLogicalProcessors", Threads);

		// Save characteristics
		this->CPU.ProcessorId = SafeString(ProcessorId.at(0));
		this->CPU.Manufacturer = SafeString(Manufacturer.at(0));
		this->CPU.Name = SafeString(Name.at(0));
		this->CPU.Cores = Cores.at(0);
		this->CPU.Threads = Threads.at(0);
	}

	/// <summary>
	///		Get GPU information
	/// </summary>
	void QueryGPU() 
	{
		// Initialization
		std::vector <const wchar_t*> Name{};
		std::vector <const wchar_t*> DriverVersion{};
		std::vector <int> XRes{};
		std::vector <int> YRes{};
		std::vector <int> RefreshRate{};

		// Get information
		QueryWMI(L"Win32_VideoController", L"Name", Name);
		QueryWMI(L"Win32_VideoController", L"DriverVersion", DriverVersion);
		QueryWMI(L"Win32_VideoController", L"CurrentHorizontalResolution", XRes);
		QueryWMI(L"Win32_VideoController", L"CurrentVerticalResolution", YRes);
		QueryWMI(L"Win32_VideoController", L"CurrentRefreshRate", RefreshRate);

		// Save characteristics
		this->GPU.resize(Name.size());

		for (int i = 0; i < Name.size(); i++) 
		{
			this->GPU.at(i).Name = SafeString(Name.at(i));
			this->GPU.at(i).DriverVersion = SafeString(DriverVersion.at(i));
			this->GPU.at(i).XResolution = XRes.at(i);
			this->GPU.at(i).YResolution = YRes.at(i);
			this->GPU.at(i).RefreshRate = RefreshRate.at(i);
		}
	}

	/// <summary>
	///		Get OS information
	/// </summary>
	void QuerySystem() 
	{
		// Initialization
		std::vector <const wchar_t*> SystemName{};
		std::vector <const wchar_t*> OSVersion{};
		std::vector <const wchar_t*> OSName{};
		std::vector <const wchar_t*> OSArchitecture{};
		std::vector <const wchar_t*> OSSerialNumber{};
		std::vector <bool> IsHypervisorPresent{};

		// Get information
		QueryWMI(L"Win32_ComputerSystem", L"Name", SystemName);
		QueryWMI(L"Win32_ComputerSystem", L"Model", IsHypervisorPresent);
		QueryWMI(L"Win32_OperatingSystem", L"Version", OSVersion);
		QueryWMI(L"Win32_OperatingSystem", L"Name", OSName);
		QueryWMI(L"Win32_OperatingSystem", L"OSArchitecture", OSArchitecture);
		QueryWMI(L"Win32_OperatingSystem", L"SerialNumber", OSSerialNumber);

		// Save characteristics
		std::wstring wOSName{ SafeString(OSName.at(0)) };
		if (wOSName.find('|') != std::wstring::npos) {
			wOSName.resize(wOSName.find('|'));
		}
		this->System.Name = SafeString(SystemName.at(0));
		this->System.IsHypervisorPresent = IsHypervisorPresent.at(0);
		this->System.OSName = wOSName;
		this->System.OSVersion = SafeString(OSVersion.at(0));
		this->System.OSSerialNumber = SafeString(OSSerialNumber.at(0));
		this->System.OSArchitecture = SafeString(OSArchitecture.at(0));
	}

	/// <summary>
	///		Get network information
	/// </summary>
	void QueryNetwork() 
	{
		// Initialization
		std::vector <const wchar_t*> Name{};
		std::vector <const wchar_t*> MAC{};

		// Get information
		QueryWMI(L"Win32_NetworkAdapter", L"Name", Name);
		QueryWMI(L"Win32_NetworkAdapter", L"MACAddress", MAC);

		// Save characteristics
		this->NetworkAdapter.resize(Name.size());
		for (int i = 0; i < Name.size(); i++) 
		{
			this->NetworkAdapter.at(i).Name = SafeString(Name.at(i));
			this->NetworkAdapter.at(i).MAC = SafeString(MAC.at(i));
		}
	}

	// Get memory information
	void QueryPhysicalMemory() 
	{
		// Initialization
		std::vector<const wchar_t*> PartNumber{};

		// Get information
		MEMORYSTATUSEX memStat;
		memStat.dwLength = sizeof(memStat);
		GlobalMemoryStatusEx(&memStat);
		QueryWMI(L"Win32_PhysicalMemory", L"PartNumber", PartNumber);

		// Save characteristics
		this->PhysicalMemory.PartNumber = SafeString(PartNumber.at(0));
		this->PhysicalMemory.TotalSize = memStat.ullTotalPhys / MB;
		this->PhysicalMemory.AvailableSize = memStat.ullAvailPhys / MB;
		this->PhysicalMemory.TotalVirtualSize = memStat.ullTotalVirtual / MB;
		this->PhysicalMemory.AvailableVirtualSize = memStat.ullAvailVirtual / MB;
		this->PhysicalMemory.TotalPageSize = memStat.ullTotalPageFile / MB;
		this->PhysicalMemory.AvailablePageSize = memStat.ullAvailPageFile / MB;
	}

	/// <summary>
	///		Get information from registry
	/// </summary>
	void QueryRegistry() 
	{
		// Save hardware id
		this->Registry.ComputerHardwareId = SafeString(GetHKLM(L"SYSTEM\\CurrentControlSet\\Control\\SystemInformation", L"ComputerHardwareId").c_str());
		this->Registry.ComputerManufacturer = SafeString(GetHKLM(L"SYSTEM\\CurrentControlSet\\Control\\SystemInformation", L"SystemManufacturer").c_str());
		this->Registry.ComputerName = SafeString(GetHKLM(L"SYSTEM\\CurrentControlSet\\Control\\SystemInformation", L"SystemProductName").c_str());
	}

	/// <summary>
	///		Get all information
	/// </summary>
	void GetComputerStatistics() 
	{
		QueryDisk();
		QuerySMBIOS();
		QueryProcessor();
		QueryGPU();
		QuerySystem();
		QueryNetwork();
		QueryPhysicalMemory();
		QueryRegistry();
	}

public:

	/// <summary>
	///		Disks information
	/// </summary>
	struct DiskObject 
	{
		std::wstring SerialNumber {};
		std::wstring Model {};
		std::wstring Interface {};
		std::wstring DriveLetter {};
		long long Size {};
		long long FreeSpace {};
		unsigned int MediaType {};
		bool IsBootDrive {};
	}; std::vector <DiskObject> Disk {};

	/// <summary>
	///		SMBIOS information
	/// </summary>
	struct 
	{
		std::wstring Manufacturer {};
		std::wstring Product {};
		std::wstring Version {};
		std::wstring SerialNumber {};
	} SMBIOS;

	/// <summary>
	///		CPU information
	/// </summary>
	struct 
	{
		std::wstring ProcessorId {};
		std::wstring Manufacturer {};
		std::wstring Name {};
		int Cores {};
		int Threads {};
	} CPU;

	/// <summary>
	///		GPU information
	/// </summary>
	struct GPUObject 
	{
		std::wstring Name {};
		std::wstring DriverVersion {};
		int XResolution {};
		int YResolution {};
		int RefreshRate {};
	}; std::vector <GPUObject> GPU {};

	/// <summary>
	///		Network information
	/// </summary>
	struct NetworkAdapterObject 
	{
		std::wstring Name{};
		std::wstring MAC{};
	}; std::vector <NetworkAdapterObject> NetworkAdapter{};

	/// <summary>
	///		OS information
	/// </summary>
	struct 
	{
		std::wstring Name {};
		bool IsHypervisorPresent {};
		std::wstring OSVersion {};
		std::wstring OSName {};
		std::wstring OSArchitecture {};
		std::wstring OSSerialNumber {};
	} System;

	/// <summary>
	///		Memory information
	/// </summary>
	struct 
	{
		std::wstring PartNumber {};
		float TotalSize {};
		float AvailableSize {};
		float TotalVirtualSize {};
		float AvailableVirtualSize {};
		float TotalPageSize {};
		float AvailablePageSize {};
	} PhysicalMemory;

	/// <summary>
	///		Hardware id information
	/// </summary>
	struct 
	{
		std::wstring ComputerHardwareId{};
		std::wstring ComputerManufacturer {};
		std::wstring ComputerName {};
	} Registry;

	/// <summary>
	///		Constructor
	/// </summary>
	ComputerStatistics() 
	{
		GetComputerStatistics();
	}
};
