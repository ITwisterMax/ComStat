#pragma once
#pragma comment(lib, "Winmm.lib")

#include <algorithm>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <vector>
#include "../Api/comstat.h"

#define FOREGROUND_WHITE 0x0007
#define VK_Z 0x5A

#define MUSIC_FILE L"musics\\music.wav"
#define HELP_FILE L"web\\index.html"
#define LOG_FILE L"logs\\log.csv"
#define STATISTICS_FILE L"logs\\statistics.csv"

std::ofstream statisticsFile;

/// <summary>
///		CommandLine 
/// </summary>
namespace CommandLine 
{
	// Initialization
	ComputerStatistics HWID {};
	std::wstring CurCmd { L"" };
	std::wstring AppName { L"   ______                            __               \n"
						  L"  / ____/___  ____ ___  ____  __  __/ /____  _____    \n"
						  L" / /   / __ \\/ __ `__ \\/ __ \\/ / / / __/ _ \\/ ___/\n"
						  L"/ /___/ /_/ / / / / / / /_/ / /_/ / /_/  __/ /        \n"
						  L"\\____/\\____/_/ /_/ /_/ .___/\\__,_/\\__/\\___/_/    \n"
						  L"                    /_/                               \n"
						  L"   _____ __        __  _      __  _                   \n"
						  L"  / ___// /_____ _/ /_(_)____/ /_(_)_________         \n"
						  L"  \\__ \\/ __/ __ `/ __/ / ___/ __/ / ___/ ___/       \n"
						  L" ___/ / /_/ /_/ / /_/ (__  ) /_/ / /__(__  )          \n"
						  L"/____/\\__/\\__,_/\\__/_/____/\\__/_/\\___/____/    \n\n"
	};
	std::wstring CmdName { L"ComStat> " };

	/// <summary>
	///		Valid commands enum
	/// </summary>
	enum 
	{
		eInvalid,
		eDisk,
		eSMBIOS,
		eGPU,
		eCPU,
		eNetwork,
		eSystem,
		ePhysicalMemory,
		eRegistry,
		eAll,
		eRealTime,
		eMusicOn,
		eMusicOff,
		eSave,
		eHelp,
		eExit
	};

	/// <summary>
	///		Valid commands name
	/// </summary>
	std::vector <std::wstring> ValidCommands
	{
		L"invalid command",
		L"disk",
		L"smbios",
		L"gpu",
		L"cpu",
		L"network",
		L"system",
		L"physicalmemory",
		L"registry",
		L"all",
		L"realtime",
		L"musicon",
		L"musicoff",
		L"save",
		L"help",
		L"exit"
	};

	/// <summary>
	///		Valid disks sub commands name
	/// </summary>
	std::vector <std::wstring> DiskSubCommands
	{
		L"invalid command",
		L"serialnumber",
		L"model",
		L"interface",
		L"driveletter",
		L"size",
		L"freespace",
		L"mediatype",
		L"isbootdrive"
	};

	/// <summary>
	///		Valid SMBIOS sub commands name
	/// </summary>
	std::vector <std::wstring> SMBIOSSubCommands
	{
		L"invalid command",
		L"serialnumber",
		L"manufacturer",
		L"product",
		L"version"
	};

	/// <summary>
	///		Valid GPU sub commands name
	/// </summary>
	std::vector <std::wstring> GPUSubCommands
	{
		L"invalid command",
		L"name",
		L"driverversion",
		L"resolution",
		L"refreshrate"
	};

	/// <summary>
	///		Valid CPU sub commands name
	/// </summary>
	std::vector <std::wstring> CPUSubCommands
	{
		L"invalid command",
		L"processorid",
		L"manufacturer",
		L"name",
		L"cores",
		L"threads"
	};

	/// <summary>
	///		Valid network sub commands name
	/// </summary>
	std::vector <std::wstring> NetworkSubCommands
	{
		L"invalid command",
		L"name",
		L"mac"
	};

	/// <summary>
	///		Valid OS sub commands name
	/// </summary>
	std::vector <std::wstring> SystemSubCommands
	{
		L"invalid command",
		L"name",
		L"ishypervisorpresent",
		L"osversion",
		L"ostitle",
		L"osarchitecture",
		L"osserialnumber"
	};

	/// <summary>
	///		Valid memory sub commands name
	/// </summary>
	std::vector <std::wstring> PhysicalMemorySubCommands
	{
		L"invalid command",
		L"partnumber",
		L"totalsize",
		L"availablesize",
		L"totalvirtualsize",
		L"availablevirtualsize",
		L"totalpagefilesize",
		L"availablepagefilesize",
	};

