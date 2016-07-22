#pragma once
#include <windows.h>
#include <cstdint>

class AddressSelectorBase
{
protected:
	enum class GameVersion
	{
		GVUNINITIALIZED,
		GV10,
		GVSTEAM,
		GVUNK,
	};

	static GameVersion m_gv;
};

template <std::uintptr_t __EP10, std::uintptr_t __EPSTEAMENC, std::uintptr_t __EPSTEAMDEC>
class AddressSelector :public AddressSelectorBase
{
public:
	AddressSelector()
	{
		if (m_gv == GameVersion::GVUNINITIALIZED)
		{
			std::uintptr_t base = (std::uintptr_t)GetModuleHandleW(NULL);
			IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)(base);
			IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + dos->e_lfanew);

			switch (nt->OptionalHeader.AddressOfEntryPoint + 0x400000)
			{
			case __EP10:
				m_gv = GameVersion::GV10;
				break;
			case __EPSTEAMENC:
			case __EPSTEAMDEC:
				m_gv = GameVersion::GVSTEAM;
				break;
			default:
				m_gv = GameVersion::GVUNK;
				break;
			}
		}
	}

	template <std::uintptr_t __Addr10, std::uintptr_t __AddrSteam, typename __DestType = void>
	__DestType *SelectAddress()
	{
		if (m_gv == GameVersion::GV10)
		{
			return ((__DestType *)__Addr10);
		}
		else if (m_gv == GameVersion::GVSTEAM)
		{
			return ((__DestType *)__AddrSteam);
		}
		else
		{
			return nullptr;
		}
	}
};

typedef AddressSelector<0x5C1E70, 0x9912ED, 0x5C6FD0> AddressSelectorLC;
typedef AddressSelector<0x667BF0, 0xA402ED, 0x666BA0> AddressSelectorVC;
