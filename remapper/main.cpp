#include <cstdio>
#include <iostream>
#include <utility>
#include <map>
#include <dinput.h>

bool program_aborted = false;
bool unsaved_changes = false;
IDirectInput* di = nullptr;
IDirectInputDevice* did_kbd = nullptr;

void remap();
void write();
void list();
void quit();
DWORD getPressedKey();
static std::map<DWORD, DWORD>& getRemaps();

#define ABORT_ON_FAILED(x) \
	if (FAILED(x)) do { printf("DI8 failed on line %d\n", __LINE__); exit(1); } while (false)
		

int main()
{
	ABORT_ON_FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&di, nullptr));
	ABORT_ON_FAILED(di->CreateDevice(GUID_SysKeyboard, &did_kbd, nullptr));
	ABORT_ON_FAILED(did_kbd->SetDataFormat(&c_dfDIKeyboard));
	ABORT_ON_FAILED(did_kbd->SetCooperativeLevel(GetConsoleWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
	ABORT_ON_FAILED(did_kbd->Acquire());

	printf("Welcome to Atelier keyremapper\n");

	while (!program_aborted)
	{
		printf("Available commands: r=remap, w=write changes, l=list remaps, q=quit\n");
		char c;
		std::cin >> c;
		switch (c)
		{
		case 'r':
		case 'R':
			remap();
			break;
		case 'w':
		case 'W':
			write();
			break;
		case 'l':
		case 'L':
			list();
			break;
		case 'q':
		case 'Q':
			quit();
			break;
		default:
			printf("Unrecognized command.\n");
			break;
		}
	}
}

void remap()
{
	DWORD wd;
	unsigned char from, to;

	while (getPressedKey() != -1);

	printf("Press incoming key (what you hit on your keyboard)...\n");
	while ((wd = getPressedKey()) == -1);
	from = (unsigned char)wd;
	printf("Pressed %d\n", from);

	while (getPressedKey() != -1);

	printf("Press ouput key (what gets sent to Atelier)...\n");
	while ((wd = getPressedKey()) == -1);
	to = (unsigned char)wd;
	printf("Pressed %d\n", to);

	auto& remaps = getRemaps();
	if (remaps.count(from) != 0)
	{
		printf("That key is already mapped. Overwrite? (Y/n)\n");
		char c;
		std::cin >> c;
		if (c != 'y' && c != 'Y')
		{
			printf("Maps unchanged.\n");
			return;
		}
	}

	remaps[from] = to;
	printf("Remapped %d -> %d\n", from, to);
	unsaved_changes = true;
}

void write()
{
	if (FILE* f = fopen("atelier.remap", "wb"))
	{
		auto& remaps = getRemaps();

		int size = (int)remaps.size();
		fwrite(&size, sizeof(int), 1, f);
		for (auto& entry : remaps)
		{
			unsigned char map[2] = { (unsigned char)entry.first, (unsigned char)entry.second };
			fwrite(&map, 1, 2, f);
		}

		fclose(f);
		unsaved_changes = false;
	}
}

void list()
{
	auto& remaps = getRemaps();
	printf("Num remaps: %d\n", (int)remaps.size());
	for (auto& entry : remaps)
		printf("%d -> %d\n", entry.first, entry.second);
}

void quit()
{
	if (unsaved_changes)
	{
		printf("You have unsaved changes. Quit anyway? (Y/n) ");
		char c;
		std::cin >> c;
		if (c != 'y' && c != 'Y')
			return;
	}

	program_aborted = true;
}

DWORD getPressedKey()
{
	unsigned char keyStates[256];
	did_kbd->GetDeviceState(256, keyStates);

	for (int i = 0; i < 256; ++i)
	{
		if (keyStates[i] & 0x80)
			return i;
	}

	return -1;
}

static std::map<DWORD, DWORD>& getRemaps()
{
	static bool initalized = false;
	static std::map<DWORD, DWORD> remaps;
	if (initalized)
		return remaps;

	initalized = true;

	if (FILE* f = fopen("atelier.remap", "rb"))
	{
		int numEntries = 0;
		fread(&numEntries, sizeof(int), 1, f);
		for (int i = 0; i < numEntries; ++i)
		{
			unsigned char map[2] = { 0 };
			fread(map, 1, 2, f);
			remaps[map[0]] = map[1];
		}
		fclose(f);
	}

	return remaps;
}