	/// <summary>
	///		Valid registry sub commands name
	/// </summary>
	std::vector <std::wstring> RegistrySubCommands
	{
		L"invalid command",
		L"computerhardwareid",
		L"computermanufacturer",
		L"computerproductname",
	};

	/// <summary>
	///		Container for commands
	/// </summary>
	struct 
	{
		int CommandIndex {};
		std::vector <int> SubCommandIndex {};
	} ParsedCommand;

	/// <summary>
	///		Iteration output
	/// </summary>
	/// 
	/// <typeparam name="T">Type</typeparam>
	/// <param name="ToPrint">Values</param>
	/// <param name="HWType">HW type</param>
	/// <param name="Iterator">Iterator</param>
	template <typename T>
	void FmtPrint(T ToPrint, int HWType, int Iterator) 
	{
		std::wcout << ToPrint;

		// Available for disks, GPU and network
		switch (HWType) 
		{
		case eDisk: 
		{
			std::wcout << (Iterator && Iterator + 1 == HWID.Disk.size() ? L"\n\n" : L"\n");
		} break;

		case eGPU: 
		{
			std::wcout << (Iterator && Iterator + 1 == HWID.GPU.size() ? L"\n\n" : L"\n");
		} break;

		case eNetwork: 
		{
			std::wcout << (Iterator && Iterator + 1 == HWID.NetworkAdapter.size() ? L"\n\n" : L"\n");
		} break;
		}
	}

	/// <summary>
	///		Print disks information
	/// </summary>
	void PrintDisks() 
	{
		for (int i = 0; i < HWID.Disk.size(); i++) 
		{
			std::wcout << L"Model:\t\t\t" << HWID.Disk.at(i).Model << std::endl;
			std::wcout << L"Serial Number:\t\t" << HWID.Disk.at(i).SerialNumber << std::endl;
			std::wcout << L"Interface Type:\t\t" << HWID.Disk.at(i).Interface << std::endl;
			std::wcout << L"Drive Letter:\t\t" << HWID.Disk.at(i).DriveLetter << std::endl;
			std::wcout << L"Size:\t\t\t" << HWID.Disk.at(i).Size << L" GB" << std::endl;
			std::wcout << L"Free Space:\t\t" << HWID.Disk.at(i).FreeSpace << L" GB" << std::endl;
			std::wcout << L"Media Type:\t\t" << (HWID.Disk.at(i).MediaType == 4 ? L"SSD" : (HWID.Disk.at(i).MediaType == 3 ? L"HDD" : L"")) << std::endl;
			std::wcout << L"Boot Drive:\t\t" << (HWID.Disk.at(i).IsBootDrive ? L"Yes" : L"No") << std::endl;

			if (i + 1 < HWID.Disk.size()) 
			{ 
				std::wcout << std::endl; 
			}
		}
	}

	/// <summary>
	///		Print SMBIOS information
	/// </summary>
	void PrintSMBIOS() 
	{
		std::wcout << L"Manufacturer:\t\t" << HWID.SMBIOS.Manufacturer << std::endl;
		std::wcout << L"Product:\t\t" << HWID.SMBIOS.Product << std::endl;
		std::wcout << L"Version:\t\t" << HWID.SMBIOS.Version << std::endl;
		std::wcout << L"Serial Number:\t\t" << HWID.SMBIOS.SerialNumber << std::endl;
	}

	/// <summary>
	///		Print GPU information
	/// </summary>
	void PrintGPUs() 
	{
		for (int i = 0; i < HWID.GPU.size(); i++) 
		{
			std::wcout << L"Name:\t\t\t" << HWID.GPU.at(i).Name << std::endl;
			std::wcout << L"Driver Version:\t\t" << HWID.GPU.at(i).DriverVersion << std::endl;
			std::wcout << L"Resolution:\t\t" << HWID.GPU.at(i).XResolution << L"x" << HWID.GPU.at(i).YResolution << std::endl;
			std::wcout << L"Refresh Rate:\t\t" << HWID.GPU.at(i).RefreshRate << std::endl;

			if (i + 1 < HWID.GPU.size()) 
			{ 
				std::wcout << std::endl; 
			}
		}
	}

	/// <summary>
	///		Print CPU information
	/// </summary>
	void PrintCPU()
	{
		std::wcout << L"Processor Id:\t\t" << HWID.CPU.ProcessorId << std::endl;
		std::wcout << L"Manufacturer:\t\t" << HWID.CPU.Manufacturer << std::endl;
		std::wcout << L"Name:\t\t\t" << HWID.CPU.Name << std::endl;
		std::wcout << L"Cores:\t\t\t" << HWID.CPU.Cores << std::endl;
		std::wcout << L"Threads:\t\t" << HWID.CPU.Threads << std::endl;
	}

	/// <summary>
	///		Print network information
	/// </summary>
	void PrintNetwork() 
	{
		for (int i = 0; i < HWID.NetworkAdapter.size(); i++) 
		{
			std::wcout << L"Name:\t\t\t" << HWID.NetworkAdapter.at(i).Name << std::endl;
			std::wcout << L"MAC Address:\t\t" << HWID.NetworkAdapter.at(i).MAC << std::endl;

			if (i + 1 < HWID.NetworkAdapter.size()) 
			{ 
				std::wcout << std::endl; 
			}
		}
	}

	/// <summary>
	///		Print OS information
	/// </summary>
	void PrintSystem() 
	{
		std::wcout << L"System Name:\t\t" << HWID.System.Name << std::endl;
		std::wcout << L"Hypervisor Present:\t" << (HWID.System.IsHypervisorPresent ? L"Yes" : L"No") << std::endl;
		std::wcout << L"OS Version:\t\t" << HWID.System.OSVersion << std::endl;
		std::wcout << L"OS Title:\t\t" << HWID.System.OSName << std::endl;
		std::wcout << L"OS Architecture:\t" << HWID.System.OSArchitecture << std::endl;
		std::wcout << L"OS Serial Number:\t" << HWID.System.OSSerialNumber << std::endl;
	}

	/// <summary>
	///		Print memory information
	/// </summary>
	void PrintPhysicalMemory() 
	{
		std::wcout << L"Part Number:\t\t\t" << HWID.PhysicalMemory.PartNumber << std::endl;
		std::wcout << L"Total Physical Size:\t\t" << HWID.PhysicalMemory.TotalSize << L" MB" << std::endl;
		std::wcout << L"Available Physical Size:\t" << HWID.PhysicalMemory.AvailableSize << L" MB" << std::endl;
		std::wcout << L"Total Virtual Size:\t\t" << HWID.PhysicalMemory.TotalVirtualSize << L" MB" << std::endl;
		std::wcout << L"Available Virtual Size:\t\t" << HWID.PhysicalMemory.AvailableVirtualSize << L" MB" << std::endl;
		std::wcout << L"Total Page File Size:\t\t" << HWID.PhysicalMemory.TotalPageSize << L" MB" << std::endl;
		std::wcout << L"Available Page File Size:\t" << HWID.PhysicalMemory.AvailablePageSize << L" MB" << std::endl;
	}

	/// <summary>
	///		Print registry information
	/// </summary>
	void PrintRegistry() 
	{
		std::wcout << L"Computer Hardware Id:\t" << HWID.Registry.ComputerHardwareId << std::endl;
		std::wcout << L"Computer Manufacturer:\t" << HWID.Registry.ComputerManufacturer << std::endl;
		std::wcout << L"Computer Product Name:\t" << HWID.Registry.ComputerName << std::endl;
	}

	/// <summary>
	///		Save disks information
	/// </summary>
	void SaveDisks() 
	{
		for (int i = 0; i < HWID.Disk.size(); i++) 
		{
			statisticsFile << "Model: " << std::string(HWID.Disk.at(i).Model.begin(), HWID.Disk.at(i).Model.end()) << "\n";
			statisticsFile << "Serial Number: " << std::string(HWID.Disk.at(i).SerialNumber.begin(), HWID.Disk.at(i).SerialNumber.end()) << "\n";
			statisticsFile << "Interface Type: " << std::string(HWID.Disk.at(i).Interface.begin(), HWID.Disk.at(i).Interface.end()) << "\n";
			statisticsFile << "Drive Letter: " << std::string(HWID.Disk.at(i).DriveLetter.begin(), HWID.Disk.at(i).DriveLetter.end()) << "\n";
			statisticsFile << "Size: " << HWID.Disk.at(i).Size << " GB" << "\n";
			statisticsFile << "Free Space: " << HWID.Disk.at(i).FreeSpace << " GB" << "\n";
			statisticsFile << "Media Type: " <<(HWID.Disk.at(i).MediaType == 4 ? "SSD" : (HWID.Disk.at(i).MediaType == 3 ? "HDD" : "")) << "\n";
			statisticsFile << "Boot Drive: " << (HWID.Disk.at(i).IsBootDrive ? "Yes" : "No") << "\n";

			if (i + 1 < HWID.Disk.size()) 
			{ 
				statisticsFile << "\n"; 
			}
		}
	}

	/// <summary>
	///		Save SMBIOS information
	/// </summary>
	void SaveSMBIOS() 
	{
		statisticsFile << "Manufacturer: " << std::string(HWID.SMBIOS.Manufacturer.begin(), HWID.SMBIOS.Manufacturer.end()) << "\n";
		statisticsFile << "Product: " << std::string(HWID.SMBIOS.Product.begin(), HWID.SMBIOS.Product.end()) << "\n";
		statisticsFile << "Version: " << std::string(HWID.SMBIOS.Version.begin(), HWID.SMBIOS.Version.end()) << "\n";
		statisticsFile << "Serial Number: " << std::string(HWID.SMBIOS.SerialNumber.begin(), HWID.SMBIOS.SerialNumber.end()) << "\n";
	}

	/// <summary>
	///		Save GPU information
	/// </summary>
	void SaveGPUs() 
	{
		for (int i = 0; i < HWID.GPU.size(); i++) 
		{
			statisticsFile << "Name: " << std::string(HWID.GPU.at(i).Name.begin(), HWID.GPU.at(i).Name.end()) << "\n";
			statisticsFile << "Driver Version: " << std::string(HWID.GPU.at(i).DriverVersion.begin(), HWID.GPU.at(i).DriverVersion.end()) << "\n";
			statisticsFile << "Resolution: " << HWID.GPU.at(i).XResolution << "x" << HWID.GPU.at(i).YResolution << "\n";
			statisticsFile << "Refresh Rate: " << HWID.GPU.at(i).RefreshRate << "\n";

			if (i + 1 < HWID.GPU.size()) 
			{ 
				statisticsFile << "\n"; 
			}
		}
	}

	/// <summary>
	///		Save CPU information
	/// </summary>
	void SaveCPU() 
	{
		statisticsFile << "Processor Id: " << std::string(HWID.CPU.ProcessorId.begin(), HWID.CPU.ProcessorId.end()) << "\n";
		statisticsFile << "Manufacturer: " << std::string(HWID.CPU.Manufacturer.begin(), HWID.CPU.Manufacturer.end()) << "\n";
		statisticsFile << "Name: " << std::string(HWID.CPU.Name.begin(), HWID.CPU.Name.end()) << "\n";
		statisticsFile << "Cores: " << HWID.CPU.Cores << "\n";
		statisticsFile << "Threads: " << HWID.CPU.Threads << "\n";
	}

	/// <summary>
	///		Save network information
	/// </summary>
	void SaveNetwork() 
	{
		for (int i = 0; i < HWID.NetworkAdapter.size(); i++) 
		{
			statisticsFile << "Name: " << std::string(HWID.NetworkAdapter.at(i).Name.begin(), HWID.NetworkAdapter.at(i).Name.end()) << "\n";
			statisticsFile << "MAC Address: " << std::string(HWID.NetworkAdapter.at(i).MAC.begin(), HWID.NetworkAdapter.at(i).MAC.end()) << "\n";

			if (i + 1 < HWID.NetworkAdapter.size()) 
			{ 
				statisticsFile << "\n"; 
			}
		}
	}

	/// <summary>
	///		Save OS information
	/// </summary>
	void SaveSystem() 
	{
		statisticsFile << "System Name: " << std::string(HWID.System.Name.begin(), HWID.System.Name.end()) << "\n";
		statisticsFile << "Hypervisor Present: " << (HWID.System.IsHypervisorPresent ? "Yes" : "No") << "\n";
		statisticsFile << "OS Version: " << std::string(HWID.System.OSVersion.begin(), HWID.System.OSVersion.end()) << "\n";
		statisticsFile << "OS Title: " << std::string(HWID.System.OSName.begin(), HWID.System.OSName.end()) << "\n";
		statisticsFile << "OS Architecture: " << std::string(HWID.System.OSArchitecture.begin(), HWID.System.OSArchitecture.end()) << "\n";
		statisticsFile << "OS Serial Number: " << std::string(HWID.System.OSSerialNumber.begin(), HWID.System.OSSerialNumber.end()) << "\n";
	}

	/// <summary>
	///		Save memory information
	/// </summary>
	void SavePhysicalMemory() 
	{
		statisticsFile << "Part Number: " << std::string(HWID.PhysicalMemory.PartNumber.begin(), HWID.PhysicalMemory.PartNumber.end()) << "\n";
		statisticsFile << "Total Physical Size: " << HWID.PhysicalMemory.TotalSize << " MB" << "\n";
		statisticsFile << "Available Physical Size: " << HWID.PhysicalMemory.AvailableSize << " MB" << "\n";
		statisticsFile << "Total Virtual Size: " << HWID.PhysicalMemory.TotalVirtualSize << " MB" << "\n";
		statisticsFile << "Available Virtual Size: " << HWID.PhysicalMemory.AvailableVirtualSize << " MB" << "\n";
		statisticsFile << "Total Page File Size: " << HWID.PhysicalMemory.TotalPageSize << " MB" << "\n";
		statisticsFile << "Available Page File Size: " << HWID.PhysicalMemory.AvailablePageSize << " MB" << "\n";
	}

	/// <summary>
	///		Save registry information
	/// </summary>
	void SaveRegistry() 
	{
		statisticsFile << "Computer Hardware Id: " << std::string(HWID.Registry.ComputerHardwareId.begin(), HWID.Registry.ComputerHardwareId.end()) << "\n";
		statisticsFile << "Computer Manufacturer: " << std::string(HWID.Registry.ComputerManufacturer.begin(), HWID.Registry.ComputerManufacturer.end()) << "\n";
		statisticsFile << "Computer Product Name: " << std::string(HWID.Registry.ComputerName.begin(), HWID.Registry.ComputerName.end()) << "\n";
	}

	/// <summary>
	///		Get valid subcommands
	/// </summary>
	/// 
	/// <returns>std::vector <std::wstring></returns>
	std::vector <std::wstring> ValidSubCommands() 
	{
		switch (ParsedCommand.CommandIndex) 
		{
		case eDisk: 
		{
			return DiskSubCommands;
		} break;

		case eSMBIOS: 
		{
			return SMBIOSSubCommands;
		} break;

		case eGPU:
		{
			return GPUSubCommands;
		} break;

		case eCPU: 
		{
			return CPUSubCommands;
		} break;

		case eNetwork: 
		{
			return NetworkSubCommands;
		} break;

		case eSystem: 
		{
			return SystemSubCommands;
		} break;

		case ePhysicalMemory: 
		{
			return PhysicalMemorySubCommands;
		} break;

		case eRegistry: 
		{
			return RegistrySubCommands;
		} break;
		}

		return {};
	}

	/// <summary>
	///		Parse command
	/// </summary>
	void ParseCommand() 
	{
		bool bFound { false };

		// Order command parts
		auto OrderVector{ [](std::vector <int> ToOrder) -> std::vector <int> {
			struct MinToMax 
			{
				MinToMax(const std::vector <int>& Vector) : _Vector(Vector) {}

				bool operator () (int Index, int Index2) 
				{
					return _Vector[Index] < _Vector[Index2];
				}

				const std::vector <int>& _Vector {};
			};

			std::vector <int> OrderedElements {};
			OrderedElements.resize(ToOrder.size());

			for (int i = 0; i < ToOrder.size(); ++i) 
			{
				OrderedElements.at(i) = i;
			}

			std::sort(OrderedElements.begin(), OrderedElements.end(), MinToMax(ToOrder));

			return OrderedElements;
		} };

		// Convert the command to lowercase to avoid any case issues
		for (int i = 0; i < CurCmd.size(); i++) 
		{
			CurCmd.at(i) = std::tolower(CurCmd.at(i));
		}

		// Determine if the start of the command contains a valid main command by looping through our vector of valid commands
		for (; ParsedCommand.CommandIndex < ValidCommands.size(); ParsedCommand.CommandIndex++) 
		{
			if (CurCmd.find(ValidCommands.at(ParsedCommand.CommandIndex)) == 0) {
				bFound = true;
				break;
			}
		}

		// The command is invalid
		if (!bFound) 
		{
			ParsedCommand.CommandIndex = eInvalid;

			return;
		}

		// Determine if the command contains a "get" request by checking for the "get" keyword
		if (CurCmd.find(L"get") != std::wstring::npos && CurCmd.size() != ValidCommands.at(ParsedCommand.CommandIndex).size() + 3) 
		{
			// Determine if "get" is directly after the main command. If it's not, it's an invalid command
			if (CurCmd.find(L"get") != ValidCommands.at(ParsedCommand.CommandIndex).size()) 
			{
				ParsedCommand.CommandIndex = eInvalid;

				return;
			}

			// If sub command has two and more arguments
			std::vector <std::wstring> SubCmds { ValidSubCommands() };
			std::vector <int> UnorderedSubIndex {};
			std::vector <int> Offsets {};

			for (int i = 0; i < SubCmds.size(); i++) 
			{
				auto Offset { CurCmd.find(SubCmds.at(i)) };

				if (Offset != std::wstring::npos) 
				{
					UnorderedSubIndex.push_back(i);
					Offsets.push_back(Offset);
				}
			}

			// No valid sub-commands, but "get" is present - invalid command
			if (UnorderedSubIndex.size() == 0 || Offsets.size() == 0) 
			{
				ParsedCommand.CommandIndex = eInvalid;

				return;
			}

			// Check if it's valid command
			auto OrderedOffsets { OrderVector(Offsets) };
			if (Offsets.at(OrderedOffsets.at(0)) != ValidCommands.at(ParsedCommand.CommandIndex).size() + 3) 
			{
				ParsedCommand.CommandIndex = eInvalid;
				return;
			}

			// Sort sub-command indices from closest to farthest relative to "get"
			ParsedCommand.SubCommandIndex.resize(UnorderedSubIndex.size());

			for (int i = 0; i < UnorderedSubIndex.size(); i++) 
			{
				ParsedCommand.SubCommandIndex.at(i) = UnorderedSubIndex.at(OrderedOffsets.at(i));
			}
		}
		// Invalid command
		else if (CurCmd.size() > ValidCommands.at(ParsedCommand.CommandIndex).size()) 
		{
			ParsedCommand.CommandIndex = eInvalid;
		}
	}

	/// <summary>
	///		Respond command
	/// </summary>
	void RespondCommand() 
	{
		ParseCommand();

		// Handle subcommands if present
		if (ParsedCommand.SubCommandIndex.size()) 
		{
			std::wcout << std::endl;

			switch (ParsedCommand.CommandIndex) 
			{
			// Disk subcommand
			case eDisk: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++) 
						{
							FmtPrint(HWID.Disk.at(j).SerialNumber, eDisk, j);
						}
					} break;

					case 2: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++) 
						{
							FmtPrint(HWID.Disk.at(j).Model, eDisk, j);
						}
					} break;

					case 3: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++) 
						{
							FmtPrint(HWID.Disk.at(j).Interface, eDisk, j);
						}
					} break;

					case 4: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++)
						{
							FmtPrint(HWID.Disk.at(j).DriveLetter, eDisk, j);
						}
					} break;

					case 5: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++)
						{
							FmtPrint(HWID.Disk.at(j).Size, eDisk, j);
						}
					} break;

					case 6: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++) 
						{
							FmtPrint(HWID.Disk.at(j).FreeSpace, eDisk, j);
						}
					} break;

					case 7: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++) 
						{
							FmtPrint(HWID.Disk.at(j).MediaType, eDisk, j);
						}
					} break;

					case 8: 
					{
						for (int j = 0; j < HWID.Disk.size(); j++) 
						{
							FmtPrint(HWID.Disk.at(j).IsBootDrive, eDisk, j);
						}
					} break;
					}
				}
			} break;

			// SMBIOS subcommand
			case eSMBIOS: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						std::wcout << HWID.SMBIOS.SerialNumber << std::endl;
					} break;

					case 2: 
					{
						std::wcout << HWID.SMBIOS.Manufacturer << std::endl;
					} break;

					case 3: 
					{
						std::wcout << HWID.SMBIOS.Product << std::endl;
					} break;

					case 4: 
					{
						std::wcout << HWID.SMBIOS.Version << std::endl;
					} break;
					}
				}
			} break;

			// GPU subcommand
			case eGPU: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						for (int i = 0; i < HWID.GPU.size(); i++) 
						{
							FmtPrint(HWID.GPU.at(i).Name, eGPU, i);
						}
					} break;

					case 2: 
					{
						for (int i = 0; i < HWID.GPU.size(); i++) 
						{
							FmtPrint(HWID.GPU.at(i).DriverVersion, eGPU, i);
						}
					} break;

					case 3: 
					{
						for (int i = 0; i < HWID.GPU.size(); i++) 
						{
							std::wstring Res(std::to_wstring(HWID.GPU.at(i).XResolution));
							Res.append(L"x").append(std::to_wstring(HWID.GPU.at(i).YResolution));

							FmtPrint(Res, eGPU, i);
						}
					} break;

					case 4: 
					{
						for (int i = 0; i < HWID.GPU.size(); i++) 
						{
							FmtPrint(HWID.GPU.at(i).RefreshRate, eGPU, i);
						}
					} break;
					}
				}
			} break;

			// CPU subcommand
			case eCPU: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						std::wcout << HWID.CPU.ProcessorId << std::endl;
					} break;

					case 2: 
					{
						std::wcout << HWID.CPU.Manufacturer << std::endl;
					} break;

					case 3: 
					{
						std::wcout << HWID.CPU.Name << std::endl;
					} break;

					case 4: 
					{
						std::wcout << HWID.CPU.Cores << std::endl;
					} break;

					case 5: 
					{
						std::wcout << HWID.CPU.Threads << std::endl;
					} break;
					}
				}
			} break;

			// Network subcommand
			case eNetwork: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						for (int i = 0; i < HWID.NetworkAdapter.size(); i++)
						{
							FmtPrint(HWID.NetworkAdapter.at(i).Name, eNetwork, i);
						}
					} break;

					case 2: 
					{
						for (int i = 0; i < HWID.NetworkAdapter.size(); i++) 
						{
							FmtPrint(HWID.NetworkAdapter.at(i).MAC, eNetwork, i);
						}
					} break;
					}
				}
			} break;

			// OS subcommand
			case eSystem: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						std::wcout << HWID.System.Name << std::endl;
					} break;

					case 2: 
					{
						std::wcout << HWID.System.IsHypervisorPresent << std::endl;
					} break;

					case 3: 
					{
						std::wcout << HWID.System.OSVersion << std::endl;
					} break;

					case 4: 
					{
						std::wcout << HWID.System.OSName << std::endl;
					} break;

					case 5: 
					{
						std::wcout << HWID.System.OSArchitecture << std::endl;
					} break;

					case 6: 
					{
						std::wcout << HWID.System.OSSerialNumber << std::endl;
					} break;
					}
				}
			} break;

			// Memory subcommand
			case ePhysicalMemory: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						std::wcout << HWID.PhysicalMemory.PartNumber << std::endl;
					} break;
					case 2: 
					{
						std::wcout << HWID.PhysicalMemory.TotalSize << L" MB" << std::endl;
					} break;
					case 3: 
					{
						std::wcout << HWID.PhysicalMemory.AvailableSize << L" MB" << std::endl;
					} break;
					case 4:
					{
						std::wcout << HWID.PhysicalMemory.TotalVirtualSize << L" MB" << std::endl;
					} break;
					case 5:
					{
						std::wcout << HWID.PhysicalMemory.AvailableVirtualSize << L" MB" << std::endl;
					} break;
					case 6:
					{
						std::wcout << HWID.PhysicalMemory.TotalPageSize << L" MB" << std::endl;
					} break;
					case 7:
					{
						std::wcout << HWID.PhysicalMemory.AvailablePageSize << L" MB" << std::endl;
					} break;
					}
				}
			} break;

			// Registry subcommand
			case eRegistry: 
			{
				for (int i = 0; i < ParsedCommand.SubCommandIndex.size(); i++) 
				{
					switch (ParsedCommand.SubCommandIndex.at(i)) 
					{
					case 1: 
					{
						std::wcout << HWID.Registry.ComputerHardwareId << std::endl;
					} break;
					case 2:
					{
						std::wcout << HWID.Registry.ComputerManufacturer << std::endl;
					} break;
					case 3:
					{
						std::wcout << HWID.Registry.ComputerName << std::endl;
					} break;
					}
				}
			} break;
			}

			goto ClearCmd;
		}

		// Handle main command
		switch (ParsedCommand.CommandIndex) 
		{
		// Music on
		case eMusicOn: 
		{
			if (!PlaySound(MUSIC_FILE, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC | SND_NODEFAULT)) 
			{
				HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(handle, FOREGROUND_RED);

				std::wcout << L"\nError! Music file not found...\n";
			}
			else
			{
				std::wcout << L"\nMusic on!\n";
			}
		} break;

		// Music off
		case eMusicOff: 
		{
			if (!PlaySound(NULL, NULL, SND_FILENAME)) 
			{
				HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(handle, FOREGROUND_RED);

				std::wcout << L"\nError! Can't stop music file...\n";
			}
			else
			{
				std::wcout << "\nMusic off!\n";
			}
		} break;

		// Disks information
		case eDisk: 
		{
			std::wcout << std::endl;
			PrintDisks();
		} break;

		// SMBIOS information
		case eSMBIOS: 
		{
			std::wcout << std::endl;
			PrintSMBIOS();
		} break;

		// GPU information
		case eGPU: 
		{
			std::wcout << std::endl;
			PrintGPUs();
		} break;

		// CPU information
		case eCPU: 
		{
			std::wcout << std::endl;
			PrintCPU();
		} break;

		// Network information
		case eNetwork: 
		{
			std::wcout << std::endl;
			PrintNetwork();
		} break;

		// OS information
		case eSystem: 
		{
			std::wcout << std::endl;
			PrintSystem();
		} break;

		// Memory information
		case ePhysicalMemory: 
		{
			std::wcout << std::endl;
			PrintPhysicalMemory();
		} break;

		// Registry information
		case eRegistry: 
		{
			std::wcout << std::endl;
			PrintRegistry();
		} break;

		// All information
		case eAll: 
		{
			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|          Disks         |\n";
			std::wcout << L"--------------------------\n\n";
			PrintDisks();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|         SMBIOS         |\n";
			std::wcout << L"--------------------------\n\n";
			PrintSMBIOS();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|          GPUs          |\n";
			std::wcout << L"--------------------------\n\n";
			PrintGPUs();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|           CPU          |\n";
			std::wcout << L"--------------------------\n\n";
			PrintCPU();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|         Network        |\n";
			std::wcout << L"--------------------------\n\n";
			PrintNetwork();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|         System         |\n";
			std::wcout << L"--------------------------\n\n";
			PrintSystem();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|          Memory        |\n";
			std::wcout << L"--------------------------\n\n";
			PrintPhysicalMemory();

			std::wcout << L"\n--------------------------\n";
			std::wcout << L"|        Registry        |\n";
			std::wcout << L"--------------------------\n\n";
			PrintRegistry();
		} break;

		// CPU and memory load
		case eRealTime: 
		{
			std::ofstream logFile;
			logFile.open(LOG_FILE, std::ios::out | std::ios::app);

			// While CTRL + Z isn't pressed
			while (!((GetKeyState(VK_CONTROL) & 0x80) & (GetKeyState(VK_Z) & 0x80)))
			{
				// Get current time
				auto timestamp = std::chrono::system_clock::now();
				time_t time = std::chrono::system_clock::to_time_t(timestamp);

				// Get CPU and memory load
				float cpuLoad = HWID.GetCPULoad() * 100;
				float memoryLoad = HWID.GetMemoryLoad();

				// Console print
				std::wcout << L"\nCurrent time: " << std::ctime(&time) 
					<< L"CPU load: " << cpuLoad << L"%, Memory load: " << memoryLoad << L"%\n";
				
				// File print
				logFile << "\nSaved time: " << std::ctime(&time)
					<< "CPU load: " << cpuLoad << "%, Memory load: " << memoryLoad << "%\n";

				Sleep(1000);
			}

			logFile.close();

			std::wcout << L"\nLogs were saved in logs/logs.csv!\n";
		} break;

		// Save all information
		case eSave: 
		{
			statisticsFile.open(STATISTICS_FILE, std::ios::out | std::ios::app);

			// Get current time
			auto timestamp = std::chrono::system_clock::now();
			time_t time = std::chrono::system_clock::to_time_t(timestamp);

			statisticsFile << "\nSaved time: " << std::ctime(&time);

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "           Disks          \n";
			statisticsFile << "--------------------------\n\n";
			SaveDisks();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "          SMBIOS          \n";
			statisticsFile << "--------------------------\n\n";
			SaveSMBIOS();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "           GPUs           \n";
			statisticsFile << "--------------------------\n\n";
			SaveGPUs();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "            CPU           \n";
			statisticsFile << "--------------------------\n\n";
			SaveCPU();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "          Network         \n";
			statisticsFile << "--------------------------\n\n";
			SaveNetwork();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "          System          \n";
			statisticsFile << "--------------------------\n\n";
			SaveSystem();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "           Memory         \n";
			statisticsFile << "--------------------------\n\n";
			SavePhysicalMemory();

			statisticsFile << "\n--------------------------\n";
			statisticsFile << "         Registry         \n";
			statisticsFile << "--------------------------\n\n";
			SaveRegistry();

			statisticsFile.close();

			std::wcout << L"\nStatistics were saved in logs/statistics.csv!\n";
		} break;

		// Get help page
		case eHelp: 
		{
			ShellExecute(NULL, L"open", HELP_FILE, NULL, NULL, SW_SHOW);

			std::wcout << L"\nHelp page was opened!\n";
		} break;

		// Exit
		case eExit: 
		{
			exit(0);
		} break;

		// Invalid command
		default: 
		{
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(handle, FOREGROUND_RED);

			std::wcout << L"\nError! Invalid command...\n";
		} break;
		}

	// Clear command
	ClearCmd:
		ParsedCommand.CommandIndex = eInvalid;
		ParsedCommand.SubCommandIndex.resize(0);
	}

	/// <summary>
	///		Create command line
	/// </summary>
	void Create() 
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, FOREGROUND_GREEN);

		std::wcout << AppName;

		// Respond commands
		auto NewLine{ []() -> void {
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(handle, FOREGROUND_GREEN);

			std::wcout << CmdName;

			SetConsoleTextAttribute(handle, FOREGROUND_WHITE);

			std::getline(std::wcin, CurCmd);

			ComputerStatistics::RemoveWhitespaces(CurCmd);
			if (CurCmd.empty()) 
			{ 
				return; 
			}

			bool bFmt 
			{
				(CurCmd.find(L"diskget") != std::wstring::npos && HWID.Disk.size() > 1
				|| CurCmd.find(L"gpuget") != std::wstring::npos && HWID.GPU.size() > 1
				|| CurCmd.find(L"networkget") != std::wstring::npos && HWID.NetworkAdapter.size() > 1)
			};

			RespondCommand();

			if (!bFmt) 
			{
				std::wcout << std::endl;
			}
		} };
		
		// Get all information
		HWID = ComputerStatistics();

		// Enter a new command
		for (;;) 
		{
			NewLine();
		}
	}
};
